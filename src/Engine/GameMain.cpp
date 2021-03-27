#include "Engine.h"

int animation_offset;
Sprite my_sprite;
Bitmap sprite_bitmap;

void UpdateBuffer_test(ScreenBuffer* ScreenBuffer, byte* asset)
{
    for (int y = 0; y < 100; y++)
    {
        for (int x = 0; x < 100; x++)
        {
            int* from = (int*)asset + x + (100 * y);
            int* to = (int*)ScreenBuffer->Pixels + x + (ScreenBuffer->Width * y);
            
            *to = *from;
        }
    }
}

void UpdateBuffer_test2(ScreenBuffer* ScreenBuffer, Sprite my_sprite)
{
    Bitmap alias = {};
    alias.Width = ScreenBuffer->Width;
    alias.Height = ScreenBuffer->Height;
    alias.Pixels = &ScreenBuffer->Pixels[0];
    Engine::Paste(&alias, &my_sprite);
}

void Engine::InitializeGame(RootMemory* RootMemory)
{
    animation_offset = 0;

    sprite_bitmap = {};
    sprite_bitmap.Width = 100;
    sprite_bitmap.Height = 100;
    sprite_bitmap.Pixels = (int*)malloc(10 * MEGABYTES);
    
    byte* mem = (byte*)malloc(10 * MEGABYTES);
    int success = Engine::OpenBitmap(mem, 10 * MEGABYTES, sprite_bitmap.Pixels, 10 * MEGABYTES, "my_image.bmp");
    free(mem);
    
    if (success == 0)
    {
        return;
    }
    
    my_sprite = Sprite {};
    
    my_sprite.Position = Couple_f {50, 50};
    my_sprite.Width = 100;
    my_sprite.Height = 100;
    my_sprite.Bitmap = &sprite_bitmap;
    
    //UpdateBuffer_test2(&RootMemory->ScreenBuffer, my_sprite);
}

void UpdateBuffer(ScreenBuffer* ScreenBuffer, int MouseX, int MouseY)
{
    for (int y = 0; y < ScreenBuffer->Height; y++)
    {
        for (int x = 0; x < ScreenBuffer->Width; x++)
        {
            int index = (y * ScreenBuffer->Width) + x;
            unsigned int* pixel_address = (unsigned int*)ScreenBuffer->Pixels + index;
            
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

void ClearBuffer(ScreenBuffer* ScreenBuffer)
{
    for (int y = 0; y < ScreenBuffer->Height; y++)
    {
        for (int x = 0; x < ScreenBuffer->Width; x++)
        {
            int index = (y * ScreenBuffer->Width) + x;
            unsigned int* pixel_address = (unsigned int*)ScreenBuffer->Pixels + index;
            
            int color = 0x00000000;
            
            *pixel_address = color;
        }
    }
}

void Engine::GameMain(ScreenBuffer* ScreenBuffer, ControlInput* ControlInput, uint32 milliseconds_passed)
{
    animation_offset++;
    //UpdateBuffer(ScreenBuffer, ControlInput->MouseX, ControlInput->MouseY);
    
    /*
    algo:
    1) clear buffer
    2) for sprite in sprites
        3) paste sprite
    */
    
    my_sprite.Position.X += 2;
    my_sprite.Position.Y += 1;
    
    ClearBuffer(ScreenBuffer);
    UpdateBuffer_test2(ScreenBuffer, my_sprite);
}