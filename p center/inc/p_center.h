
#ifndef _HO_P_CENTER_H
#define _HO_P_CENTER_H

#include "config.h"
#include <string>

HO_NAMESPACE_BEGIN(pcenter)

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

    hoStatus ReadFile(const std::string& file);

private:
    hoStatus AllocMemory();
    void ReleaseMemory();
    void InitData();

    void PrintMatrix();
    void GetShortPathByPloyd();

private:
    FTable* m_fTable;
    DTable* m_dTable;

    int m_nNodes;
    int m_nFacility;
    int m_nEdges;

    int m_bestobjval;
    int* m_bestsols;

    int m_curobjval;
    int* m_cursols;
    
    double ** m_distanceGraph;
    double ** m_disSortedGraph;
    double ** m_disSequenceGraph;
};

HO_NAMESPACE_END

#endif