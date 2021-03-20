#include "Clock.h"

void
Clock::Initialize()
{
    LARGE_INTEGER frequency_result;
    QueryPerformanceFrequency(&frequency_result);
    Frequency = pow(10, 6) * (uint)frequency_result.QuadPart; // convert to 10^6 Hz
}

uint
Clock::GetTime()
{
    LARGE_INTEGER counter_result;
    QueryPerformanceCounter(&counter_result);
    return (uint)counter_result.QuadPart / Frequency;
}