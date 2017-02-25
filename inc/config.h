
#ifndef _HO_CONFIG_H
#define _HO_CONFIG_H

// NOTE: HO -- heuristic optimization

// namesapce definition
#ifndef NOT_USING_NAMESPACE
#define HO_NAMESPACE_BEGIN(n) namespace n {
#define HO_NAMESPACE_END }
#else
#define HO_NAMESPACE_BEGIN
#define HO_NAMESPACE_END
#endif

// ÔËÐÐ×´Ì¬
typedef enum
{
    hoOK = 0,
    hoWarning,
    hoError,
    hoFatal,
    hoMemoryOut,
    hoFileOpenError,
    hoInvalidFile
} hoStatus;

typedef enum
{
    hoInitialize,
    hoRunning,
    hoTerminate
} hoRunningMode;

#define hoNull 0 
#define BUFFER_SIZE 128

#endif