
#include "common/ho_parameter.h"
#include "json/json.h"

#include <fstream>

using namespace std;

HO_NAMESPACE_BEGIN(utility)

HoConfigHandler::HoConfigHandler(const std::string& conffile)
: m_bRunSingleIns(true)
, m_bAbsolutePath(true)
, m_bPrintTime(true)
, m_bLogToFile(true)
, m_bLogRandomSeed(true)
, m_logLevel(hoLogInfo)
, m_timeStart(0)
, m_timeStop(0)
{
    hoStatus rt = ParseConfigFile(conffile);
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
            if (!root["insname"].isNull())
            {
                m_strInsName = root["insname"].asString();
            }

            if (!root["workpath"].isNull())
            {
                m_strWorkPath = root["workpath"].asString();
                if (m_strWorkPath.back() != '\\' && m_strWorkPath.back() != '/')
                {
                    m_strWorkPath += "\\";
                }
            }

            if (!root["instpath"].isNull())
            {
                m_strInstPath = root["instpath"].asString();
                if (m_strInstPath.back() != '\\' && m_strInstPath.back() != '/')
                {
                    m_strInstPath += "\\";
                }
            }

            if (!root["abusolutepath"].isNull())
            {
                string strabu = root["abusolutepath"].asString();
                if (strabu == "true")
                {
                    m_bAbsolutePath = true;
                }
            }

            if (!root["runsingle"].isNull())
            {
                string strabu = root["runsingle"].asString();
                if (strabu == "false")
                {
                    m_bRunSingleIns = false;
                }
            }

            if (!root["instfile"].isNull())
            {
                m_strInsFile = root["instfile"].asString();
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

HO_NAMESPACE_END
