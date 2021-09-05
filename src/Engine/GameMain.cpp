#include "Engine.h"
#include "struct/Environment.h"

namespace MGE { namespace Engine {

Environment* MyEnv;
int animation_offset;
Sprite smile_sprite;
Sprite ascii_sprite;
TextSprite my_text;

void InitializeGame(Memory allocation, Memory vol, Bitmap** bitmap_one, Bitmap** bitmap_two)
{
    uint64 required_memory = sizeof(Environment);
    if (allocation.Size < required_memory)
    {
        Error("[Error] Not enogh memory given to InitializeGame.\n");
        return;
    }
    
    MyEnv = (Environment*)allocation.Addr;

    animation_offset = 0;
    
    byte* image_one = (byte*)malloc(10 * MEGABYTES);
    byte* image_two = (byte*)malloc(10 * MEGABYTES);
    byte* image_three = (byte*)malloc(10 * MEGABYTES);
    Memory one = {image_one, 10 * MEGABYTES};
    Memory two = {image_two, 10 * MEGABYTES};
    Memory three = {image_three, 10 * MEGABYTES};
    LoadAsset(vol, MyEnv, 0, one);
    LoadAsset(vol, MyEnv, 1, two);
    LoadAsset(vol, MyEnv, 2, three);
    
    /*
    smile_sprite = Sprite {};
    smile_sprite.Position = {50.0f, 50.0f};
    smile_sprite.Scale = {1.4f, 1.4f};
    //smile_sprite.Bitmap = GetAsset(1);
    smile_sprite.Width = smile_sprite.Bitmap->Width;
    smile_sprite.Height = smile_sprite.Bitmap->Height;
    
    ascii_sprite = Sprite {};
    ascii_sprite.Position = Vector::float2 {50, 50};
    ascii_sprite.Scale = 0.5f;
    //ascii_sprite.Bitmap = GetAsset(0);
    ascii_sprite.Width = ascii_sprite.Bitmap->Width;
    ascii_sprite.Height = ascii_sprite.Bitmap->Height;
    */
    my_text = {};
    my_text.Position = Vector::float2 {200, 50};
    my_text.Scale = 1;
    my_text.Color = 0xFF44CCCC;
    
    char buffer[32];
    char* pointer = String::ToString(buffer, -500200100);
    Util::MoveString(my_text.Glyphs, pointer);
    
    *bitmap_one = GetAsset(MyEnv, 1);
    *bitmap_two = GetAsset(MyEnv, 2);
}

void ClearBuffer(ScreenBuffer* ScreenBuffer)
{
    for (int y = 0; y < ScreenBuffer->Height; y++)
    {
        for (int x = 0; x < ScreenBuffer->Width; x++)
        {
            int index = (y * ScreenBuffer->Width) + x;
            unsigned int* pixel_address = (unsigned int*)ScreenBuffer->Pixels + index;
            
            int color = 0x00222222;
            
            *pixel_address = color;
        }
    }
}

void GameMain(ScreenBuffer* ScreenBuffer, ControlInput* ControlInput, DebugInfo* DebugInfo)
{
    animation_offset++;
    
    smile_sprite.Position.x += 2;
    smile_sprite.Position.y += 1;
    
    ClearBuffer(ScreenBuffer);
    
    //Engine::Paste((Bitmap*)ScreenBuffer, &smile_sprite);
    //Engine::Paste((Bitmap*)ScreenBuffer, &ascii_sprite);
    //Engine::PasteText((Bitmap*)ScreenBuffer, &my_text);
}

}}