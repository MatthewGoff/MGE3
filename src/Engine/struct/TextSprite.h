#pragma once

namespace MGE {

struct TextSprite
{
    Standard::Vector::float2 Position;
    float Scale;
    int Color;
    char Glyphs[128];
};

}