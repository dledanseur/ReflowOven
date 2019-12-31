#ifndef BUZZERMANAGER_HPP
#define BUZZERMANAGER_HPP

#include <Arduino.h>

class BuzzerManager {
  private:
    uint8_t buzzerPin;
    int channel;
    int resolution;
    int frequency;
  public:
    BuzzerManager(uint8_t buzzerPin);
    void singleTone(unsigned int frequency, unsigned long duration);
    void triTone(unsigned int frequency, int increment, unsigned long singleDuration);
};

#endif