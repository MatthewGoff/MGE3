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
    int local_x = x - sprite->Position.X;
    int local_y = y - sprite->Position.Y;
    
    return SampleBitmap((float)local_x, (float)local_y, sprite->Bitmap);
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