#include "Engine.h"

int SampleBitmap(float x, float y, Bitmap* bitmap)
{
    if (x < 0 | y < 0 | x >= bitmap->Width | y >= bitmap->Height)
    {
        return 0;
    }
    
    int* address = bitmap->Pixels + (bitmap->Width * (int)y) + (int)x;
    return *address;
}

int BlendPixel(int x, int y, Sprite* sprite)
{
    float local_x = x - sprite->Position.X;
    float local_y = y - sprite->Position.Y;
    
    int sample1 = SampleBitmap(
        local_x + 0.25f,
        local_y + 0.25f,
        sprite->Bitmap);
    int sample2 = SampleBitmap(
        local_x + 0.25f,
        local_y + 0.75f,
        sprite->Bitmap);
    int sample3 = SampleBitmap(
        local_x + 0.75f,
        local_y + 0.25f,
        sprite->Bitmap);
    int sample4 = SampleBitmap(
        local_x + 0.75f,
        local_y + 0.75f,
        sprite->Bitmap);

    int64 test = 0x00FF0000;
    test += 0x00FF0000;
    test += 0x00FF0000;
    test += 0x00FF0000;
    int64 test2 = test / 4;
    int64 test3 = test >> 2;

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

    int low_y = (int)sprite->Position.Y;
    int low_x = (int)sprite->Position.X;
    
    low_y = Max(low_y, 0);
    low_x = Max(low_x, 0);
    
    int high_y = (int)sprite->Position.Y + sprite->Height;
    int high_x = (int)sprite->Position.X + sprite->Width;
    
    high_y = Min(high_y, destination->Height);
    high_x = Min(high_x, destination->Width);
    
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