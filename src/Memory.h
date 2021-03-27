#ifndef MEMORY_H
#define MEMORY_H

struct ScreenBuffer
{
    int Width;
    int Height;
    int Pixels[WINDOW_WIDTH * WINDOW_HEIGHT];// * 4]; // 4 bytes per pixel
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

struct RootMemory
{
    ScreenBuffer ScreenBuffer;
    ControlInput ControlInput;
    FileBuffer FileBuffer;
};

#endif