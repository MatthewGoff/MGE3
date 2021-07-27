#include "Engine.h"

namespace MGE { namespace Engine {

int AverageColors(int c1, int c2, int c3, int c4)
{
    uint64 a = (uint64)(c1 & 0xFF000000)
             + (uint64)(c2 & 0xFF000000)
             + (uint64)(c3 & 0xFF000000)
             + (uint64)(c4 & 0xFF000000);
    a /= 4;
    a &= 0xFF000000;
    
    int64 r = (c1 & 0x00FF0000)
            + (c2 & 0x00FF0000)
            + (c3 & 0x00FF0000)
            + (c4 & 0x00FF0000);
    r /= 4;
    r &= 0x00FF0000;

    int64 g = (c1 & 0x0000FF00)
            + (c2 & 0x0000FF00)
            + (c3 & 0x0000FF00)
            + (c4 & 0x0000FF00);
    g /= 4;
    g &= 0x0000FF00;

    int64 b = (c1 & 0x000000FF)
            + (c2 & 0x000000FF)
            + (c3 & 0x000000FF)
            + (c4 & 0x000000FF);
    b /= 4;
    b &= 0x000000FF;
    
    return a | r | g | b;
}

int AlphaComposite(int base, int addition)
{        
    float a = (float)(addition & 0xFF000000) / 0xFF000000;
    
    float _r = a * (addition & 0x00FF0000) + (1 - a) * (base & 0x00FF0000);
    int r = ((int)_r) & 0x00FF0000;
    
    float _g = a * (addition & 0x0000FF00) + (1 - a) * (base & 0x0000FF00);
    int g = ((int)_g) & 0x0000FF00;
    
    float _b = a * (addition & 0x000000FF) + (1 - a) * (base & 0x000000FF);
    int b = ((int)_b) & 0x000000FF;
    
    return 0xFF000000 | r | g | b;
}

int SampleBitmap(Vector::float2 point, Bitmap* bitmap)
{
    if (point.x < 0 | point.y < 0 | point.x >= bitmap->Width | point.y >= bitmap->Height)
    {
        return 0;
    }
    
    int* address = bitmap->Pixels + (bitmap->Width * (int)point.y) + (int)point.x;
    return *address;
}

int Multisample(int x, int y, Sprite* sprite)
{
    Vector::float2 global = Vector::float2 {(float)x, (float)y};
    Vector::float2 local = Vector::Sub(global, sprite->Position);
    
    Vector::float2 p1 = Vector::Add(local, 0.25f, 0.25f);
    Vector::float2 p2 = Vector::Add(local, 0.25f, 0.75f);
    Vector::float2 p3 = Vector::Add(local, 0.75f, 0.25f);
    Vector::float2 p4 = Vector::Add(local, 0.75f, 0.75f);
    
    p1 = Vector::Div(p1, sprite->Scale);
    p2 = Vector::Div(p2, sprite->Scale);
    p3 = Vector::Div(p3, sprite->Scale);
    p4 = Vector::Div(p4, sprite->Scale);
    
    int sample1 = SampleBitmap(p1, sprite->Bitmap);
    int sample2 = SampleBitmap(p2, sprite->Bitmap);
    int sample3 = SampleBitmap(p3, sprite->Bitmap);
    int sample4 = SampleBitmap(p4, sprite->Bitmap);

    return AverageColors(sample1, sample2, sample3, sample4);
}

void Paste(Bitmap* destination, Sprite* sprite)
{

    int low_y = (int)sprite->Position.y;
    int low_x = (int)sprite->Position.x;
    
    low_y = Util::Max(low_y, 0);
    low_x = Util::Max(low_x, 0);
    
    int high_y = (int)sprite->Position.y + sprite->Height * sprite->Scale;
    int high_x = (int)sprite->Position.x + sprite->Width * sprite->Scale;
    
    high_y = Util::Min(high_y, destination->Height);
    high_x = Util::Min(high_x, destination->Width);
    
    for (int y = low_y; y < high_y; y++)
    {
        for (int x = low_x; x < high_x; x++)
        {
            int* address = destination->Pixels + (destination->Width * y) + (x);
            
            int color = Multisample(x, y, sprite);
            color = AlphaComposite(*address, color);
            *address = color;
        }
    }
}

/*
Takes global location x & y. Also takes a sprite with a position.
*/
int MultisampleGlyph(
    int x,
    int y,
    Vector::float2 position,
    float scale,
    char glyph)
{
    Vector::float2 global = Vector::float2 {(float)x, (float)y};
    Vector::float2 local = Vector::Sub(global, position);
    
    Vector::float2 p1 = Vector::Add(local, 0.25f, 0.25f);
    Vector::float2 p2 = Vector::Add(local, 0.25f, 0.75f);
    Vector::float2 p3 = Vector::Add(local, 0.75f, 0.25f);
    Vector::float2 p4 = Vector::Add(local, 0.75f, 0.75f);
    
    p1 = Vector::Div(p1, scale);
    p2 = Vector::Div(p2, scale);
    p3 = Vector::Div(p3, scale);
    p4 = Vector::Div(p4, scale);
    
    int vertical_offset = 120 * (glyph - 32);
    
    p1 = Vector::Add(p1, 0, vertical_offset);
    p2 = Vector::Add(p2, 0, vertical_offset);
    p3 = Vector::Add(p3, 0, vertical_offset);
    p4 = Vector::Add(p4, 0, vertical_offset);
    
    Bitmap* bitmap = GetAsset(0);
    int sample1 = SampleBitmap(p1, bitmap);
    int sample2 = SampleBitmap(p2, bitmap);
    int sample3 = SampleBitmap(p3, bitmap);
    int sample4 = SampleBitmap(p4, bitmap);
    
    return AverageColors(sample1, sample2, sample3, sample4);
}

void PasteGlyph(
    Bitmap* destination,
    Vector::float2 position,
    float scale,
    int color,
    char glyph)
{
    int low_y = (int)position.y;
    int low_x = (int)position.x;
    
    low_y = Util::Max(low_y, 0);
    low_x = Util::Max(low_x, 0);
    
    int high_y = (int)position.y + 120 * scale;
    int high_x = (int)position.x + 44 * scale;
    
    high_y = Util::Min(high_y, destination->Height);
    high_x = Util::Min(high_x, destination->Width);
    
    for (int y = low_y; y < high_y; y++)
    {
        for (int x = low_x; x < high_x; x++)
        {
            int* address = destination->Pixels + (destination->Width * y) + (x);
            
            int sample = MultisampleGlyph(x, y, position, scale, glyph);
            sample = (sample & 0xFF000000) | (color & 0x00FFFFFF);
            sample = AlphaComposite(*address, sample);
            *address = sample;
        }
    }
}

void PasteText(Bitmap* destination, TextSprite* text)
{
    int ordinal_position = 0;
    
    char* runner = (char*)&text->Glyphs;
    while (*runner != 0)
    {
        Vector::float2 position = Vector::Add(text->Position, 44 * ordinal_position, 0);
        
        PasteGlyph(destination, position, text->Scale, text->Color, *runner);
        
        ordinal_position++;
        runner++;
    }
}

}}