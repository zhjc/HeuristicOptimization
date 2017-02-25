
#include "p_center.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cassert>
#include <climits>
#include <ctime>
#include <vector>

#include "common/utility.h"

#include "population.h"

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
, m_runningMode(hoInitialize)
, m_bestsols(hoNull)
, m_cursols(hoNull)
, m_Sc(0.0)
{
    // nothing to do
}

hoStatus PCenter::RunSinglePhase(int run_type)
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

hoStatus PCenter::RunPopulationPhase(int run_type)
{
    vector<Population*> vecpops;

    int iter = 0;

    while (iter < 50)
    {
        for (size_t i = 0; i < vecpops.size(); ++i)
        {
            Population* pops = vecpops[i];

            // TODO
        }
    }

    return hoOK;
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

        // nusercenter表示最长服务边上的 用户结点
        int user_of_longest_service_edge = vecmaxdis[rand() % vecmaxdis.size()];

        LogInfo("selected center node " + ToStr(user_of_longest_service_edge) 
            + " with longest distance " + ToStr(dmaxdis));

        int nfacility = 0;
        vector<int> vecnewuser;
        for (int i = 0; i < m_nNodes; ++i)
        {
            if (m_disSortedGraph[user_of_longest_service_edge][i] == m_fTable[user_of_longest_service_edge].firf)
            {
                // 遍历到为当前用户结点提供服务的设施点,直接退出,因为遍历的点已经排序
                break;
            }
            else
            {
                if (m_cursols[m_disSortedGraph[user_of_longest_service_edge][i]] == FACILITY_NODE)
                {
                    // 最长边为半径的圆里已有服务结点时跳过
                    // TODO:是否会造出两个设施太接近???
                    continue;
                }

                vecnewuser.push_back(m_disSortedGraph[user_of_longest_service_edge][i]);
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

hoStatus PCenter::GeneticAlgorithm()
{
    return hoOK;
}

hoStatus PCenter::Mutation1(int* sols)
{
    // 在[p/2,p]中随机选取一个数q,在Pi的设施中随机选取q个设施，
    // 再在V-Pi集合中选取p-q个用户节点，构成新的解S
    int q = rand() * (m_nFacility - m_nFacility / 2) + m_nFacility / 2;

    return hoOK;
}

hoStatus PCenter::Mutation2(int* sols)
{
    // 基于设施Facility不能太接近的原则
    // 对于当前解，找出最近的一对设施点，并将这对设施点删除
    // 再调用LocalSearch程序来添加设施点

    return hoOK;
}

hoStatus PCenter::Crossover1(int* sols1, int* sols2)
{
    // 随机从两个父解中选取p个设施，构造新解

    return hoOK;
}

hoStatus PCenter::Crossover2(int* sols1, int* sols2)
{
    // 首先随机选取两个用户节点，随机选取一个[0.1,0.9]之间的数字q，di1作为设施i到第一个用户的距离，
    // di2作为设施i到第二个用户的距离

    // 第一个子解由两部分组成:第一个父解中选择所有di1/di2<=q的所有设施i,以及第二个父解中所有dj1/dj2>q的所有设施j
    // 第二个子解由两部分组成:第一个父解中选择所有di1/di2>q的所有设施i,以及第二个父解中所有dj1/dj2<=q的所有设施j

    // 如果子解中的设施数超过p,则随机删除以降到p;如果设施数不足p,则由LocalSearch程序补充

    double q = rand() * 0.8 + 0.1;



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
    m_runningMode = hoRunning;
    
    //PrintDistanceToFile(m_nNodes, m_distanceGraph);

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
        double longest_service_edge = 0.0;
        vector<int> vec_user_nodes_with_long_edge;
        for (int i = 0; i < m_nNodes; ++i)
        {
            if (longest_service_edge < m_dTable[i].fird)
            {
                longest_service_edge = m_dTable[i].fird;
                vec_user_nodes_with_long_edge.resize(0);
                vec_user_nodes_with_long_edge.push_back(i);
            }
            else if (longest_service_edge == m_dTable[i].fird)
            {
                vec_user_nodes_with_long_edge.push_back(i);
            }
        }

        assert(vec_user_nodes_with_long_edge.size()>0);

        int select_center_user_node = vec_user_nodes_with_long_edge[rand() % vec_user_nodes_with_long_edge.size()];

        LogInfo("selected node of maximum length " + ToStr(select_center_user_node) + "");

        // try nodes in circle (nSelect, dMaxDistance), choose the best one
        vector<SwapPair> vecsps;
        FindPair(select_center_user_node, longest_service_edge, &vecsps, iter);

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

        LogInfo("current iter:" + ToStr(iter));
        LogInfo("selected node " + ToStr(sp.vertex) + " for swap " + ToStr(sp.facility));

        // do swap operation and set TabuList
        double dSc = 0.0;
        AddFacility(sp.vertex, &dSc);
        RemoveFacility(sp.facility, &dSc);

        // 设置禁忌, 此处行为用户节点, 列为设施节点
        setTabu(sp.facility, sp.vertex, iter);

        /*for (int i = 0; i < m_nNodes; ++i)
        {
            if (m_dTable[i].fird > m_curobjval)
            {
                m_curobjval = m_dTable[i].fird;
            }
        }*/

        // update best solution
        if (dSc < m_bestobjval)
        {
            m_bestobjval = dSc;
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

hoStatus PCenter::FindPair(int center_user_node, double longest_service_edge, vector<SwapPair>* vecsp, long iter)
{
    hoStatus st = hoOK;

    double dC = 0.0;

    // 需要计算m_curobjval, m_cursols

    // NOTE: 判断是否在禁忌之中

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

    int selectnode = center_user_node;
    double dmax = DBL_MAX; // longest_service_edge;

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

    vector<SwapPair> vecChoose;
    vector<SwapPair> vecChooseTabu;
    bool bChoose = false;
    bool bChooseTabu = false;
    double costval = DBL_MAX;
    double costvaltabu = DBL_MAX;

    // 存储现场
    //memcpy(m_fTable_copy, m_fTable, sizeof(FTable)*m_nNodes);
    //memcpy(m_dTable_copy, m_dTable, sizeof(DTable)*m_nNodes);
    /*for (int j = 0; j < m_nNodes; ++j)
    {
        m_fTable_copy[j].firf = m_fTable[j].firf;
        m_fTable_copy[j].secf = m_fTable[j].secf;
        m_dTable_copy[j].fird = m_dTable[j].fird;
        m_dTable_copy[j].secd = m_dTable[j].secd;
    }*/

    /*vector<int> vec_facilities_can_delete;
    for (int i = 0; i < m_nNodes; ++i)
    {
        if (m_cursols[i] == FACILITY_NODE)
        {
            vec_facilities_can_delete.push_back(i);
        }
    }
    assert(vec_facilities_can_delete.size() == m_nFacility);*/

    size_t naddset = vecAddSets.size();
    for (size_t k = 0; k < naddset; ++k)
    {
        AddFacility(vecAddSets[k], &dC); // 执行完该操作后有p+1个设施结点,dC为最长的服务边

        for (int i = 0; i < m_nNodes; ++i)
        {
            m_f[i] = 0;
            /*if (m_cursols[i] == FACILITY_NODE){
                m_f[i] = 0;
            }*/
        }

        for (int i = 0; i < m_nNodes; ++i)
        {
            if (i == vecAddSets[k])
            {
                continue;
            }

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
                if (m_f[i] == 0)
                {
                    // show only the facility, has no vertexs
                    continue;
                }

                if (m_f[i] <= dmax)
                {
                    if (isTabu(vecAddSets[k], i, iter))
                    {
                        if (m_f[i] == dmax)
                        {
                            sp.facility = i;
                            sp.vertex = vecAddSets[k];
                            vecChooseTabu.push_back(sp);
                            bChooseTabu = true;
                        }
                        else if (m_f[i] < dmax)
                        {
                            vecChooseTabu.resize(0);
                            sp.facility = i;
                            sp.vertex = vecAddSets[k];
                            vecChooseTabu.push_back(sp);
                            dmax = m_f[i];
                            costvaltabu = m_f[i];
                            bChooseTabu = true;
                        }
                    }
                    else
                    {
                        if (m_f[i] == dmax)
                        {
                            sp.facility = i;
                            sp.vertex = vecAddSets[k];
                            vecChoose.push_back(sp);
                            bChoose = true;
                        }
                        else if (m_f[i] < dmax)
                        {
                            vecChoose.resize(0);
                            sp.facility = i;
                            sp.vertex = vecAddSets[k];
                            vecChoose.push_back(sp);
                            dmax = m_f[i];
                            costval = m_f[i];
                            bChoose = true;
                        }
                    }
                }
            }
        }

        RemoveFacility(vecAddSets[k], &dC);

        // 恢复现场
        //memcpy(m_fTable, m_fTable_copy, sizeof(FTable)*m_nNodes);
        //memcpy(m_dTable, m_dTable_copy, sizeof(DTable)*m_nNodes);
        /*for (int j = 0; j < m_nNodes; ++j)
        {
            m_fTable[j].firf = m_fTable_copy[j].firf;
            m_fTable[j].secf = m_fTable_copy[j].secf;
            m_dTable[j].fird = m_dTable_copy[j].fird;
            m_dTable[j].secd = m_dTable_copy[j].secd;
        }*/
    }

    m_Sc = dC; // 最大的服务边

    if (bChooseTabu)
    {
        if (costvaltabu>0 && costvaltabu < m_bestobjval && costvaltabu < costval)
        {
            vecsp->insert(vecsp->begin(), vecChooseTabu.begin(), vecChooseTabu.end());
            return st;
        }

        // 执行到此且bChooseTabu是false时,表示已被禁忌,需继续循环等待完成禁忌
    }

    if (bChoose)
    {
        vecsp->insert(vecsp->begin(), vecChoose.begin(), vecChoose.end());
    }

    return st;
}

hoStatus PCenter::FindPair_new(int center_user_node, double longest_service_edge, vector<SwapPair>* vecsp, long iter)
{
    hoStatus st = hoOK;

    double dC = 0.0;

    int selectnode = center_user_node;
    double dmax = longest_service_edge;

    int serverofselectnode = m_fTable[selectnode].firf;

    // 找到selectnode为圆心，其到服务点的距离为半径的圆内的点，随机选择一个作为将要添加的结点，即选择Nwk
    vector<int> vecAddSets;
    for (int i = 0; i < m_nNodes; ++i)
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

    vector<int> vec_facilities_can_delete;
    for (int i = 0; i < m_nNodes; ++i)
    {
        if (m_cursols[i] == FACILITY_NODE)
        {
            vec_facilities_can_delete.push_back(i);
        }
    }

    assert(vec_facilities_can_delete.size() == m_nFacility);

    double costval = DBL_MAX;
    double costvaltabu = DBL_MAX;
    vector<SwapPair> vecselectsp;
    vector<SwapPair> vecselectsptabu;
    bool bchoose = false;
    bool bchoosetabu = false;

    int debug_count = 0;

    size_t naddset = vecAddSets.size();
    for (size_t k = 0; k < naddset; ++k)
    {
        AddFacility(vecAddSets[k], &dC); // 执行完该操作后有p+1个设施结点

        for (size_t index = 0; index < vec_facilities_can_delete.size(); ++index)
        {
            RemoveFacility(vec_facilities_can_delete[index], &dC);

            if (isTabu(vecAddSets[k], vec_facilities_can_delete[index], iter))
            {
                if (dC <= costvaltabu)
                {
                    bchoosetabu = true;
                    if (dC < costvaltabu)
                    {
                        costvaltabu = dC;
                        vecselectsptabu.resize(0);
                    }
                    vecselectsptabu.push_back(SwapPair(vec_facilities_can_delete[index], vecAddSets[k]));
                }
                
            }
            else
            {
                if (dC <= costval)
                {
                    bchoose = true;
                    if (dC < costval)
                    {
                        costval = dC;
                        vecselectsp.resize(0);
                    }
                    vecselectsp.push_back(SwapPair(vec_facilities_can_delete[index], vecAddSets[k]));
                }
            }

            AddFacility(vec_facilities_can_delete[index], &dC);

            debug_count++;
        }

        RemoveFacility(vecAddSets[k], &dC);
    }

    m_Sc = dC; // 最大的服务边

    if (bchoosetabu)
    {
        if (costvaltabu < m_bestobjval && costvaltabu<costval)
        {
            vecsp->insert(vecsp->begin(), vecselectsptabu.begin(), vecselectsptabu.end());
            return st;
        }
    }
    
    if (bchoose)
    {
        vecsp->insert(vecsp->begin(), vecselectsp.begin(), vecselectsp.end());
    }

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
    if (hoInitialize != m_runningMode)
    {
        cout << "best values: " << ToStr(m_bestobjval) << endl;
    }
    
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

bool PCenter::isTabu(int facility, int user, int iter)
{
    if (m_TabuList[facility][user] > iter)
    {
        return true;
    }

    return false;
}

void PCenter::setTabu(int facility, int user, int iter)
{
    m_TabuList[facility][user] = iter + rand() % 10 + m_nNodes / 5;
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