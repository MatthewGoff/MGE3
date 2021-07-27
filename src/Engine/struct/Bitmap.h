#ifndef BITMAP_H
#define BITMAP_H

namespace MGE {

struct Bitmap
{
    int Width;
    int Height;
    int Pixels[];
};

}

#endif