
#include <iostream>
#include <string>
#include "p_center.h"
#include "p_center_parameter.h"

#include "common/ho_file_handler.h"

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

    // �������е��㷨���ͣ����ڵ���������Ⱥ�㷨��
    // ��Ⱥ���㷨���
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

    for (size_t i = 0; i < vecFiles.size(); ++i)
    {
        do
        {
            // ƴ����־�ļ�����
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

            st = pc->RunSinglePhase(run_type);
            if (st != hoOK)
            {
                ret = -1;
                delete pc;
                break;
            }

            delete pc;
            delete plogger;
        } while (false);
    }
    
    delete pcp;

	return ret;
}