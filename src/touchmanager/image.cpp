#include "touchmanager/image.hpp"
#include <ArduinoLog.h>

Image::Image(uint8_t* data, uint16_t w, uint16_t h) {
    this->data = data;
    this->w = w;
    this->h = h;
}

void Image::display(Display& tft, uint16_t x, uint16_t y) {
    tft.pushImage(x,y,w,h, this->data);
}

uint16_t Image::getW() {
    return this->w;
}

uint16_t Image::getH() {
    return this->h;
}
