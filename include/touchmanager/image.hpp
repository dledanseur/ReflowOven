#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "Arduino.h"
#include "TFT_eSPI.h"
#include "display.h"

class Image{
    private:
    uint8_t* data;
    uint16_t w;
    uint16_t h;

    public:
    Image(uint8_t* data, uint16_t w, uint16_t h);
    void display(Display& tft, uint16_t x, uint16_t y);
    uint16_t getW();
    uint16_t getH();
};

#endif