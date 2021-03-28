#ifndef SPRITE_H
#define SPRITE_H

#include "Bitmap.h"

struct Sprite
{
    Vector::float2 Position;
    int Width;
    int Height;
    float Scale;
    Bitmap* Bitmap;
};

#endif