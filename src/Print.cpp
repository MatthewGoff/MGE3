#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include "Print.h"

/* The body of each Print overload is identical only because I couldn't think of
 * anything better. As far as I know C++ doesn't have any "void" data type.
*/

int StringLength(char* string)
{
    int length;
    for (length = 0; *(string + length) != '\0'; length++){}
    return length;
}

void Print(char* string)
{
    int length = StringLength(string) + 100;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, string);
    OutputDebugString(my_buffer);
}

void Print(char* string, int32 var)
{
    int length = StringLength(string) + 100;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, string, var);
    OutputDebugString(my_buffer);
}

void Print(char* string, int64 var)
{
    int length = StringLength(string) + 100;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, string, var);
    OutputDebugString(my_buffer);
}

void Print(char* string, float var)
{
    int length = StringLength(string) + 100;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, string, var);
    OutputDebugString(my_buffer);
}

void Print(char* string, double var)
{
    int length = StringLength(string) + 100;
    char* my_buffer = (char*)alloca(length * sizeof(char));
    sprintf(my_buffer, string, var);
    OutputDebugString(my_buffer);
}