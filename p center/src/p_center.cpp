
#include "p_center.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cassert>
#include <climits>
#include <ctime>
#include <vector>

#include "common/utility.h"

using namespace std;
using namespace utility;

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
void PrintMatrix(T** a, int nr, int nc, bool tofile = false)
{
    if (tofile)
    {
        ofstream ofile("D:/TEMP/11.txt", ios::app);
        ofile << "matrix:\n";
        for (int i = 0; i < nr; ++i)
        {
            for (int j = 0; j < nc; ++j)
            {
                ofile << a[i][j] << " ";
            }
            ofile << endl;
        }
        ofile << endl;

        ofile.close();
    }
    else
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
}

PCenter::PCenter()
: m_nFacility(0)
, m_nCurFacility(0)
, m_nNodes(0)
, m_nEdges(0)
, m_curobjval(0)
, m_bestobjval(DBL_MAX)
, m_distanceGraph(hoNull)
, m_disSortedGraph(hoNull)
, m_disSequenceGraph(hoNull)
, m_TabuList(hoNull)
, m_dTable(hoNull)
, m_fTable(hoNull)
, m_fTable_copy(hoNull)
, m_dTable_copy(hoNull)
, m_bestsols(hoNull)
, m_cursols(hoNull)
, m_Sc(0.0)
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

hoStatus PCenter::AddFacilityInternal()
{
    double dSc = 0.0;  // 文献中Sc表示最大的服务边

    while (m_nCurFacility < m_nFacility)
    {
        double dmaxdis = 0.0;
        vector<int> vecmaxdis;

        for (int i = 0; i < m_nNodes; ++i)
        {
            if (m_cursols[i] != FACILITY_NODE)
            {
                if (dmaxdis < m_dTable[i].fird)
                {
                    dmaxdis = m_dTable[i].fird;
                    vecmaxdis.clear();
                    vecmaxdis.push_back(i);
                }
                else if (dmaxdis == m_dTable[i].fird)
                {
                    vecmaxdis.push_back(i);
                }
            }
        }

        assert(vecmaxdis.size() > 0);

        int nusercenter = vecmaxdis[rand() % vecmaxdis.size()];

        LogInfo("selected node of longest edge is " + ToStr(nusercenter));

        int nfacility = 0;
        vector<int> vecnewuser;
        for (int i = 0; i < m_nNodes; ++i)
        {
            if (m_disSortedGraph[nusercenter][i] == m_fTable[nusercenter].firf)
            {
                break;
            }
            else
            {
                if (m_cursols[m_disSortedGraph[nusercenter][i]] == FACILITY_NODE)
                {
                    continue;
                }

                vecnewuser.push_back(m_disSortedGraph[nusercenter][i]);
            }
        }

        assert(vecnewuser.size() > 0);
        nfacility = vecnewuser[rand() % vecnewuser.size()];

        AddFacility(nfacility, &dSc);
        LogInfo("selected facility is " + ToStr(nfacility));
        //PrintFAndDTable();
    }

    return hoOK;
}

hoStatus PCenter::GenerateInitSol()
{
    hoStatus st = hoOK;
    double dSc = 0.0;

    int firstnode = rand() % m_nNodes;

    LogInfo("Begin generate initial solution!");

    AddFacility(firstnode, &dSc);
    //PrintFAndDTable();
    LogInfo("The first facility " + ToStr(firstnode));

    st = AddFacilityInternal();

    PrintResultInfo();

    return st;
}

hoStatus PCenter::LocalSearch()
{
    hoStatus st = hoOK;

    st = AddFacilityInternal(); // make sure has p facilities

    LogInfo("Begin local search:");

    long nmaxiter = 500;
    long iter = 0;
    SwapPair sp;

    m_TabuList = new int*[m_nNodes];
    for (int i = 0; i < m_nNodes; ++i)
    {
        m_TabuList[i] = new int[m_nNodes];
        for (int j = 0; j < m_nNodes; ++j)
        {
            m_TabuList[i][j] = 0;
        }
    }

    while (iter < nmaxiter)
    {
        double dMaxDistance = 0.0;
        vector<int> vecMaxDistanceNode;
        for (int i = 0; i < m_nNodes; ++i)
        {
            if (dMaxDistance < m_dTable[i].fird)
            {
                dMaxDistance = m_dTable[i].fird;
                vecMaxDistanceNode.clear();
                vecMaxDistanceNode.push_back(i);
            }
            else if (dMaxDistance == m_dTable[i].fird)
            {
                vecMaxDistanceNode.push_back(i);
            }
        }

        assert(vecMaxDistanceNode.size()>0);

        int nSelect = vecMaxDistanceNode[rand() % vecMaxDistanceNode.size()];

        LogInfo("selected node of maximum length " + ToStr(nSelect) + "");

        // try nodes in circle (nSelect, dMaxDistance), choose the best one
        vector<SwapPair> vecsps;
        FindPair(nSelect, dMaxDistance, &vecsps, iter);

        if (vecsps.empty())
        {
            iter++;
            continue;
        }

        assert(vecsps.size() > 0);
        sp = vecsps[rand()%vecsps.size()];

        if (false && sp.facility == sp.vertex)
        {
            LogInfo("selected node " + ToStr(sp.facility) + " for swap " + ToStr(sp.vertex));
            LogInfo("stop procedure!");
            break;
        }

        LogInfo("selected node " + ToStr(sp.vertex) + " for swap " + ToStr(sp.facility));

        // do swap operation and set TabuList
        double dSc = 0.0;
        AddFacility(sp.vertex, &dSc);
        RemoveFacility(sp.facility, &dSc);
        m_TabuList[sp.facility][sp.vertex] = iter + rand() % 10 + m_nNodes / 10;

        for (int i = 0; i < m_nNodes; ++i)
        {
            if (m_dTable[i].fird > m_curobjval)
            {
                m_curobjval = m_dTable[i].fird;
            }
        }

        // update best solution
        if (m_curobjval < m_bestobjval)
        {
            m_bestobjval = m_curobjval;
            for (int i = 0; i < m_nNodes; ++i)
            {
                m_bestsols[i] = m_cursols[i];
            }
        }

        iter++;
    }

    LogInfo("finish local search");

    //PrintResultInfo();
    
    for (int i = 0; i < m_nNodes; ++i)
    {
        delete[] m_TabuList[i];
    }
    delete[] m_TabuList;
    
    return st;
}

hoStatus PCenter::AddFacility(int facility, double* sc)
{
    hoStatus st = hoOK;

    double dSc = 0.0; // p+1个结点时的最大边Sc

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

    double dSc = 0.0; // 删除一个结点后变成p个结点时最大边，也即Mf   // 文献中Sc表示最大的服务边

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
            FindSec(i, &fsec, &dsec);
            m_dTable[i].secd = dsec;
            m_fTable[i].secf = fsec;
        }
        else if (m_fTable[i].secf == facility)
        {
            FindSec(i, &fsec, &dsec);
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

// find second facility and distance（扫描当前解搜寻次近邻的服务点）
hoStatus PCenter::FindSec(int curnode, int* f, double* d)
{
    hoStatus st = hoOK;

    *f = INT_MAX;
    *d = DBL_MAX;

    for (int i = 0; i < m_nNodes; ++i)
    {
        if (i!=curnode && m_cursols[i]==FACILITY_NODE && i!=m_fTable[i].firf)
        {
            if (m_distanceGraph[curnode][i] < *d)
            {
                *d = m_distanceGraph[curnode][i];
                *f = i;
            }
        }
    }

    return st;
}

hoStatus PCenter::FindPair(int curf, double dmaxdis, vector<SwapPair>* vecsp, long iter)
{
    hoStatus st = hoOK;

    double dC = 0.0;

    // 需要计算m_curobjval, m_cursols
    // 判断是否在禁忌之中

    /*vector<int> vecMaxDis;
    // 找到最长边dij
    for (int i = 0; i < m_nNodes; ++i)
    {
        if (m_dTable[i].fird > dmax)
        {
            dmax = m_dTable[i].fird;
            vecMaxDis.resize(0);
            vecMaxDis.push_back(i); // push的node必为用户节点
        }
        else if (m_dTable[i].fird == dmax)
        {
            vecMaxDis.push_back(i);
        }
    }

    assert(vecMaxDis.size() > 0);
    int selectnode = vecMaxDis[rand()%vecMaxDis.size()];
    LogInfo("find pair selected node " + ToStr(selectnode) + " for swap");*/

    int selectnode = curf;
    double dmax = dmaxdis;

    int serverofselectnode = m_fTable[selectnode].firf;

    // 找到selectnode为圆心，其到服务点的距离为半径的圆内的点，随机选择一个作为将要添加的结点，即选择Nwk
    vector<int> vecAddSets;
    for (int i = 0; i < m_nNodes;++i)
    {
        if (m_disSortedGraph[selectnode][i] == serverofselectnode)
        {
            break;
        }
        else
        {
            if (m_cursols[m_disSortedGraph[selectnode][i]] == FACILITY_NODE)
            {
                continue;
            }

            vecAddSets.push_back(m_disSortedGraph[selectnode][i]); // ??是否包含中心设施
        }
    }

    // 存储现场
    //memcpy(m_fTable_copy, m_fTable, sizeof(FTable)*m_nNodes);
    //memcpy(m_dTable_copy, m_dTable, sizeof(DTable)*m_nNodes);
    for (int j = 0; j < m_nNodes; ++j)
    {
        m_fTable_copy[j].firf = m_fTable[j].firf;
        m_fTable_copy[j].secf = m_fTable[j].secf;
        m_dTable_copy[j].fird = m_dTable[j].fird;
        m_dTable_copy[j].secd = m_dTable[j].secd;
    }

    size_t naddset = vecAddSets.size();
    for (size_t k = 0; k < naddset; ++k)
    {
        AddFacility(vecAddSets[k], &dC);

        for (int i = 0; i < m_nNodes; ++i)
        {
            m_f[i] = 0;
            /*if (m_cursols[i] == FACILITY_NODE)
            {
                m_f[i] = 0;
            }*/
        }

        for (int i = 0; i < m_nNodes; ++i)
        {
            if (m_cursols[i] == USER_NODE)
            {
                double minv = m_distanceGraph[i][vecAddSets[k]];
                if (minv > m_dTable[i].secd)
                {
                    minv = m_dTable[i].secd;
                }

                if (minv > m_f[m_fTable[i].firf])
                {
                    m_f[m_fTable[i].firf] = minv;
                }
            }
        }

        SwapPair sp;
        for (int i = 0; i < m_nNodes; ++i)
        {
            if (i == vecAddSets[k])
            {
                continue;
            }

            if (m_cursols[i] == FACILITY_NODE)
            {
                if (m_f[i] == dmax)
                {
                    sp.facility = i;
                    sp.vertex = vecAddSets[k];
                    vecsp->push_back(sp);
                }
                else if (m_f[i] < dmax)
                {
                    vecsp->resize(0);
                    sp.facility = i;
                    sp.vertex = vecAddSets[k];
                    vecsp->push_back(sp);
                    dmax = m_f[i];
                }
            }
        }

        RemoveFacility(vecAddSets[k], &dC);

        // 恢复现场
        //memcpy(m_fTable, m_fTable_copy, sizeof(FTable)*m_nNodes);
        //memcpy(m_dTable, m_dTable_copy, sizeof(DTable)*m_nNodes);
        for (int j = 0; j < m_nNodes; ++j)
        {
            m_fTable[j].firf = m_fTable_copy[j].firf;
            m_fTable[j].secf = m_fTable_copy[j].secf;
            m_dTable[j].fird = m_dTable_copy[j].fird;
            m_dTable[j].secd = m_dTable_copy[j].secd;
        }
    }

    m_Sc = dC; // 最大的服务边

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

        /*cout << "distance matrix:" << endl;
        PrintDistanceMatrix();
        cout << "sorted distance matrix:" << endl;
        PrintMatrix(m_disSortedGraph, m_nNodes, m_nNodes);
        cout << "number distance matrix:" << endl;
        PrintMatrix(m_disSequenceGraph, m_nNodes, m_nNodes);

        PrintMatrix(m_distanceGraph, m_nNodes, m_nNodes, true);
        PrintMatrix(m_disSortedGraph, m_nNodes, m_nNodes, true);
        PrintMatrix(m_disSequenceGraph, m_nNodes, m_nNodes, true);*/
        //PrintMatrix(m_distanceGraph, m_nNodes, m_nNodes, true);
    } while (false);

    return st;
}

void PCenter::PrintResultInfo(bool onlyfacilit)
{
    cout << "best values: " << ToStr(m_bestobjval) << endl;
    for (int i = 0; i < m_nNodes; ++i)
    {
        if (onlyfacilit)
        {
            if (m_cursols[i] == FACILITY_NODE)
            {
                cout << i+1 << " ";
            }
        }
        else 
        {
            cout << i+1 << "-" << m_bestsols[i] << " ";
        }
    }
    cout << endl;
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

    m_fTable_copy = new FTable[m_nNodes];
    m_dTable_copy = new DTable[m_nNodes];

    m_bestsols = new int[m_nNodes];
    m_cursols = new int[m_nNodes];

    m_f = new double[m_nNodes];
    if (m_fTable == hoNull || m_dTable == hoNull || m_fTable_copy == hoNull || m_dTable_copy == hoNull ||
        m_bestsols == hoNull || m_cursols == hoNull || m_f == hoNull)
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

    delete[] m_dTable_copy;
    m_dTable_copy = hoNull;
    delete[] m_fTable_copy;
    m_fTable_copy = hoNull;

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

        m_f[i] = 0.0;
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