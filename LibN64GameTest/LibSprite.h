#pragma once
#include <libdragon.h>

class libSprite {
public:
    void Ready(int w, int h, int* nd);
    void Draw(display_context_t disp, int x, int y, bool alpha, int scale);

private:
    int Width;
    int Height;
    int* Data;
};
