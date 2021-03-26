#ifndef MEMORY_H
#define MEMORY_H

struct ScreenBuffer
{
    int Width;
    int Height;
    byte bytes[WINDOW_WIDTH * WINDOW_HEIGHT * 4]; // 4 bytes per pixel
};

struct ControlInput
{
    int MouseX;
    int MouseY;
};

struct FileBuffer
{
    byte bytes[10 * MEGABYTES];
    int Size = 10 * MEGABYTES;
};
/*
#define ASSET_VACANT 0
#define ASSET_REQUESTED 1
#define ASSET_READY 2

struct AssetExchange
{
    int status;
    byte bytes[10 * MEGABYTES];
};
*/
struct RootMemory
{
    ScreenBuffer ScreenBuffer;
    ControlInput ControlInput;
    FileBuffer FileBuffer;
    //AssetExchange AssetExchange;
    byte WorkingMemory;
};

#endif