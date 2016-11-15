
#include "p_center.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cassert>
#include <climits>
#include <ctime>

using namespace std;

#define FACILITY_NODE 1
#define USER_NODE 0

HO_NAMESPACE_BEGIN(pcenter)

enum fileType
{
    DIST_TYPE, // distance
    COOR_TYPE, // coordinate
    NONE_TYPE  // unknown type
};

struct tspPoint
{
    int id;
    double x;
    double y;
};

fileType GetInputFileType(const string& file);

template<class T>
void PrintOneColumn(T a[], int size)
{
    for (int i = 0; i < size; ++i)
    {
        cout << a[i] << " ";
    }

    cout << endl;
}

template<class T>
void PrintMatrix(T** a, int nr, int nc)
{
    cout << "matrix:\n";
    for (int i = 0; i < nr; ++i)
    {
        for (int j = 0; j < nc; ++j)
        {
            cout << a[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

PCenter::PCenter()
: m_nFacility(0)
, m_nCurFacility(0)
, m_nNodes(0)
, m_nEdges(0)
, m_curobjval(0)
, m_bestobjval(0)
, m_distanceGraph(hoNull)
, m_disSortedGraph(hoNull)
, m_disSequenceGraph(hoNull)
, m_dTable(hoNull)
, m_fTable(hoNull)
, m_bestsols(hoNull)
, m_cursols(hoNull)
, m_Sc(0)
{
    // nothing to do
}

hoStatus PCenter::Run()
{
    hoStatus st = hoOK;

    unsigned int seed = (unsigned int)time(hoNull);
    srand(seed);
    
    do 
    {
        // generate initial solution
        st = GenerateInitSol();
        if (st != hoOK)
        {
            break;
        }

        // local search
        st = LocalSearch();
        if (st != hoOK)
        {
            break;
        }

        PrintResultInfo();
    } while (false);

    return st;
}

hoStatus PCenter::GenerateInitSol()
{
    hoStatus st = hoOK;
    double dSc = 0.0;

    int firstnode = rand() % m_nNodes;

    AddFacility(firstnode, &dSc);

    while (m_nCurFacility < m_nFacility)
    {
        int curfacility = 0;
        AddFacility(curfacility, &dSc);
    }

    return st;
}

hoStatus PCenter::LocalSearch()
{
    hoStatus st = hoOK;
    return st;
}

void PCenter::PrintResultInfo()
{
    // TODO
}

void PCenter::PrintFAndDTable()
{
    cout << "F Table:\n";

    for (int i = 0; i < m_nNodes; ++i)
    {
        cout << i << " " << m_fTable[i].firf << " " << m_fTable[i].secf << "\n";
    }

    cout << "\nD Table:\n";
    for (int i = 0; i < m_nNodes; ++i)
    {
        cout << i << " " << m_dTable[i].fird << " " << m_dTable[i].secd << "\n";
    }

    cout << endl;
}

hoStatus PCenter::AddFacility(int facility, double* sc)
{
    hoStatus st = hoOK;

    double dSc = 0.0;

    m_cursols[facility] = FACILITY_NODE;
    m_nCurFacility++;

    for (int i = 0; i < m_nNodes; ++i)
    {
        if (m_distanceGraph[facility][i] < m_dTable[i].fird)
        {
            m_dTable[i].secd = m_dTable[i].fird;
            m_fTable[i].secf = m_fTable[i].firf;
            m_dTable[i].fird = m_distanceGraph[facility][i];
            m_fTable[i].firf = facility;
        }
        else if (m_distanceGraph[facility][i] < m_dTable[i].secd)
        {
            m_dTable[i].secd = m_distanceGraph[facility][i];
            m_fTable[i].secf = facility;
        }

        if (m_dTable[i].fird > dSc)
        {
            dSc = m_dTable[i].fird;
        }
    }

    *sc = dSc;
    
    return st;
}

hoStatus PCenter::RemoveFacility(int facility, double* sc)
{
    hoStatus st = hoOK;

    double dSc = 0.0;

    m_cursols[facility] = USER_NODE;
    m_nCurFacility--;

    for (int i = 0; i < m_nNodes; ++i)
    {
        double dsec = 0.0;
        int fsec = 0;

        if (m_fTable[i].firf == facility)
        {
            m_dTable[i].fird = m_dTable[i].secd;
            m_fTable[i].firf = m_fTable[i].secf;
            FindSec(&fsec, &dsec);
            m_dTable[i].secd = dsec;
            m_fTable[i].secf = fsec;
        }
        else if (m_fTable[i].secf == facility)
        {
            FindSec(&fsec, &dsec);
            m_dTable[i].secd = dsec;
            m_fTable[i].secf = fsec;
        }

        if (m_dTable[i].fird > dSc)
        {
            dSc = m_dTable[i].fird;
        }
    }

    *sc = dSc;

    return st;
}

hoStatus PCenter::FindSec(int* f, double* d)
{
    hoStatus st = hoOK;

    // TODO

    return st;
}

hoStatus PCenter::FindPair(int curf, SwapPair* sp)
{
    hoStatus st = hoOK;
    return st;
}

hoStatus PCenter::ReadFile(const string& file)
{
    hoStatus st = hoOK;
    
    do 
    {
        fileType ft = GetInputFileType(file);
        if (ft == DIST_TYPE)
        {
            fstream in(file);
            if (!in.is_open())
            {
                return hoInvalidFile;
            }

            in >> m_nNodes >> m_nEdges >> m_nFacility;

            st = AllocMemory();
            if (st != hoOK)
            {
                break;
            }

            InitData();

            int node1,node2,dis;
            for (int i = 0; i < m_nEdges; ++i)
            {
                in >> node1 >> node2 >> dis;
                m_distanceGraph[node1 - 1][node2 - 1] = dis;
                m_distanceGraph[node2 - 1][node1 - 1] = dis;
            }

            // PrintMatrix();
            GetShortPathByPloyd();
            // PrintMatrix();

            in.close();
        }
        else if (ft == COOR_TYPE)
        {
            fstream in(file);
            if (!in.is_open())
            {
                return hoInvalidFile;
            }

            in >> m_nNodes;

            m_nFacility = m_nNodes / 15; // TODO: set number of facility

            st = AllocMemory();
            if (st != hoOK)
            {
                break;
            }

            InitData();

            tspPoint* tp = new tspPoint[m_nNodes];
            int num, co_x, co_y;
            for (int i = 0; i < m_nNodes; ++i)
            {
                in >> num >> co_x >> co_y;
                tp[i].id = num;
                tp[i].x = co_x;
                tp[i].y = co_y;
            }

            //string streof;
            //in >> streof;
            //if (streof != "EOF")
            //{
            //    st = hoInvalidFile;
            //}

            // calculate Euclid Distance
            for (int i = 0; i < m_nNodes; ++i)
            {
                for (int j = i+1; j < m_nNodes; ++j)
                {
                    assert(i + 1 == tp[i].id);
                    assert(j + 1 == tp[j].id);
                    m_distanceGraph[i][j] = m_distanceGraph[j][i] = 
                        sqrt((tp[i].x - tp[j].x)*(tp[i].x - tp[j].x) + (tp[i].y - tp[j].y)*(tp[i].y - tp[j].y));
                }
            }

            // PrintMatrix();

            delete[] tp;
            in.close();
        }
        else
        {
            st = hoInvalidFile;
        }

        // sort distance matrix
        for (int i = 0; i < m_nNodes; ++i)
        {
            //PrintOneColumn(m_disSortedGraph[i], m_nNodes);
            QuickSort(m_disSortedGraph[i], i, 0, m_nNodes - 1);
            //PrintOneColumn(m_disSortedGraph[i], m_nNodes);
            //PrintOneColumn(m_distanceGraph[i], m_nNodes);
        }

        for (int i = 0; i < m_nNodes; ++i)
        {
            for (int j = 0; j < m_nNodes; ++j)
            {
                m_disSequenceGraph[i][m_disSortedGraph[i][j]] = j;
            }
        }

        //cout << "distance matrix:" << endl;
        //PrintDistanceMatrix();
        //cout << "sorted distance matrix:" << endl;
        //PrintMatrix(m_disSortedGraph, m_nNodes, m_nNodes);
        //cout << "number distance matrix:" << endl;
        //PrintMatrix(m_disSequenceGraph, m_nNodes, m_nNodes);
    } while (false);

    return st;
}

hoStatus PCenter::AllocMemory()
{
    m_distanceGraph = new double *[m_nNodes];
    m_disSortedGraph = new int *[m_nNodes];
    m_disSequenceGraph = new int *[m_nNodes];
    if (m_distanceGraph==hoNull || m_disSortedGraph==hoNull ||
        m_disSequenceGraph==hoNull)
    {
        // TODO: release newed data
        return hoMemoryOut;
    }

    for (int i = 0; i < m_nNodes; ++i)
    {
        m_distanceGraph[i] = new double[m_nNodes];
        m_disSortedGraph[i] = new int[m_nNodes];
        m_disSequenceGraph[i] = new int[m_nNodes];
        if (m_distanceGraph[i] == hoNull || m_disSortedGraph[i]==hoNull ||
            m_disSequenceGraph[i] == hoNull)
        {
            // TODO: release newed data
            return hoMemoryOut;
        }
    }

    m_fTable = new FTable[m_nNodes];
    m_dTable = new DTable[m_nNodes];

    m_bestsols = new int[m_nNodes];
    m_cursols = new int[m_nNodes];
    if (m_fTable==hoNull || m_dTable==hoNull ||
        m_bestsols==hoNull || m_cursols==hoNull)
    {
        // TODO: release newed memory
        return hoMemoryOut;
    }

    //InitData();

    return hoOK;
}

void PCenter::ReleaseMemory()
{
    if (m_distanceGraph != hoNull)
    {
        for (int i = 0; i < m_nNodes; ++i)
        {
            delete [] m_distanceGraph[i];
            m_distanceGraph[i] = hoNull;
        }
    }

    if (m_disSortedGraph != hoNull)
    {
        for (int i = 0; i < m_nNodes; ++i)
        {
            delete[] m_disSortedGraph[i];
            m_disSortedGraph[i] = hoNull;
        }
    }

    if (m_disSequenceGraph != hoNull)
    {
        for (int i = 0; i < m_nNodes; ++i)
        {
            delete[] m_disSequenceGraph[i];
            m_disSequenceGraph[i] = hoNull;
        }
    }

    delete[] m_distanceGraph;
    m_distanceGraph = hoNull;
    delete[] m_disSortedGraph;
    m_disSortedGraph = hoNull;
    delete[] m_disSequenceGraph;
    m_disSequenceGraph = hoNull;
    
    delete[] m_dTable;
    m_dTable = hoNull;
    delete[] m_fTable;
    m_fTable = hoNull;
    delete[] m_bestsols;
    m_bestsols = hoNull;
    delete[] m_cursols;
    m_cursols = hoNull;
}

void PCenter::InitData()
{
    for (int i = 0; i < m_nNodes; ++i)
    {
        for (int j = 0; j < m_nNodes; j++)
        {
            m_distanceGraph[i][j] = 0;
            m_disSortedGraph[i][j] = j;
            m_disSequenceGraph[i][j] = j;
        }
    }

    for (int i = 0; i < m_nNodes; ++i)
    {
        m_fTable[i].firf = INT_MAX;
        m_fTable[i].secf = INT_MAX;

        m_dTable[i].fird = DBL_MAX;
        m_dTable[i].secd = DBL_MAX;

        m_bestsols[i] = 0;
        m_cursols[i] = 0;
    }
}

void PCenter::PrintDistanceMatrix()
{
    cout << "distance matrix:\n";
    for (int i = 0; i < m_nNodes; ++i)
    {
        for (int j = 0; j < m_nNodes; ++j)
        {
            cout << m_distanceGraph[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void PCenter::GetShortPathByPloyd()
{
    for (int i = 0; i < m_nNodes; i++)
    {
        for (int j = 0; j < m_nNodes; j++)
        {
            if (i == j)
            {
                continue;
            }

            for (int k = 0; k < m_nNodes; k++)
            {
                if (j == k)
                {
                    continue;
                }

                if (m_distanceGraph[j][i] && m_distanceGraph[i][k])
                {
                    if (m_distanceGraph[j][k] == 0)
                    {
                        m_distanceGraph[j][k] = m_distanceGraph[j][i] + m_distanceGraph[i][k];
                    }
                    else
                    {
                        if (m_distanceGraph[j][i] + m_distanceGraph[i][k] < m_distanceGraph[j][k])
                        {
                            m_distanceGraph[j][k] = m_distanceGraph[j][i] + m_distanceGraph[i][k];
                        }
                    }
                }
            }
        }
    }
}

void PCenter::QuickSort(int a[], int curr, int l, int r)
{
	// partition
    if (r <= l)
    {
        return;
    }

    int i = l-1;
    int j = r;

    int t = a[r];

    for (;;)
    {
        while (m_distanceGraph[curr][a[++i]] < m_distanceGraph[curr][t])
        {
            // nothing to do
        }

        while (m_distanceGraph[curr][t] < m_distanceGraph[curr][a[--j]])
        {
            if (j == l)
            {
                break;
            }
        }

        if (i >= j)
        {
            break;
        }

        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }

    int tmp = a[i];
    a[i] = a[r];
    a[r] = tmp;

	// quick sort
    QuickSort(a, curr, l, i - 1);
    QuickSort(a, curr, i + 1, r);
}

PCenter::~PCenter()
{
    ReleaseMemory();
}

fileType GetInputFileType(const string& file)
{
    string strfilename = file;
    size_t index = strfilename.find_last_of(".");
    if (index != string::npos)
    {
        string strext = strfilename.substr(index, strfilename.size() - 1);
        if (strext == ".txt")
        {
            return DIST_TYPE;
        }
        else if (strext == ".tsp")
        {
            return COOR_TYPE;
        }
    }

    return NONE_TYPE;
}

HO_NAMESPACE_END