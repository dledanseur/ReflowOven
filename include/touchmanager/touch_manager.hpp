#include <stdlib.h>
#include <map>

#ifndef TOUCHMANAGER_HPP
#define TOUCHMANAGER_HPP

#include "button.hpp"
#include "custom_sprite.h"
#include "label.hpp"

class Command;
class Button;

class TimedCommand {
    public:
    TimedCommand(unsigned long when, Command& cmd):when(when), cmd(cmd) {};
    unsigned long when;
    Command& cmd;
};

class TouchManager {
    private:
    std::vector<Button*> buttons;
    std::vector<Label*> labels;
    std::vector<Image*> images;
    std::map<uint, Command*> registeredCommands;
    std::map<uint, TimedCommand*> scheduledCommands;
    TFT_eSPI& tft;
    bool useSprites=false;
    CustomSprite* sprite=NULL;
    uint32_t backgroundColor;
    uint16_t width = TFT_WIDTH;
    uint16_t height = TFT_HEIGHT;
    uint8_t orientation = 0;
    Display* display=NULL;
    uint16_t touch_x1, touch_x2, touch_y1, touch_y2;
    bool touchCalibrated = false;

    void createSprite();

    public:
    TouchManager(TFT_eSPI& tft, uint32_t backgroundColor=TFT_BLACK, bool useSprites=false);
    Image& createImage(uint8_t* data, uint16_t w, uint16_t h);
    Button& createButton(Image& img, uint16_t x, uint16_t y, Command &cmd, bool hidden=false);
    Button& createButton(uint8_t* data, uint16_t x, uint16_t y, uint16_t w, uint16_t h, Command& cmd, bool hidden=false);
    Label& createLabel(String& string, uint16_t x=0, uint16_t y=0, bool hidden=false);
    Label& createLabel(const char* string, uint16_t x=0, uint16_t y=0, bool hidden=false);
    void registerCommand(uint tag, Command& cmd);
    Command& getRegisteredCommand(uint tag);
    void scheduleCommand(uint tag, unsigned long in, Command& cmd);
    void cancelScheduledCommand(uint tag);
    void redraw();
    void flip();
    void loop();
    void rotate(uint8_t orientation);
    void init();
    bool getTouch(uint16_t& x, uint16_t& y, uint16_t sensivity);
    void calibrateTouch(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);

    ~TouchManager();
};

#endif