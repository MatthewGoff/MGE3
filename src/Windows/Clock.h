#ifndef CLOCK_H
#define CLOCK_H

namespace MGE { namespace Windows { namespace Clock
{
    void Initialize();
    int64 GetTimeMicro();
    int32 GetTimeMilli();
}}}

#endif