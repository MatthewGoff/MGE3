#ifndef SPRITE_H
#define SPRITE_H

#include "Bitmap.h"

struct Sprite
{
    Couple_f Position;
    int Width;
    int Height;
    Bitmap* Bitmap;
};

#endif