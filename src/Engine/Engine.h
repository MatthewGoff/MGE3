#pragma once

#include "Engine/struct/Bitmap.h"
#include "Engine/struct/Glyph.h"
#include "Engine/struct/TextSprite.h"

namespace MGE { namespace Engine {

// GameMain.cpp
void InitializeGame(RootMemory* main_memory);
void GameMain(ScreenBuffer* ScreenBuffer, ControlInput* ControlInput, DebugInfo* DebugInfo);

// OpenBitmap.cpp
bool OpenBitmap(byte* mem, int mem_size, Bitmap* destination, int dest_size, char* path);

// Rendering.cpp
//void Paste(Bitmap* destination, Sprite* sprite);
//void PasteText(Bitmap* destination, TextSprite* text);

// AssetManager.cpp
void LoadAsset(Memory vol, int id, Memory destination);
Bitmap* GetAsset(int id);

}}