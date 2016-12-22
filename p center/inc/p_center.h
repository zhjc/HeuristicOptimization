
#ifndef _HO_P_CENTER_H
#define _HO_P_CENTER_H

#include "config.h"
#include <string>
#include <vector>

HO_NAMESPACE_BEGIN(pcenter)

#define PRINT_LOG

struct FTable
{
    int firf;
    int secf;
};

struct DTable
{
    double fird;
    double secd;
};

struct SwapPair
{
    int facility;
    int vertex;
};

class PCenter
{
public:
    PCenter();
    ~PCenter();

    hoStatus Run();

    hoStatus GenerateInitSol();
    hoStatus LocalSearch();
    
    hoStatus AddFacility(int facility, double* sc);
    hoStatus RemoveFacility(int facility, double* sc);
    hoStatus FindPair(int curf, double d, std::vector<SwapPair>* vecsp, long iter);
    hoStatus FindSec(int curnode, int* f, double* d);

    hoStatus ReadFile(const std::string& file);

    void PrintResultInfo(bool onlyfacility=true);
    void PrintFAndDTable();

private:
    hoStatus AllocMemory();
    void ReleaseMemory();
    void InitData();

    void PrintDistanceMatrix();
    void GetShortPathByPloyd();
    void QuickSort(int a[], int curr, int l, int r);

private:
    FTable* m_fTable;
    DTable* m_dTable;

    FTable* m_fTable_copy;
    DTable* m_dTable_copy;

    int m_nNodes;
    int m_nFacility;
    int m_nCurFacility;
    int m_nEdges;

    double m_bestobjval;
    int* m_bestsols;

    double m_curobjval;
    int* m_cursols;
    
    double ** m_distanceGraph;
    int ** m_disSortedGraph;
    int ** m_disSequenceGraph;

    int** m_TabuList;

    double* m_f;

    int m_Sc;
};

HO_NAMESPACE_END

#endif