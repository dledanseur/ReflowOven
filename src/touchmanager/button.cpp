#include "touchmanager/button.hpp"

Button::Button(Image& img, uint16_t x, uint16_t y, Display& tft, Command& command, bool hidden):
        img(img), x(x), y(y), tft(tft), command(command), hidden(hidden) {}

boolean Button::isTouchInside(uint16_t x, uint16_t y) {
    uint16_t x1=this->x, y1=this->y, x2=x1+this->img.getW(), y2=y1+this->img.getH();

    Log.verbose("Testing touch %i,%i against x,y,x2,y2 %i,%i%,%i,%i" CR, x,y,x1,y1,x2,y2);
    boolean inside = (x>=x1 && x<=x2 &&y>=y1 && y<=y2);
    return inside;

}

void Button::maybeTrigger(uint16_t x, uint16_t y, TouchManager& manager) {
    if (!hidden && isTouchInside(x,y)) {
        this->command.execute(manager);
    }
}

void Button::display() {
    if (!hidden) {
        this->img.display(this->tft, this->x, this->y);
    }
}

void Button::setHidden(bool hidden) {
    this->hidden = hidden;
}
