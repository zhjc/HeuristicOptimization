
#include "p_center.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cassert>

using namespace std;

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

PCenter::PCenter()
: m_nFacility(0)
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
{
    // nothing to do
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

hoStatus PCenter::ReadFile(const string& file)
{
    hoStatus st = hoOK;
    
    do 
    {
        fileType ft = GetInputFileType(file);
        if (ft == DIST_TYPE)
        {
            fstream in(file);
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

            in >> m_nNodes;
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
            for (int j = 0; j < m_nNodes; ++j)
            {

            }
        }

    } while (false);

    return st;
}

hoStatus PCenter::AllocMemory()
{
    m_distanceGraph = new double *[m_nNodes];
    m_disSortedGraph = new double *[m_nNodes];
    m_disSequenceGraph = new double *[m_nNodes];
    if (m_distanceGraph==hoNull || m_disSortedGraph==hoNull ||
        m_disSequenceGraph==hoNull)
    {
        // TODO: release newed data
        return hoMemoryOut;
    }

    for (int i = 0; i < m_nNodes; ++i)
    {
        m_distanceGraph[i] = new double[m_nNodes];
        m_disSortedGraph[i] = new double[m_nNodes];
        m_disSequenceGraph[i] = new double[m_nNodes];
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
            m_disSortedGraph[i][j] = 0;
            m_disSequenceGraph[i][j] = j;
        }
    }

    for (int i = 0; i < m_nNodes; ++i)
    {
        m_fTable[i].firf = 0;
        m_fTable[i].secf = 0;

        m_dTable[i].fird = 0.0;
        m_dTable[i].secd = 0.0;

        m_bestsols[0] = 0;
        m_cursols[0] = 0;
    }
}

void PCenter::PrintMatrix()
{
    cout << "current matrix:\n";
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

PCenter::~PCenter()
{
    ReleaseMemory();
}

HO_NAMESPACE_END