
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

        st = pc->Run();
        if (st != hoOK)
        {
            ret = -1;
            break;
        }

        delete pc;
    } while (false);

	return ret;
}