
#include "p_center_parameter.h"
#include "json/json.h"

#include <ctime>

using namespace std;

HO_NAMESPACE_BEGIN(utility)

HoConfigHandler::HoConfigHandler(const std::string& conffile)
: m_bRunSingleIns(true)
, m_bAbsolutePath(true)
, m_bPrintTime(true)
, m_bLogToFile(true)
, m_bLogRandomSeed(true)
{
    hoStatus rt = ParseConfigFile(conffile);
}

void HoConfigHandler::SetTimeSeed()
{
    unsigned int nseed = (unsigned)time(NULL);
    srand(nseed);
}

void HoConfigHandler::TimeBegin()
{
    m_timeStart = clock();
}

void HoConfigHandler::TimeStop()
{
    m_timeStop = clock();
}

void HoConfigHandler::PrintTimeInterval()
{
    double timeInterval = double(m_timeStop - m_timeStart) / CLOCKS_PER_SEC;
}


bool HoConfigHandler::IsPrintTime()
{
    return m_bPrintTime;
}

bool HoConfigHandler::IsLogToFile()
{
    return m_bLogToFile;
}

bool HoConfigHandler::IsLogRandomSeed()
{
    return m_bLogRandomSeed;
}

bool HoConfigHandler::IsRunSingleIns()
{
    return m_bRunSingleIns;
}

bool HoConfigHandler::IsAbsolutePath()
{
    return m_bAbsolutePath;
}

hoStatus HoConfigHandler::ParseConfigFile(const std::string& strConfFile)
{
    Json::Reader reader;
    Json::Value root;

    ifstream in(strConfFile);
    if (in.is_open())
    {
        if (reader.parse(in, root))
        {
            string strTmp;
            if (!root["name"].isNull())
            {
                m_strInsName = root["name"].asString();
            }

            if (!root["workpath"].isNull())
            {
                m_strWorkPath = root["workpath"].asString();
            }

            if (!root["imgabusolutepath"].isNull())
            {
                string strabu = root["imgabusolutepath"].asString();
                if (strabu == "true")
                {
                    m_bAbsolutePath = true;
                }
            }

            if (!root["config"].isNull())
            {
                int configsize = root["config"].size();
                for (int i = 0; i < configsize; ++i)
                {
                    if (!root["config"][i]["printtme"].isNull())
                    {
                        strTmp = root["config"][i]["printtme"].asString();
                        if (strTmp == "true")
                        {
                            m_bPrintTime = true;
                        }
                    }

                    if (!root["config"][i]["lograndomseed"].isNull())
                    {
                        strTmp = root["config"][i]["lograndomseed"].asString();
                        if (strTmp == "true")
                        {
                            m_bLogRandomSeed = true;
                        }
                    }
                }
            }
        }
        else
        {
            return hoError;
        }
    }
    else
    {
        return hoError;
    }

    return hoOK;
}

PCenterConfigHandler::PCenterConfigHandler(const std::string& conffile)
: HoConfigHandler(conffile)
, m_bTabuFVPair(true)
, m_bTabuVOnly(false)
, m_bTabuFOnly(false)
{
    hoStatus rt = ParseConfigFile(conffile);
}

bool PCenterConfigHandler::IsTabuFVPair()
{
    return m_bTabuFVPair;
}

bool PCenterConfigHandler::IsTabuFOnly()
{
    return m_bTabuFOnly;
}

bool PCenterConfigHandler::IsTabuVOnly()
{
    return m_bTabuVOnly;
}

hoStatus PCenterConfigHandler::ParseConfigFile(const std::string& strConfFile)
{
    Json::Reader reader;
    Json::Value root;

    ifstream in(strConfFile);
    if (in.is_open())
    {
        if (reader.parse(in, root))
        {
            string strTmp;
            
            if (!root["config"].isNull())
            {
                int configsize = root["config"].size();
                for (int i = 0; i < configsize; ++i)
                {
                    if (!root["config"][i]["tabu_f_u"].isNull())
                    {
                    strTmp = root["config"][i]["tabu_f_u"].asString();
                    if (strTmp == "true")
                    {
                    m_bTabuFVPair = true;
                    }
                    }

                    if (!root["config"][i]["tabu_f_only"].isNull())
                    {
                    strTmp = root["config"][i]["tabu_f_only"].asString();
                    if (strTmp == "true")
                    {
                    m_bTabuFOnly = true;
                    }
                    }

                    if (!root["config"][i]["tabu_u_only"].isNull())
                    {
                    strTmp = root["config"][i]["tabu_u_only"].asString();
                    if (strTmp == "true")
                    {
                    m_bTabuVOnly = true;
                    }
                    }

                    if (!root["config"][i]["divideofrandom"].isNull())
                    {
                    strTmp = root["config"][i]["divideofrandom"].asString();

                    }
                }
            }
        }
        else
        {
            return hoError;
        }
    }
    else
    {
        return hoError;
    }

    return hoOK;
}

HO_NAMESPACE_END