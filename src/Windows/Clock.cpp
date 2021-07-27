#include <windows.h>
//#include "Windows.h"

double Frequency; // ticks per micro second (usually 1.0)

namespace MGE { namespace Windows { namespace Clock
{
    void Initialize()
    {
        LARGE_INTEGER frequency_result;
        QueryPerformanceFrequency(&frequency_result);
        Frequency = (double)frequency_result.QuadPart / (double)pow(10, 6);
    }
    
    int64 GetTimeMicro()
    {
        LARGE_INTEGER counter_result;
        QueryPerformanceCounter(&counter_result);
        
        return counter_result.QuadPart / Frequency;
    }
    
    int32 GetTimeMilli()
    {
        return (int32)(GetTimeMicro() / 1000);
    }
}}}