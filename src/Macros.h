#pragma once

//#define UINT32_MAX (0xFFFFFFFF)

#define BITSPERBYTE (8)
#define KILOBYTES (1024)
#define MEGABYTES (1024 * KILOBYTES)
#define GIGABYTES (1024 * MEGABYTES)

#if 1
#define Assert(expression) if(!(expression)) {*(int * )0 = 0;}
#else
#define Assert(expression)
#endif

// Logging
//{
    #include PLATFORM_HEADER
    
    #if BUILD_LOG_LEVEL >= 5
    #define Debug(msg) PLATFORM::Log(5, msg);
    #else
    #define Debug(msg)
    #endif

    #if BUILD_LOG_LEVEL >= 4
    #define Info(msg) PLATFORM::Log(4, msg);
    #else
    #define Info(msg)
    #endif

    #if BUILD_LOG_LEVEL >= 3
    #define Warning(msg) PLATFORM::Log(3, msg);
    #else
    #define Warning(msg)
    #endif

    #if BUILD_LOG_LEVEL >= 2
    #define Error(msg) PLATFORM::Log(2, msg);
    #else
    #define Error(msg)
    #endif

    #if BUILD_LOG_LEVEL >= 1
    #define Fatal(msg) PLATFORM::Log(1, msg);
    #else
    #define Fatal(msg)
    #endif
//}
