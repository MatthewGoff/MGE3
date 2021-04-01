#ifndef TEXTSPRITE_H
#define TEXTSPRITE_H

struct TextSprite
{
    Vector::float2 Position;
    float Scale;
    int Color;
    char Glyphs[128];
};

#endif