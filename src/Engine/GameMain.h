#ifndef GAMEMAIN_H
#define GAMEMAIN_H

#include "WinPlatform.h"

void InitializeGame(EngineMemory* main_memory);
void GameMain(EngineMemory* main_memory, uint32 milliseconds_passed);

#endif