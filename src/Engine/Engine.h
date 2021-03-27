#ifndef ENGINE_H
#define ENGINE_H

#include "Engine/struct/Bitmap.h"
#include "Engine/struct/Sprite.h"

namespace Engine
{
    // GameMain.cpp
    void InitializeGame(RootMemory* main_memory);
    void GameMain(ScreenBuffer* ScreenBuffer, ControlInput* ControlInput, uint32 milliseconds_passed);
    
    // OpenBitmap.cpp
    int OpenBitmap(byte* mem, int mem_size, int* destination, int dest_size, char* path);
    
    // Rendering.cpp
    void Paste(Bitmap* destination, Sprite* sprite);
}

#endif