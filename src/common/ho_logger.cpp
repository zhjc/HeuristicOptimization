
#include "common/ho_logger.h"
#include <iostream>

using namespace std;

HO_NAMESPACE_BEGIN(utility)

HoLogger::HoLogger(const string& strLogName)
{
    if (strLogName != "")
    {
        m_logFile.open(strLogName);
        if (!m_logFile.is_open())
        {
            // error
            int m = -1;
        }
        m_logType = hoLog2File;
    }
    else
    {
        m_logType = hoLog2Std;
    }
}

void HoLogger::LogInfo(const std::string& strMsg)
{
    if (!m_logFile.is_open())
    {
        return;
    }

    if (m_logType == hoLog2File)
    {
        m_logFile << strMsg << endl;
    }
    else
    {
        cout << strMsg << endl;
    }
}

HoLogger::~HoLogger()
{
    if (m_logFile.is_open())
    {
        m_logFile.close();
    }
}

HO_NAMESPACE_END