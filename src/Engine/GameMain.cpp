#include "Engine.h"

int animation_offset;
Sprite smile_sprite;
Sprite ascii_sprite;
Glyph symbol;

void Engine::InitializeGame(RootMemory* RootMemory)
{
    animation_offset = 0;
    
    LoadAsset(0);
    LoadAsset(1);
    
    smile_sprite = Sprite {};
    smile_sprite.Position = Vector::float2 {50, 50};
    smile_sprite.Scale = 1.4f;
    smile_sprite.Bitmap = GetAsset(1);
    smile_sprite.Width = smile_sprite.Bitmap->Width;
    smile_sprite.Height = smile_sprite.Bitmap->Height;
    
    ascii_sprite = Sprite {};
    ascii_sprite.Position = Vector::float2 {50, 50};
    ascii_sprite.Scale = 0.5f;
    ascii_sprite.Bitmap = GetAsset(0);
    ascii_sprite.Width = ascii_sprite.Bitmap->Width;
    ascii_sprite.Height = ascii_sprite.Bitmap->Height;
    
    symbol = {};
    symbol.Position = Vector::float2 {200, 50};
    symbol.Scale = 1;
    symbol.ascii = '&';
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
    
    smile_sprite.Position.x += 2;
    smile_sprite.Position.y += 1;
    
    ClearBuffer(ScreenBuffer);
    
    Engine::Paste((Bitmap*)ScreenBuffer, &smile_sprite);
    Engine::Paste((Bitmap*)ScreenBuffer, &ascii_sprite);
    Engine::PasteGlyph((Bitmap*)ScreenBuffer, &symbol);
}