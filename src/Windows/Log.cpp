#include <windows.h>
#include <stdio.h>

namespace MGE { namespace Windows {

int StringLength(char* string)
{
    int length;
    for (length = 0; *(string + length) != '\0'; length++){}
    return length;
}

void Log(int lvl, char* msg)
{          
    int length = StringLength(msg) + 32;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, msg);
    OutputDebugString(my_buffer);
}

void Log(int lvl, int32 var)
{
    int length = 32;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, "%d", var);
    OutputDebugString(my_buffer);
}

void Log(int lvl, float var)
{
    int length = 32;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, "%f", var);
    OutputDebugString(my_buffer);
}

void Log(int lvl, int64 var)
{
    Log(lvl, (int32)var);
}

void Log(int lvl, uint32 var)
{
    Log(lvl, (int32)var);
}

void Log(int lvl, uint64 var)
{
    Log(lvl, (int32)var);
}

void Log(int lvl, double var)
{
    Log(lvl, (float)var);
}

}}