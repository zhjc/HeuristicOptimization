
#ifndef P_CENTER_PARAMETER_H
#define P_CENTER_PARAMETER_H

#include "common/utility.h"

#include <fstream>
#include <ctime>
#include <string>
using namespace std;

HO_NAMESPACE_BEGIN(utility)

class HoConfigHandler
{
public:
    HoConfigHandler(const std::string& conffile);

public:

    void SetTimeSeed();
    void TimeBegin();
    void TimeStop();
    void PrintTimeInterval();

    // 设置日志文件名称
    void SetLogFileName(const std::string& strLogFileName)
    {
        m_strLogFile = strLogFileName;
    }

    bool IsPrintTime();
    bool IsLogToFile();
    bool IsLogRandomSeed();
    bool IsRunSingleIns();
    bool IsAbsolutePath();

    virtual hoStatus ParseConfigFile(const std::string& strConfFile);

private:
    // time
    time_t m_timeStart, m_timeStop;
    bool m_bPrintTime;

    // log
    hoLogType m_logLevel;
    bool m_bLogToFile;
    std::string m_strLogFile;

    bool m_bLogRandomSeed;

    bool m_bRunSingleIns;
    std::string m_strInsName;

    std::string m_strWorkPath;
    bool m_bAbsolutePath;
};

class PCenterConfigHandler : public HoConfigHandler
{
public:
    PCenterConfigHandler(const std::string& conffile);

public:
    
    bool IsTabuFVPair();
    bool IsTabuFOnly();
    bool IsTabuVOnly();
    
    hoStatus ParseConfigFile(const std::string& strConfFile);

private:

    // other config
    bool m_bTabuFVPair;
    bool m_bTabuFOnly;
    bool m_bTabuVOnly;
};

HO_NAMESPACE_END

#endif
