/*
The purpose of this file (for the time being) is to display a circle that follows the mouse. Then we'll do bitmap
*/

#include "Main.h"
#include "Print.h"

int animation_offset;

int InitializeGame(EngineMemory* main_memory)
{
    animation_offset = 0;
    return 277;
}

void UpdateBuffer(EngineMemory* main_memory)
{
    Print("UpdatingBuffer\n");
    for (int y = 0; y < main_memory->BitmapBuffer.Height; y++)
    {
        for (int x = 0; x < main_memory->BitmapBuffer.Width; x++)
        {
            int index = (y * main_memory->BitmapBuffer.Width) + x;
            unsigned int* pixel_address = (unsigned int*) main_memory->BitmapBuffer.bytes + index;
            
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

int Main(EngineMemory* engine_memory, uint32 milliseconds_passed)
{
    animation_offset++;
    UpdateBuffer(engine_memory);
    return engine_memory->ControlInput.MouseX;
}