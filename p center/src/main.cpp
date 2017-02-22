
#include <iostream>
#include <string>
#include "p_center.h"

using namespace std;

//int values[] = { 40, 10, 100, 90, 20, 25 };

int compare(const void * a, const void * b)
{
    return (*(int*)a - *(int*)b);
}

int main(int argc, char* argv[])
{
    string strfilename = argv[1];

    int ret = 0;

    // 区分运行的算法类型（基于单个或是种群算法）
    // 种群中算法类别
    int run_type = 0;

    do 
    {
        pcenter::PCenter* pc = new pcenter::PCenter();
        hoStatus st = hoOK;
        st = pc->ReadFile(strfilename);
        if (st != hoOK)
        {
            ret = -1;
            break;
        }

        st = pc->RunSinglePhase(run_type);
        if (st != hoOK)
        {
            ret = -1;
            break;
        }

        delete pc;
    } while (false);

	return ret;
}