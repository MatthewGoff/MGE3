#ifndef WINDOWS_H
#define WINDOWS_H

namespace MGE { namespace Windows
{
    //WindowsOS.cpp
    uint32 ReadEntireFile(
        void* buffer_ptr,
        uint32 buffer_size,
        char* file_name);
    
    // Log.cpp
    void Log(int lvl, char* msg);
    void Log(int lvl, int32 var);
    void Log(int lvl, int64 var);
    void Log(int lvl, uint32 var);
    void Log(int lvl, uint64 var);
    void Log(int lvl, float var);
    void Log(int lvl, double var);
}}

#endif