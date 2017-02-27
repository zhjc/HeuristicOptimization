
#ifndef HO_LOGGER_H
#define HO_LOGGER_H

#include "config.h"
#include <string>
#include <fstream>

HO_NAMESPACE_BEGIN(utility)

typedef enum
{
    hoLogDebug = 0,
    hoLogInfo,
    hoLogWarning,
    hoLogError
} hoLogType;

typedef enum
{
    hoLog2Std = 0,
    hoLog2File
} hoLogOutType;

class HoLogger
{
public:
    HoLogger(const std::string& strLogName);

    virtual ~HoLogger();

    void LogInfo(const std::string& strMsg);

private:
    std::ofstream m_logFile;
    hoLogOutType m_logType;
};

HO_NAMESPACE_END

#endif