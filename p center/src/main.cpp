
#include <iostream>
#include <string>
#include "p_center.h"
#include "p_center_parameter.h"

#include "common/ho_file_handler.h"
#include "common/utility.h"

#include <ctime>

using namespace std;
using namespace utility;

//int values[] = { 40, 10, 100, 90, 20, 25 };

int compare(const void * a, const void * b)
{
    return (*(int*)a - *(int*)b);
}

int main(int argc, char* argv[])
{
    string strfilename = argv[1];

    PCenterConfigHandler* pcp = new PCenterConfigHandler(strfilename);

    int ret = 0;
    bool bRunSingle = pcp->IsRunSingleIns();

    // 区分运行的算法类型（基于单个或是种群算法）
    // 种群中算法类别
    int run_type = 0;

    vector<string> vecFiles;
    if (!bRunSingle)
    {
        HoFileHandler* pfh = new HoFileHandler();
        //pfh->BrouseDirectory(pcp->GetInstPath()+pcp->GetInstanceName());
        //pfh->GetFilesSet(&vecFiles);
        pfh->GetFilesSetFromFile(pcp->GetInstanceFile(), &vecFiles);
    }
    else
    {
        vecFiles.push_back(pcp->GetInstPath()+pcp->GetInstanceName());
    }

    int com_count = 6;

    for (size_t i = 0; i < vecFiles.size(); ++i)
    {
        do
        {
            // 拼接日志文件名称
            string strLogFileName = vecFiles[i];
            string::size_type last = strLogFileName.rfind("\\");
            if (last == string::npos)
            {
                last = strLogFileName.rfind("/");
            }

            string strlogprefixext = strLogFileName.substr(last+1, strLogFileName.size() - 4);
            string strlogprefix;
            for (size_t j = 0; j < strlogprefixext.size(); ++j)
            {
                if (strlogprefixext[j] == '.')
                {
                    break;
                }
                strlogprefix.push_back(strlogprefixext[j]);
            }

            strlogprefix += "_";
            strlogprefix += ToStr(com_count);

            HoLogger* plogger = new HoLogger(pcp->GetWorkPath()+strlogprefix+"_log.txt");

            pcenter::PCenter* pc = new pcenter::PCenter(pcp);

            pc->SetLogger(plogger);

            hoStatus st = hoOK;
            st = pc->ReadFile(vecFiles[i]);
            if (st != hoOK)
            {
                ret = -1;
                delete pc;
                break;
            }

            clock_t start = clock();
            st = pc->RunSinglePhase(run_type);
            clock_t end = clock();

            cout << "run time:" << (double)(end - start) / CLK_TCK << "s" << endl;

            if (st != hoOK)
            {
                ret = -1;
                delete pc;
                break;
            }

            delete pc;
            delete plogger;

            com_count++;
        } while (com_count<5);
    }
    
    delete pcp;

	return ret;
}