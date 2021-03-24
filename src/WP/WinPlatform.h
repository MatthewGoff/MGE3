#ifndef WINPLATFORM_H
#define WINPLATFORM_H

//#include "Print.h"

//platform API goes here

//Print()
//LoadFile()

struct EngineMemory
{
    struct
    {
        //byte bytes[960*540*4];
        byte bytes[1920 * 1080 * 4];
        int Width;
        int Height;
    } ScreenBuffer;
    struct
    {
        byte bytes[10 * MEGABYTES];
        int Size = 10 * MEGABYTES;
    } FileBuffer;
    struct
    {
        int MouseX;
        int MouseY;
    } ControlInput;
    byte* Heap;
};

#endif