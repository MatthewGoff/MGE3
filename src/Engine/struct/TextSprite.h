#ifndef TEXTSPRITE_H
#define TEXTSPRITE_H

namespace MGE {

struct TextSprite
{
    Vector::float2 Position;
    float Scale;
    int Color;
    char Glyphs[128];
};

}

#endif