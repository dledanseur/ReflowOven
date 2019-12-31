#include <ArduinoLog.h>
#include <TFT_eSPI.h>

#ifndef CUSTOMSPRITE_HPP
#define CUSTOMSPRITE_HPP

class CustomSprite: public TFT_eSprite {
  public:
    CustomSprite(TFT_eSPI* tft);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data);
};
#endif