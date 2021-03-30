#ifndef ENGINE_H
#define ENGINE_H

#include "Engine/struct/Bitmap.h"
#include "Engine/struct/Sprite.h"
#include "Engine/struct/Glyph.h"
#include "Engine/struct/TextSprite.h"

namespace Engine
{
    // GameMain.cpp
    void InitializeGame(RootMemory* main_memory);
    void GameMain(ScreenBuffer* ScreenBuffer, ControlInput* ControlInput, uint32 milliseconds_passed);
    
    // OpenBitmap.cpp
    bool OpenBitmap(byte* mem, int mem_size, Bitmap* destination, int dest_size, char* path);
    
    // Rendering.cpp
    void Paste(Bitmap* destination, Sprite* sprite);
    void PasteGlyph(
        Bitmap* destination,
        Vector::float2 position,
        float scale,
        char glyph);
    void PasteText(Bitmap* destination, TextSprite* text);
    
    // AssetManager.cpp
    void LoadAsset(int id);
    Bitmap* GetAsset(int id);
}

#endif