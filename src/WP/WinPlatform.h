#ifndef WINPLATFORM_H
#define WINPLATFORM_H

#include "Print.h"

//platform API goes here

//Print()
//LoadFile()

struct ControlInput
{
    int MouseX;
    int MouseY;
};

/*
struct Bitmap
{
    //byte bytes[960*540*4];
    byte bytes[1920 * 1080 * 4];
    int Width;
    int Height;
};
*/
struct FileBuffer
{
    byte bytes[10 * MEGABYTES];
};

struct EngineMemory
{
    ControlInput ControlInput;
    struct asdf {
        //byte bytes[960*540*4];
        byte bytes[1920 * 1080 * 4];
        int Width;
        int Height;
    } BitmapBuffer;
    FileBuffer FileBuffer;
    byte* Heap;
    /*
    struct {
    
    } ControlInput; //???
    */
};

#endif