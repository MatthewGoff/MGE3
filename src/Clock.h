#ifndef CLOCK_H
#define CLOCK_H

#include <Windows.h>

namespace Clock
{
    namespace
    {
        uint Frequency;
    }
    void Initialize();
    uint GetTime(); // in micro seconds
}

#endif