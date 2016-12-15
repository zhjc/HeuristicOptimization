
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

    pcenter::PCenter* pc = new pcenter::PCenter();

    pc->ReadFile(strfilename);

    //qsort(values, 6, sizeof(int), compare);
    //for (int n = 0; n < 6; n++)
    //    printf("%d ", values[n]);

    delete pc;

	return 0;
}