#ifndef P_CENTER_H
#define P_CENTER_H

//#include "config.h"
//#include "graph.h"
#include <stdio.h>


typedef enum {
	pcOK,
	pcWarning,
	pcError,
	pcFatal
} pcStatus;

typedef enum {
	ORLIB,
	TSPLIB
} pcInputType;

typedef enum {
	NONE,
	USE_TABU
} pcSearchType;

void* pcInitialize(char* pFile);

pcStatus pcSetOption(void* pData, int argc, int* argv[]);

pcStatus pcLocalSearch(void* pData);

pcStatus pcPrintResult(void* pData, FILE* pFile);

pcStatus pcPrintF_D_Table(void* pData, FILE* filePath);

pcStatus pcFinalize(void* pData);


#endif