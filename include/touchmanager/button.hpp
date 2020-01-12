#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>
#include "image.hpp"
#include "touch_manager.hpp"
#include "display.h"


class TouchManager;
class Command;

class Button {
    friend class TouchManager;

    private:
    Image& img;
    uint16_t x;
    uint16_t y;
    Display& tft;
    Command& command;
    bool hidden;


    public:
    Button(Image& img, uint16_t x, uint16_t y, Display& tft, Command& command, bool hidden=false);
    boolean isTouchInside(uint16_t x, uint16_t y);
    boolean maybeTrigger(uint16_t x, uint16_t y, TouchManager& manager);
    void display();
    void setHidden(bool hidden);
};
#endif