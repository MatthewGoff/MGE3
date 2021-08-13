#pragma once

namespace MGE { namespace Standard { namespace Vector {

struct float2
{
    float x;
    float y;
};

struct float3
{
    float x;
    float y;
    float z;
};

struct int2
{
    int x;
    int y;
};

float2 Add(float2 oprnd1, float2 oprnd2);
float2 Add(float2 vector, float dx, float dy);
float2 Sub(float2 oprnd1, float2 oprnd2);
float2 Mul(float2 vector, float scalar);
float2 Div(float2 vector, float scalar);

}}}