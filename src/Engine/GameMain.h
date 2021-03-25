#ifndef GAMEMAIN_H
#define GAMEMAIN_H

void InitializeGame(RootMemory* main_memory);
void GameMain(ScreenBuffer* ScreenBuffer, ControlInput* ControlInput, uint32 milliseconds_passed);

#endif