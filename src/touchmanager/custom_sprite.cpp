#include "touchmanager/custom_sprite.h"

CustomSprite::CustomSprite(TFT_eSPI* tft): TFT_eSprite(tft) {};

void CustomSprite::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data) {
    if ((x >= _iwidth) || (y >= _iheight) || (w == 0) || (h == 0) || !_created) return;
    if ((x + w < 0) || (y + h < 0)) return;

    int32_t  xo = 0;
    int32_t  yo = 0;

    int32_t  xs = x;
    int32_t  ys = y;

    int32_t ws = w;
    int32_t hs = h;

    if (x < 0) { xo = -x; ws += x; xs = 0; }
    if (y < 0) { yo = -y; hs += y; ys = 0; }

    if (xs + ws >= (int32_t)_iwidth)  ws = _iwidth  - xs;
    if (ys + hs >= (int32_t)_iheight) hs = _iheight - ys;

    for (int32_t yp = yo; yp < yo + hs; yp++)
    {
      x = xs;
      for (int32_t xp = xo; xp < xo + ws; xp++)
      {
        uint8_t color = data[xp + yp * w];
        _img8[x + ys * _iwidth] = color;
        x++;
      }
      ys++;
    }

}