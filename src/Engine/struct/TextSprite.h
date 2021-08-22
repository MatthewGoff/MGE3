#pragma once

namespace MGE {

struct TextSprite
{
    Vector::float2 Position;
    float Scale;
    int Color;
    char Glyphs[128];
};

}