
#include "p_center_parameter.h"
#include "json/json.h"

#include <fstream>

using namespace std;

HO_NAMESPACE_BEGIN(utility)

PCenterConfigHandler::PCenterConfigHandler(const std::string& conffile)
: HoConfigHandler(conffile)
, m_bTabuFVPair(true)
, m_bTabuVOnly(false)
, m_bTabuFOnly(false)
, m_nAddLengthOfTabu(0)
, m_dMutationProb(0.03)
, m_dCrossoverProb(0.8)
{
    hoStatus rt = ParseConfigFile(conffile);
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
                        m_nAddLengthOfTabu = stoi(strTmp.c_str());
                    }

                    if (!root["config"][i]["mutationprob"].isNull())
                    {
                        strTmp = root["config"][i]["mutationprob"].asString();
                        m_dMutationProb = stof(strTmp.c_str());
                    }

                    if (!root["config"][i]["crossoverprob"].isNull())
                    {
                        strTmp = root["config"][i]["crossoverprob"].asString();
                        m_dCrossoverProb = stof(strTmp.c_str());
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