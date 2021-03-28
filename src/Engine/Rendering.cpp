#include "Engine.h"

int SampleBitmap(Vector::float2 point, Bitmap* bitmap)
{
    if (point.x < 0 | point.y < 0 | point.x >= bitmap->Width | point.y >= bitmap->Height)
    {
        return 0;
    }
    
    int* address = bitmap->Pixels + (bitmap->Width * (int)point.y) + (int)point.x;
    return *address;
}

int BlendPixel(int x, int y, Sprite* sprite)
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

    int64 a = (sample1 & 0xFF000000)
            + (sample2 & 0xFF000000)
            + (sample3 & 0xFF000000)
            + (sample4 & 0xFF000000);
    a /= 4;
    a &= 0xFF000000;
    
    int64 r = (sample1 & 0x00FF0000)
            + (sample2 & 0x00FF0000)
            + (sample3 & 0x00FF0000)
            + (sample4 & 0x00FF0000);
    r /= 4;
    r &= 0x00FF0000;

    int64 g = (sample1 & 0x0000FF00)
            + (sample2 & 0x0000FF00)
            + (sample3 & 0x0000FF00)
            + (sample4 & 0x0000FF00);
    g /= 4;
    g &= 0x0000FF00;

    int64 b = (sample1 & 0x000000FF)
            + (sample2 & 0x000000FF)
            + (sample3 & 0x000000FF)
            + (sample4 & 0x000000FF);
    b /= 4;
    b &= 0x000000FF;
    
    return a | r | g | b;
}

void Engine::Paste(Bitmap* destination, Sprite* sprite)
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
            int color = BlendPixel(x, y, sprite);
            int* address = destination->Pixels + (destination->Width * y) + (x);
            *address = color;
        }
    }
}

void PasteAlpha()
{
    
}