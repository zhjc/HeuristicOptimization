
#include "common/utility.h"

#include <fstream>
using namespace std;

HO_NAMESPACE_BEGIN(utility)

#ifdef PRINT_LOG
void LogInfo(const std::string& str)
{
#ifdef LOG_TO_FILE
#else
    std::cout << str << std::endl;
#endif // LOG_TO_FILE
}
#else
void LogInfo(const std::string& str){}
#endif

void PrintDistanceToFile(int nNode, double** distanceMatrix)
{
    ofstream out;
    out.open("out_pmed_pcenter.txt");

    for (int i = 0; i < nNode; ++i)
    {
        out << i << ": ";
        for (int j = 0; j < nNode; ++j)
        {
            out << distanceMatrix[i][j] << " ";
        }
        out << "\n";
    }

    out.close();
}

int ParseConfigFile()
{
    /*
    Json::Reader reader;
    Json::Value root;

    ifstream in(strconf);
    if (in.is_open())
    {
        if (reader.parse(in, root))
        {
            if (!root["name"].isNull())
            {
                strname = root["name"].asString();
            }

            if (!root["workpath"].isNull())
            {
                strworkpath = root["workpath"].asString();
            }

            if (!root["imgabusolutepath"].isNull())
            {
                string strabu = root["imgabusolutepath"].asString();
                if (strabu == "true")
                {
                    m_imgabusolutepath = true;
                }
            }

            if (!root["config"].isNull())
            {
                int configsize = root["config"].size();
                for (int i = 0; i < configsize; ++i)
                {
                    if (!root["config"][i]["dataset"].isNull())
                    {
                        strdataset = root["config"][i]["dataset"].asString();
                    }

                    if (!root["config"][i]["savepath"].isNull())
                    {
                        strsavepath = root["config"][i]["savepath"].asString();
                    }

                    if (!root["config"][i]["faceCascadeFilename"].isNull())
                    {
                        strfaceCascadeFilename = root["config"][i]["faceCascadeFilename"].asString();
                    }

                    if (!root["config"][i]["flandmarkModel"].isNull())
                    {
                        strflandmarkModel = root["config"][i]["flandmarkModel"].asString();
                    }
                }
            }
        }
        else
        {
            return -2;
        }
    }
    else
    {
        return -2;
    }*/

    return 0;
}

HO_NAMESPACE_END

