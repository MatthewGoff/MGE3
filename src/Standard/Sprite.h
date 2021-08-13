#pragma once

namespace MGE { namespace Standard {

struct Sprite
{
    Vector::float2 Position;
    Vector::float2 Scale;
    Vector::float2 Pivot;
    float Rotation;
    
    Vector::float2 TopLeft()
    {
        Vector::float2 result = {0.0f, 0.0f};
        result = Vector::Sub(result, Pivot);
        result = Vector::Mul(result, Scale);
        result = Vector::Rotate(result, Rotation);
        result = Vector::Add(result, Position);
        return result;
    }
    
    Vector::float2 TopRight()
    {
        Vector::float2 result = {1.0f, 0.0f};
        result = Vector::Sub(result, Pivot);
        result = Vector::Mul(result, Scale);
        result = Vector::Rotate(result, Rotation);
        result = Vector::Add(result, Position);
        return result;
    }
    
    Vector::float2 BottomLeft()
    {
        Vector::float2 result = {0.0f, 1.0f};
        result = Vector::Sub(result, Pivot);
        result = Vector::Mul(result, Scale);
        result = Vector::Rotate(result, Rotation);
        result = Vector::Add(result, Position);
        return result;
    }
    
    Vector::float2 BottomRight()
    {
        Vector::float2 result = {1.0f, 1.0f};
        result = Vector::Sub(result, Pivot);
        result = Vector::Mul(result, Scale);
        result = Vector::Rotate(result, Rotation);
        result = Vector::Add(result, Position);
        return result;
    }
};

}}