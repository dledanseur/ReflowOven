#include "temperaturemanager.hpp"
#include <SPI.h>
#include <ArduinoLog.h>

static const int spiClk = 5000000UL;

#define MAX31855_CONVERSION_TIME            100    //in milliseconds, 9..10Hz sampling rate 

TemperatureManager::TemperatureManager(uint8_t clk_pin, uint8_t cs_pin, uint8_t miso_pin): 
              clk_pin(clk_pin), cs_pin(cs_pin), miso_pin(miso_pin) {

  //thermocouple = new Adafruit_MAX31855(clk_pin,cs_pin,miso_pin);
  //spi = &SPI;
  spi = new SPIClass(HSPI);
  //spi->begin(18, 19, 23, 15);
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);
  spi->begin();

}

TemperatureManager::~TemperatureManager() {
  delete spi;
  spi = NULL;
}


/*
  error_status
    Bit 3   Fault
    Bit 2   Short to VCC
    Bit 1   Short to GND
    Bit 0   Open Circuit
*/
uint32_t TemperatureManager::spiread32(void) {
  uint32_t d = 0;

  digitalWrite(cs_pin, LOW);
  delay(1);

  // hardware SPI

  spi->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  d = spi->transfer(0);
  d <<= 8;
  d |= spi->transfer(0);
  d <<= 8;
  d |= spi->transfer(0);
  d <<= 8;
  d |= spi->transfer(0);

  spi->endTransaction();

  digitalWrite(cs_pin, HIGH);
  //Serial.println(d, HEX);
  Log.verbose("Raw thermocouple data %i" CR, d);
  return d;
}

double TemperatureManager::readCelsius(int32_t v) {

  //Serial.print("0x"); Serial.println(v, HEX);

  /*
  float internal = (v >> 4) & 0x7FF;
  internal *= 0.0625;
  if ((v >> 4) & 0x800) 
    internal *= -1;
  Serial.print("\tInternal Temp: "); Serial.println(internal);
  */

  if (v & 0x7) {
    // uh oh, a serious problem!
    return NAN; 
  }

  if (v & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    v = 0xFFFFC000 | ((v >> 18) & 0x00003FFFF);
  }
  else {
    // Positive value, just drop the lower 18 bits.
    v >>= 18;
  }
  //Serial.println(v, HEX);
  
  double centigrade = v;

  // LSB = 0.25 degrees C
  centigrade *= 0.25;
  return centigrade;
}

uint8_t TemperatureManager::readError(int32_t v) {
  return v & 0x7;
}

double TemperatureManager::readFarenheit(int32_t v) {
  float f = readCelsius(v);
  f *= 9.0;
  f /= 5.0;
  f += 32;
  return f;
}

double TemperatureManager::readInternal(int32_t v) {

  // ignore bottom 4 bits - they're just thermocouple data
  v >>= 4;

  // pull the bottom 11 bits off
  float internal = v & 0x7FF;
  // check sign bit!
  if (v & 0x800) {
    // Convert to negative value by extending sign and casting to signed type.
    int16_t tmp = 0xF800 | (v & 0x7FF);
    internal = tmp;
  }
  internal *= 0.0625; // LSB = 0.0625 degrees
  //Serial.print("\tInternal Temp: "); Serial.println(internal);
  return internal;
}

uint8_t TemperatureManager::read(void)
{
  int32_t v = spiread32();
  thermcpl_temp = this->readCelsius(v);
  uint8_t error  = this->readError(v);
  coldj_temp = this->readInternal(v);

  Log.verbose("Thermocouple Error %i, temperature %D, internal %D" CR, error, thermcpl_temp, coldj_temp);
  
  return error;
}
