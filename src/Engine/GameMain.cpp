/*
The purpose of this file (for the time being) is to display a circle that follows the mouse. Then we'll do bitmap
*/

#include "GameMain.h"
#include "Print.h"

int animation_offset;

void InitializeGame(EngineMemory* main_memory)
{
    animation_offset = 0;
}

void UpdateBuffer(EngineMemory* main_memory)
{
    for (int y = 0; y < main_memory->ScreenBuffer.Height; y++)
    {
        for (int x = 0; x < main_memory->ScreenBuffer.Width; x++)
        {
            int index = (y * main_memory->ScreenBuffer.Width) + x;
            unsigned int* pixel_address = (unsigned int*) main_memory->ScreenBuffer.bytes + index;
            
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

            *pixel_address = color;
        }
    }
}

void GameMain(EngineMemory* engine_memory, uint32 milliseconds_passed)
{
    animation_offset++;
    UpdateBuffer(engine_memory);
}