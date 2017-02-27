
#ifndef HO_PARAMETER_H
#define HO_PARAMETER_H

#include "config.h"
#include "common/ho_logger.h"

#include <ctime>
#include <string>

HO_NAMESPACE_BEGIN(utility)

class HoConfigHandler
{
public:
    HoConfigHandler(const std::string& conffile);

    virtual ~HoConfigHandler() {}

public:

    void SetRandomSeed(unsigned int nseed)
    {
        srand(nseed);
    }

    int IntRandom() const
    {
        return rand();
    }

    void TimeBegin()
    {
        m_timeStart = clock();
    }

    void TimeStop()
    {
        m_timeStop = clock();
    }

    double TimeInterval()
    {
        return double(m_timeStop - m_timeStart) / CLOCKS_PER_SEC;
    }

    void SetLogFileName(const std::string& strLogFileName)
    {
        m_strLogFile = strLogFileName;
    }

    const std::string& GetWorkPath() const
    {
        return m_strWorkPath;
    }

    const std::string& GetInstPath() const
    {
        return m_strInstPath;
    }

    const std::string& GetInstanceName() const
    {
        return m_strInsName;
    }

    const std::string& GetInstanceFile() const
    {
        return m_strInsFile;
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
    std::string m_strInsFile;

    std::string m_strInstPath;
    std::string m_strWorkPath;
    bool m_bAbsolutePath;
};

HO_NAMESPACE_END

#endif