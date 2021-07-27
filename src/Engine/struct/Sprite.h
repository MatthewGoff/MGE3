#ifndef SPRITE_H
#define SPRITE_H

#include "Bitmap.h"

namespace MGE {

struct Sprite
{
    Vector::float2 Position;
    int Width;
    int Height;
    float Scale;
    Bitmap* Bitmap;
};

}

#endif