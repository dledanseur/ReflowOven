#include "touchmanager/label.hpp"

Label::Label(const String& txt, uint16_t x, uint16_t y, bool hidden): m_text(txt), 
                                            m_x(x), m_y(y), hidden(hidden) {}

void Label::setText(const String& txt) {
    m_text = txt;
}

void Label::setX(uint16_t x) {
    m_x = x;
}
void Label::setY(uint16_t y) {
    m_y = y;
}

void Label::setTextSize(uint16_t textSize) {
    m_textSize = textSize;
}

void Label::setTextColor(uint16_t textColor) {
    m_textColor = textColor;
}

void Label::display(Display& display) {
    if (!hidden) {
        display.drawString(m_text, m_x, m_y, m_textSize, m_textColor);
    }
}