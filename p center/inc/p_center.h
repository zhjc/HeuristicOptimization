
#ifndef _HO_P_CENTER_H
#define _HO_P_CENTER_H

#include "config.h"

#include <string>
#include <vector>

HO_NAMESPACE_BEGIN(utility)
class PCenterConfigHandler;
class HoLogger;
HO_NAMESPACE_END

HO_NAMESPACE_BEGIN(pcenter)

#define PRINT_LOG
//#define OPT_FIND_SEC
//#define OUTPUT_NW

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

    SwapPair() :
        facility(0), vertex(0) {}

    SwapPair(int f, int v) :
        facility(f), vertex(v) {}
};

class PCenter
{
public:
    PCenter(utility::PCenterConfigHandler* config);
    ~PCenter();

    hoStatus RunSinglePhase(int run_type);
    hoStatus RunPopulationPhase(int run_type);

    hoStatus GeneticAlgorithm();
    hoStatus Mutation1(int* sols);
    hoStatus Mutation2(int* sols);
    hoStatus Crossover1(int* sols1, int* sols2);
    hoStatus Crossover2(int* sols1, int* sols2);

    hoStatus GenerateInitSol();
    hoStatus LocalSearch();
    
    hoStatus AddFacility(int facility, double* sc);
    hoStatus RemoveFacility(int facility, double* sc);
    hoStatus FindPair(int curf, double d, std::vector<SwapPair>* vecsp, long iter);
    hoStatus FindPair_new(int curf, double d, std::vector<SwapPair>* vecsp, long iter);
    hoStatus FindPair_3(int, double, std::vector<SwapPair>*, long);
    hoStatus FindSec(int curnode, int delf, int* f, double* d, bool isfirf=true);

    hoStatus ReadFile(const std::string& file);

    void PrintResultInfo(bool onlyfacility=true);
    void PrintFAndDTable();

    void SetLogger(utility::HoLogger* pLogger)
    {
        m_logger = pLogger;
    }

private:
    bool isTabu(int facility, int user, int iter);
    void setTabu(int facility, int user, int iter);

private:
    hoStatus AllocMemory();
    void ReleaseMemory();
    void InitData();

    void PrintDistanceMatrix();
    void GetShortPathByPloyd();
    void QuickSort(int a[], int curr, int l, int r);

    hoStatus AddFacilityInternal();

private:
    FTable* m_fTable;
    DTable* m_dTable;
    FTable* m_fTable_copy;
    DTable* m_dTable_copy;

    hoRunningMode m_runningMode;

    int m_nNodes;
    int m_nEdges;
    int m_nFacility;
    int m_nCurFacility;
    
    double m_bestobjval;
    int* m_bestsols;

    double m_curobjval;
    int* m_cursols;

    int* m_facilities;
    
    double ** m_distanceGraph;
    int ** m_disSortedGraph;

    int* m_tabuNode;
    int* m_tabuFacility;

    double* m_f;


    // discard
    double m_Sc;
    utility::PCenterConfigHandler* m_config;
    utility::HoLogger* m_logger;
};

HO_NAMESPACE_END

#endif