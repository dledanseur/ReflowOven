#include "display.h"

#ifndef LABEL_HPP
#define LABEL_HPP

class Label {
  private:
    String m_text;
    uint16_t m_x=0, m_y=0, m_textSize=1, m_textColor=TFT_WHITE;
    bool hidden=false;

  public:
    Label(const String& txt, uint16_t x=0, uint16_t y=0, bool hidden=false);
    void setText(const String& txt);
    void setX(uint16_t x);
    void setY(uint16_t y);
    void setTextSize(uint16_t textSize);
    void setTextColor(uint16_t textColor);
    void display(Display& display);
};
#endif