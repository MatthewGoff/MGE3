#ifndef LOGGING_H
#define LOGGING_H

#define BUILD_LOG_LEVEL 5
    
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

#endif