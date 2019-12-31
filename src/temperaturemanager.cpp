#include "temperaturemanager.hpp"
#include <SPI.h>
#include <ArduinoLog.h>

static const int spiClk = 5000000UL;

#define MAX31855_CONVERSION_TIME            100    //in milliseconds, 9..10Hz sampling rate 

TemperatureManager::TemperatureManager(uint8_t clk_pin, uint8_t cs_pin, uint8_t miso_pin): 
              clk_pin(clk_pin), cs_pin(cs_pin), miso_pin(miso_pin) {

  //thermocouple = new Adafruit_MAX31855(clk_pin,cs_pin,miso_pin);
  thermocouple = new Adafruit_MAX31855(clk_pin, cs_pin, miso_pin);
  
  thermocouple->begin();

  

}

TemperatureManager::~TemperatureManager() {
  delete thermocouple;
  thermocouple = NULL;
}


/*
  error_status
    Bit 3   Fault
    Bit 2   Short to VCC
    Bit 1   Short to GND
    Bit 0   Open Circuit
*/
uint8_t TemperatureManager::read(void)
{
  thermcpl_temp = thermocouple->readCelsius();
  uint8_t error  = thermocouple->readError();
  coldj_temp = thermocouple->readInternal();

  Log.verbose("Thermocouple Error %i, temperature %D, internal %D" CR, error, thermcpl_temp, coldj_temp);
  
  return error;
}
