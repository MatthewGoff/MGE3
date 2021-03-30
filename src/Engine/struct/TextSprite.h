#ifndef TEXTSPRITE_H
#define TEXTSPRITE_H

struct TextSprite
{
    Vector::float2 Position;
    float Scale;
    char Glyphs[128];
};

#endif