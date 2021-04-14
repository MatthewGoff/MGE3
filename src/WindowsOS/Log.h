#ifndef LOG_H
#define LOG_H

namespace WindowsOS
{
    void Log(int lvl, char* msg);
    void Log(int lvl, int32 var);
    void Log(int lvl, int64 var);
    void Log(int lvl, uint32 var);
    void Log(int lvl, uint64 var);
    void Log(int lvl, float var);
    void Log(int lvl, double var);
}

#endif