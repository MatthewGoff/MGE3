
namespace MGE { namespace Standard { namespace Vector {

float2 Add(float2 oprnd1, float2 oprnd2)
{
    return float2 {oprnd1.x + oprnd2.x, oprnd1.y + oprnd2.y};
}

float2 Add(float2 vector, float dx, float dy)
{
    return float2 {vector.x + dx, vector.y + dy};
}

float2 Sub(float2 oprnd1, float2 oprnd2)
{
    return float2 {oprnd1.x - oprnd2.x, oprnd1.y - oprnd2.y};
}

float2 Mul(float2 vector, float scalar)
{
    return float2 {vector.x * scalar, vector.y * scalar};
}

float2 Div(float2 vector, float scalar)
{
    return float2 {vector.x / scalar, vector.y / scalar};
}

}}}