#ifndef TEMPERATUREMANAGER_HPP
#define TEMPERATUREMANAGER_HPP

#include <Arduino.h>
#include <SPI.h>

class TemperatureManager {
  private:
    double thermcpl_temp                   = 0.0;                          // thermocouple temperature data
    double coldj_temp                      = 0.0;                          // cold-junction temperature data
    uint8_t clk_pin;
    uint8_t cs_pin;
    uint8_t miso_pin;
    SPIClass* spi;
    double readInternal(int32_t v);
    double readCelsius(int32_t v);
    double readFarenheit(int32_t v);
    uint8_t readError(int32_t v);
    uint32_t spiread32(void);

 public:

  public:
    TemperatureManager(uint8_t clk_pin, uint8_t cs_pin, uint8_t miso_pin);
    uint8_t read(void);
    double getThermcplTemp() { return thermcpl_temp; };
    double getColdjTemp() { return coldj_temp; };
    ~TemperatureManager();

};

#endif