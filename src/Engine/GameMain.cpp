#include "OpenBitmap.h"

int animation_offset;

void UpdateBuffer_test(ScreenBuffer* ScreenBuffer, byte* WorkingMemory)
{
    for (int y = 0; y < 100; y++)
    {
        for (int x = 0; x < 100; x++)
        {
            int* from = (int*)WorkingMemory + x + (100 * y);
            int* to = (int*)ScreenBuffer->bytes + x + (ScreenBuffer->Width * y);
            
            *to = *from;
        }
    }
}

void InitializeGame(RootMemory* RootMemory)
{
    animation_offset = 0;
    byte* asset = (byte*)malloc(10 * MEGABYTES);
    
    byte* mem = (byte*)malloc(10 * MEGABYTES);
    int success = OpenBitmap(mem, 10 * MEGABYTES, asset, 10 * MEGABYTES, "my_image.bmp");
    free(mem);
    
    if (success > 0)
    {
        UpdateBuffer_test(&RootMemory->ScreenBuffer, asset);
    }
    
    free(asset);
}

void UpdateBuffer(ScreenBuffer* ScreenBuffer, int MouseX, int MouseY)
{
    for (int y = 0; y < ScreenBuffer->Height; y++)
    {
        for (int x = 0; x < ScreenBuffer->Width; x++)
        {
            int index = (y * ScreenBuffer->Width) + x;
            unsigned int* pixel_address = (unsigned int*)ScreenBuffer->bytes + index;
            
            int r = 0x00;
            int g = 0x00;
            int b = 0x00;
            int a = 0xFF; //alpha channel is only padding atm
            
            int y_offset = y + animation_offset;
            int x_offset = x + (2 * animation_offset);
            if (y_offset & (1 << 5))
            {
                r = 0xFF;
            }
            
            if (x_offset & (1 << 6))
            {
                b = 0xFF;
            }
            
            if ((x_offset + y_offset) & (1 << 7))
            {
                g = 0xFF;
            }
            
            // Windows specific ordering explained on handmade hero.
            int color = (a << (8 * 3)) + (r << (8 * 2)) + (g << (8 * 1)) + b;

            int dx = x - MouseX;
            int dy = y - MouseY;
            int asqrd = dx * dx;
            int bsqrd = dy * dy;
            int csqrd = asqrd + bsqrd;
            float distance = sqrt(csqrd);
            if (distance < 30)
            {
                color = 0x00000000;
            }
            else if (distance < 40)
            {
                color = 0xFFFFFFFF;
            }
            
            *pixel_address = color;
        }
    }
}

void GameMain(ScreenBuffer* ScreenBuffer, ControlInput* ControlInput, uint32 milliseconds_passed)
{
    animation_offset++;
    //UpdateBuffer(ScreenBuffer, ControlInput->MouseX, ControlInput->MouseY);
}