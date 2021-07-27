#pragma once

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