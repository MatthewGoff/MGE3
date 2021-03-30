#ifndef STANDARD_H
#define STANDARD_H

namespace Vector
{
    struct float2
    {
        float x;
        float y;
    };

    // Vector.cpp
    float2 Add(float2 oprnd1, float2 oprnd2);
    float2 Add(float2 vector, float dx, float dy);
    float2 Sub(float2 oprnd1, float2 oprnd2);
    float2 Mul(float2 vector, float scalar);
    float2 Div(float2 vector, float scalar);
}

namespace Util
{
    // Util.cpp
    int ReverseEnd(int word);
    int Min(int a, int b);
    int Max(int a, int b);
    void MoveString(char* to, char* from);
}

#endif