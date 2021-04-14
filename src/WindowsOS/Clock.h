#ifndef CLOCK_H
#define CLOCK_H

namespace WindowsOS {
namespace Clock
{
    void Initialize();
    int64 GetTimeMicro();
    int32 GetTimeMilli();
}
}

#endif