#ifndef WINDOWSOS_H
#define WINDOWSOS_H

#include "Log.h"

namespace WindowsOS
{
    uint32 ReadEntireFile(
        void* buffer_ptr,
        uint32 buffer_size,
        char* file_name);
}

#endif