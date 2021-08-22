#pragma once

namespace MGE {

struct ScreenBuffer
{
    int Width;
    int Height;
    int Pixels[WINDOW_WIDTH * WINDOW_HEIGHT];// * 4]; // 4 bytes per pixel
};

struct ControlInput
{
    Vector::int2 CursorPosition;
};

struct FileBuffer
{
    byte bytes[10 * MEGABYTES];
    int Size = 10 * MEGABYTES;
};

struct DebugInfo
{
    int FrameRate;
    float WorkLoad;
    float Time;
    Vector::int2 CursorPosition;
};

struct RootMemory
{
    ScreenBuffer ScreenBuffer;
    ControlInput ControlInput;
    DebugInfo DebugInfo;
    FileBuffer FileBuffer;
};

}