#ifndef MEMORY_H
#define MEMORY_H

struct ScreenBuffer
{
    //byte bytes[960*540*4];
    byte bytes[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
    int Width;
    int Height;
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