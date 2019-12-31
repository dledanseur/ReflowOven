#ifndef TEMPERATUREMANAGER_HPP
#define TEMPERATUREMANAGER_HPP

#include <Arduino.h>
#include <MAX31855.h>
#include <SPI.h>

#include <Adafruit_MAX31855.h>

class TemperatureManager {
  private:
    double thermcpl_temp                   = 0.0;                          // thermocouple temperature data
    double coldj_temp                      = 0.0;                          // cold-junction temperature data
    uint8_t clk_pin;
    uint8_t cs_pin;
    uint8_t miso_pin;
    Adafruit_MAX31855* thermocouple;
    int32_t readRawData();
  public:
    TemperatureManager(uint8_t clk_pin, uint8_t cs_pin, uint8_t miso_pin);
    uint8_t read(void);
    double getThermcplTemp() { return thermcpl_temp; };
    double getColdjTemp() { return coldj_temp; };
    ~TemperatureManager();

};

#endif