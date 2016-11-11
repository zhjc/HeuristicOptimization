
#include <iostream>
#include <string>
#include "p_center.h"

using namespace std;

int main(int argc, char* argv[])
{
    string strfilename = argv[1];

    pcenter::PCenter* pc = new pcenter::PCenter();

    pc->ReadFile(strfilename);

    delete pc;

	return 0;
}