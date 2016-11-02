
#include "graph_coloring.h"
#include <string>
using namespace std;

int main(int argc, char* argv[])
{
	int ret = 0;

	// parse command line arguments
	string strinsfile = argv[1];
	int numc = 52;

	do
	{
		gcp::GraphColoring gcpc;
		hoStatus stu = gcpc.LoadInsFile(strinsfile, numc);
		if (stu != hoOK)
		{
			ret = -1;
			break;
		}

		if (numc > 0)
		{
			stu = gcpc.RandomInitColor();
		}

		stu = gcpc.TS();
		if (stu != hoOK)
		{
			ret = -1;
			break;
		}

		gcpc.PrintTabuSolInfo();

	} while (false);

	return ret;
}