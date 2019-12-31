#include <stdlib.h>
#include <ArduinoLog.h>
#include "touchmanager/touch_manager.hpp"
#include "touchmanager/display.h"

TouchManager::TouchManager(TFT_eSPI& tft, uint32_t backgroundColor, bool useSprites):
    tft(tft), useSprites(useSprites) {
    touch_x1 = touch_y1 = 0;
    touch_x2 = width;
    touch_y2 = height;
}

void TouchManager::createSprite() {
    if (display) {
        delete display;
        display = NULL;
    }

    if (sprite) {
        delete sprite;
        sprite = NULL;
    }

    this->sprite = new CustomSprite(&tft);
    sprite->setColorDepth(8);
    sprite->createSprite(width, height);
    sprite->fillRect(0, 0, width, height, backgroundColor);
    this->sprite->pushSprite(0,0);
    this->display=new SpriteDisplay(*sprite);
}

void TouchManager::init() {
    tft.init();
    tft.fillRect(0, 0, width, height, backgroundColor);
    if (useSprites) {
        createSprite();
    }
    else {
        this->display=new TftDisplay(tft);
    }
}

void TouchManager::rotate(uint8_t orientation) {
    tft.setRotation(orientation);
    if ( (this->orientation == 0 || this->orientation == 2) && 
        (orientation == 1 || orientation == 3)) {
        std::swap(width, height);
    }
    else if ( (this->orientation == 1 || this->orientation == 3) && 
        (orientation == 2 || orientation == 0)) {
        std::swap(width, height);
    }
    this->orientation = orientation;

    if (useSprites) {
        // need to recreate sprite to the correct dimensions
        createSprite();
    }
}

void TouchManager::redraw() {
    this->display->fillRect(0, 0, width, height, backgroundColor);
    for (auto btn : this->buttons) {
        btn->display();
    }

    for (auto lbl : this->labels) {
        lbl->display(*display);
    }

}

void TouchManager::flip() {
    if (useSprites) {
        this->sprite->pushSprite(0,0);
    }
}

Image& TouchManager::createImage(uint8_t* data, uint16_t w, uint16_t h) {
    Image* img = new Image(data, w ,h);
    this->images.push_back(img);
    return *img;
}

Button& TouchManager::createButton(Image& img, uint16_t x, uint16_t y, Command &cmd, bool hidden) {
    Button* btn = new Button(img, x, y, *display, cmd, hidden);
    this->buttons.push_back(btn);
    return *btn;
}

Button& TouchManager::createButton(uint8_t* data, uint16_t x, uint16_t y, uint16_t w, uint16_t h,Command& cmd, bool hidden) {
    Image& img = this->createImage(data, w ,h);
    return createButton(img, x, y, cmd, hidden);
}

Label& TouchManager::createLabel(String& string, uint16_t x, uint16_t y, bool hidden) {
    Label* lbl = new Label(string, x, y, hidden);
    this->labels.push_back(lbl);
    return *lbl;
}

Label& TouchManager::createLabel(const char* string, uint16_t x, uint16_t y, bool hidden) {
    String lbl = string;
    return createLabel(lbl, x, y, hidden);
}

void TouchManager::loop() {
    uint16_t x;
    uint16_t y;
    boolean pressed = getTouch(x, y, 20);

    if (pressed) {
        for (auto b: this->buttons) {
            b->maybeTrigger(x, y, *this);
        }
    }

    auto it = scheduledCommands.begin();
    unsigned long now = millis();

    while (it != scheduledCommands.end()) {
        TimedCommand* tcmd = it->second;

        if (tcmd->when < now) {
            tcmd->cmd.execute(*this);
            Log.notice("Cancelling task %p" CR, tcmd);
            Serial.flush();
            it = scheduledCommands.erase(it);
            delete tcmd;
        }
        else {
            it++;
        }
    }
}

void TouchManager::registerCommand(uint tag, Command& cmd) {
    registeredCommands[tag] = &cmd;
}

Command& TouchManager::getRegisteredCommand(uint tag) {
    Log.verbose("Asking for command %i" CR, tag);

    Command* cmd = registeredCommands[tag];

    return *cmd;
}

void TouchManager::scheduleCommand(uint tag, unsigned long in, Command& cmd) {
    TimedCommand* tcmd = new TimedCommand(millis()+in, cmd);
    Log.verbose("Scheduling task %p" CR, tcmd);
    scheduledCommands[tag] = tcmd;
}

void TouchManager::cancelScheduledCommand(uint tag) {
    if (scheduledCommands.count(tag)) {
        TimedCommand* tcmd = scheduledCommands[tag];
        Log.verbose("Cancelling task %p" CR, tcmd);
        scheduledCommands.erase(tag);
        delete tcmd;
        Log.verbose("Done cancelling task %p" CR, tcmd);
    }
}

TouchManager::~TouchManager() {
    auto delImg = [](Image* img) { delete img; };
    auto delButtons = [](Button* btn) { delete btn; };
    auto delLabels = [](Label* lbl) { delete lbl; };
    
    for_each(this->images.begin(), this->images.end(), delImg);
    for_each(this->buttons.begin(), this->buttons.end(), delButtons);
    for_each(this->labels.begin(), this->labels.end(), delLabels);

    if (sprite != NULL) {
        sprite->deleteSprite();
        delete sprite;
        sprite=NULL;
    }

    delete display;
    display=NULL;
}

bool TouchManager::getTouch(uint16_t& x, uint16_t& y, uint16_t sensivity) {
    bool pressed = tft.getTouch(&x, &y, sensivity);

    if (pressed) {
        Log.notice("Uncalibrated Touch on %i,%i" CR, x, y);
        //tft.getTouchRaw(&x, &y);
        //x = (x - touch_x1) * (touch_x2 - touch_x1);
        //y = (y - touch_y1) * (touch_y2 - touch_y1);

        Log.notice("Touch on %i,%i, width %i, height %i" CR, x, y, width, height);
        // not tested for orientation 2 & 3
        /*if (orientation == 1 || orientation == 3) {
            std::swap(x,y);
        }*/

        if ( orientation == 1 || orientation == 3) {
            x = width - x;
        }

        if (orientation == 1 || orientation == 2) {
            y = height - y;
        }

        Log.notice("Touch after rotation on %i,%i" CR, x, y);
    }

    return pressed;
}

void TouchManager::calibrateTouch(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2) {
    touch_x1 = x1;
    touch_x2 = x2;
    touch_y1 = y1;
    touch_y2 = y2;

    touchCalibrated = true;
}