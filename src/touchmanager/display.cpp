#include "touchmanager/display.h"

Display::~Display() {}

TftDisplay::TftDisplay(TFT_eSPI& tft): m_tft(tft) {}

void TftDisplay::pushImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* data) {
    m_tft.pushImage(x, y, w, h, data);
}

void TftDisplay::drawString(const String &string, uint16_t x, uint16_t y, uint16_t textSize, uint16_t color) {
    m_tft.setTextColor(color);
    m_tft.setTextSize(textSize);
    m_tft.setCursor(x, y);
    m_tft.print(string);
}

void TftDisplay::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    m_tft.fillRect(x, y, w, h, color);
}

TftDisplay::~TftDisplay() {}

SpriteDisplay::SpriteDisplay(CustomSprite& sprite): m_sprite(sprite) {}

void SpriteDisplay::pushImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* data) {
    m_sprite.pushImage(x, y, w, h, data);
}

void SpriteDisplay::drawString(const String& string, uint16_t x, uint16_t y, uint16_t textSize, uint16_t color) {
    m_sprite.setTextColor(color);
    m_sprite.setTextSize(textSize);
    m_sprite.setCursor(x, y);
    m_sprite.print(string);
}
void SpriteDisplay::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    m_sprite.fillRect(x, y, w, h, color);
}

SpriteDisplay::~SpriteDisplay() {}