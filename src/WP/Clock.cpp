#include <Windows.h>
#include "Clock.h"

double Frequency; // ticks per micro second (usually 1.0)

void
Clock::Initialize()
{
    LARGE_INTEGER frequency_result;
    QueryPerformanceFrequency(&frequency_result);
    Frequency = (double)frequency_result.QuadPart / (double)pow(10, 6);
}

int64
Clock::GetTimeMicro()
{
    LARGE_INTEGER counter_result;
    QueryPerformanceCounter(&counter_result);
    
    return counter_result.QuadPart / Frequency;
}

int32
Clock::GetTimeMilli()
{
    return (int32)(GetTimeMicro() / 1000);
}