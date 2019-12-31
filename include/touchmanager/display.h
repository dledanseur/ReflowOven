#include <Arduino.h>
#include "TFT_eSPI.h"
#include "custom_sprite.h"

#ifndef DISPLAY_HPP
#define DISPLAY_HPP

class Display {
  public:
    virtual void pushImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* data);
    virtual void drawString(const String& string, uint16_t x, uint16_t y, uint16_t textSize, uint16_t color=TFT_WHITE);
    virtual void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    virtual ~Display();
};

class TftDisplay: public Display {
  private:
    TFT_eSPI& m_tft;
  public:
    TftDisplay(TFT_eSPI& tft);
    virtual void pushImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* data);
    virtual void drawString(const String& string, uint16_t x, uint16_t y, uint16_t textSize, uint16_t color=TFT_WHITE);
    virtual void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    virtual ~TftDisplay();
};

class SpriteDisplay: public Display {
  private:
    CustomSprite& m_sprite;
  public:
    SpriteDisplay(CustomSprite& sprite);
    virtual void pushImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* data);
    virtual void drawString(const String& string, uint16_t x, uint16_t y, uint16_t textSize, uint16_t color=TFT_WHITE);
    virtual void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    virtual ~SpriteDisplay();
};


#endif