
#include "graph_coloring.h"
#include "graph.h"
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

#define HO_DEBUG 0

HO_NAMESPACE_BEGIN(gcp)

GraphColoring::GraphColoring()
: m_Graph(hoNull)
, numColors(-1)
, m_conflicts(-1)
, m_enditer(0)
, m_maxiter(0)
, m_iternum(0)
, m_lstime(0.0)
, m_nbestmove(0)
, m_ntabubestmove(0)
{
    // nothing to do
}

hoStatus GraphColoring::TS()
{
    int** tabulist = new int*[m_Graph->numNodes];
    int** coloradj = new int*[m_Graph->numNodes];
    int* bestsol = new int[m_Graph->numNodes];
    int bestnc = 0;
    if (tabulist == hoNull || coloradj == hoNull || bestsol == hoNull)
    {
        return hoMemoryOut;
    }

    for (int i = 0; i < m_Graph->numNodes; ++i)
    {
        tabulist[i] = new int[numColors];
        coloradj[i] = new int[numColors];
        if (tabulist[i] == hoNull || coloradj[i] == hoNull)
        {
            return hoMemoryOut;
        }

        for (int j = 0; j < numColors; ++j)
        {
            tabulist[i][j] = 0;
            coloradj[i][j] = 0;
        }

        bestsol[i] = -1;
    }

    m_bestmoves = new OneMove[2000];
    m_tabubestmoves = new OneMove[2000];

    OneMove* move = new OneMove();

    time_t starttime, stoptime;
    starttime = clock();

    buildColorAdj(coloradj);
    //printConflictingNodesList(coloradj);
    //printAdjacency(coloradj);

    int niter = 0;
    int fixlong = 10; // 52colors--[28];  50colors--[21-0.7]
    float proplong = 0.6f;

    cout << "fixlong: " << fixlong << "\n";
    cout << "proplong: " << proplong << "\n";

    m_maxiter = 8000000;
    m_enditer = m_maxiter;

    m_conflicts = nodesConficting(coloradj);
    bestnc = m_conflicts;
    savebestsol(bestsol);

    int oldC, tabuT;

    srand((unsigned)time(NULL));

    while (m_conflicts > 0 /*&& niter<m_enditer*/)
    {
        niter++;

        move = findBestOneMove(coloradj, tabulist, move, niter, m_conflicts, bestnc);
        if (move->id == -1)
        {
            continue;
        }

        Vertex* v = m_Graph->GetVertexFromList(move->id);
        oldC = v->color;
        v->color = move->bestnew;

        updateAdjacency(coloradj, move);
        m_conflicts = nodesConficting(coloradj);
        tabuT = setTabu(coloradj, tabulist, move, fixlong, proplong, niter);

#if HO_DEBUG
        cout << "move node " << move->id << " color " << move->color << " to color " << move->bestnew << "\n";
        cout << "iter " << niter << " tenure " << tabuT << "\n";
        cout << "conflicts " << m_conflicts << "\n";
#endif

        if (m_conflicts < bestnc)
        {
            bestnc = m_conflicts;
            m_enditer = niter + m_maxiter;
            savebestsol(bestsol);
        }

        if (m_conflicts == 1)
        {
            exit(-4);
        }
    }

    stoptime = clock();
    m_lstime = double(stoptime - starttime) / CLOCKS_PER_SEC;

    m_iternum = niter;

    for (int i = 0; i < m_Graph->numNodes; ++i)
    {
        delete[] tabulist[i];
        delete[] coloradj[i];
    }

    delete tabulist;
    delete coloradj;
    delete[] bestsol;

    return hoOK;
}

int GraphColoring::setTabu(int **adjColors, int **tabuList, OneMove *move, int fixLong, float propLong, int nIt)
{
    int propval = (int)(propLong * m_conflicts);
    int tabuval = nIt + rand() % fixLong + propval;

    tabuList[move->id - 1][move->color] = tabuval;

    return tabuval;
}

void GraphColoring::updateAdjacency(int **adjColors, OneMove *move)
{
    Vertex* v = m_Graph->GetVertexFromList(move->id);
    Adjacent* adj = v->adja->next;

    while (adj != v->adja)
    {
        adjColors[(adj->vertex->id) - 1][move->color]--;
        adjColors[(adj->vertex->id) - 1][move->bestnew]++;

        adj = adj->next;
    }
}

hoStatus GraphColoring::VNS()
{
    return hoOK;
}

hoStatus GraphColoring::SA()
{
    return hoOK;
}

hoStatus GraphColoring::LoadInsFile(const std::string& insfile, int numc)
{
    m_Graph = new Graph();
    if (m_Graph == hoNull)
    {
        return hoMemoryOut;
    }

    if (numc > 0)
    {
        numColors = numc;
    }

    hoStatus st = hoOK;
    st = m_Graph->InitGraphFromFile(insfile);

    cout << "colors: " << numc << "\n";

    return st;
}

hoStatus GraphColoring::RandomInitColor()
{
    Vertex* node = m_Graph->nodesList;
    Vertex* cur = node->next;

    if (numColors == -1)
    {
        return hoError;
    }

    cout << "Random coloring the nodes graph\n";

    while (cur != node)
    {
        cur->color = rand() % numColors;
        cur = cur->next;
    }

    return hoOK;
}

hoStatus GraphColoring::PrintTabuSolInfo()
{
    if (m_conflicts == -1)
    {
        cout << "No application run!\n";
        return hoOK;
    }

    cout << "\n";

    if (m_conflicts == 0)
    {
        cout << "Yeah, Find a " << numColors << " colors solution!\n";
    }
    else
    {
        cout << "Can't find a " << numColors << " colors solution!\n";
    }

    cout << "- - - - - - - - - - - solution - - - - - - - - - - -\n";
    cout << "iteration numbers: " << m_iternum << "\n";
    cout << "max iter: " << m_maxiter << "\n";
    cout << "end iter: " << m_enditer << "\n";
    cout << "conflict: " << m_conflicts << "\n";
    cout << "search time: " << m_lstime << "s\n";

    cout << "- - - - - - - - - - solution end - - - - - - - - - -\n";

    return hoOK;
}

void GraphColoring::buildColorAdj(int** coloradj)
{
    for (int i = 0; i < m_Graph->numNodes; ++i)
    {
        for (int j = 0; j < numColors; ++j)
        {
            coloradj[i][j] = 0;
        }
    }

    Vertex* cur = m_Graph->nodesList->next;
    while (cur != m_Graph->nodesList)
    {
        Adjacent* curadj = cur->adja->next;

        while (curadj != cur->adja)
        {
            coloradj[cur->id - 1][curadj->vertex->color]++;
            curadj = curadj->next;
        }

        cur = cur->next;
    }
}

void GraphColoring::savebestsol(int* sols)
{
    Vertex* cur = m_Graph->nodesList->next;

    while (cur != m_Graph->nodesList)
    {
        sols[cur->id - 1] = cur->color;
        cur = cur->next;
    }
}

int GraphColoring::nodesConficting(int** coloradj)
{
    int nc = 0;

    Vertex* curv = m_Graph->nodesList->next;

    while (curv != m_Graph->nodesList)
    {
        if (coloradj[curv->id - 1][curv->color] > 0)
        {
            nc++;
        }

        curv = curv->next;
    }

    return nc;
}

OneMove* GraphColoring::findBestOneMove(int** coloradj, int** tabulist, OneMove* move, int niter, int nc, int bestnc)
{
    int i, profit;
    int best = 1000;
    int tabubest = 1000;

    OneMove bestmove;
    bestmove.id = 1;
    bestmove.color = 0;
    bestmove.bestnew = 1;

    int bestcount = 0;

    bool choose = false;
    bool tabuchoose = false;

    Vertex* cur = m_Graph->nodesList->next;
    while (cur != m_Graph->nodesList)
    {
        if (coloradj[cur->id - 1][cur->color] > 0)
        {
            for (i = 0; i < numColors; ++i)
            {
                if (i != cur->color)
                {
                    profit = moveProfit(coloradj, cur, i);

                    if (!isTabu(coloradj, tabulist, cur, i, niter, nc, bestnc))
                    {
                        if (profit < best)
                        {
                            choose = true;
                            best = profit;
                            bestmove.id = cur->id;
                            bestmove.color = cur->color;
                            bestmove.bestnew = i;
                            bestcount = 1;

                            m_nbestmove = 1;
                            m_bestmoves[0].id = cur->id;
                            m_bestmoves[0].color = cur->color;
                            m_bestmoves[0].bestnew = i;
                        }
                        else if (profit == best)
                        {
                            m_bestmoves[m_nbestmove].id = cur->id;
                            m_bestmoves[m_nbestmove].color = cur->color;
                            m_bestmoves[m_nbestmove].bestnew = i;
                            m_nbestmove++;

                            if (m_nbestmove + 1 > 2000)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (profit < tabubest)
                        {
                            tabuchoose = true;
                            tabubest = profit;
                            m_tabubestmoves[0].id = cur->id;
                            m_tabubestmoves[0].color = cur->color;
                            m_tabubestmoves[0].bestnew = i;
                            m_ntabubestmove = 1;
                        }
                        else if (profit == tabubest)
                        {
                            m_tabubestmoves[m_ntabubestmove].id = cur->id;
                            m_tabubestmoves[m_ntabubestmove].color = cur->color;
                            m_tabubestmoves[m_ntabubestmove].bestnew = i;
                            m_ntabubestmove++;

                            if (m_ntabubestmove + 1 > 2000)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }

        cur = cur->next;
    }

    bool usetabu = false;
    if (tabuchoose)
    {
        if (choose)
        {
            profit = coloradj[m_tabubestmoves[0].id - 1][m_tabubestmoves[0].bestnew] -
                coloradj[m_tabubestmoves[0].id - 1][m_tabubestmoves[0].color];
            if (tabubest < best && (nc + profit)<bestnc)
            {
                usetabu = true;
                int k = (m_ntabubestmove > 1) ? (rand() % m_ntabubestmove) : 0;
                move->id = m_tabubestmoves[k].id;
                move->color = m_tabubestmoves[k].color;
                move->bestnew = m_tabubestmoves[k].bestnew;

                tabulist[m_tabubestmoves[k].id - 1][m_tabubestmoves[k].bestnew] = 0;

                m_nbestmove = 0;
                m_ntabubestmove = 0;

                return move;
            }
        }
        else
        {
            // nothing to do
        }
    }

    if (choose)
    {
        move->id = bestmove.id;
        move->color = bestmove.color;
        move->bestnew = bestmove.bestnew;
    }
    else
    {
        move->id = -1;
        move->color = bestmove.color;
        move->bestnew = bestmove.bestnew;

        m_nbestmove = 0;
        m_ntabubestmove = 0;

        return move;
    }

    if (m_nbestmove > 1)
    {
        int k = rand() % m_nbestmove;
        move->id = m_bestmoves[k].id;
        move->color = m_bestmoves[k].color;
        move->bestnew = m_bestmoves[k].bestnew;
    }

    m_nbestmove = 0;
    m_ntabubestmove = 0;

    return move;
}

int GraphColoring::moveProfit(int** coloradj, Vertex* cur, int newcolor)
{
    return coloradj[cur->id - 1][newcolor] - coloradj[cur->id - 1][cur->color];
}

bool GraphColoring::isTabu(int** coloradj, int** tabulist, Vertex* cur, int newcolor, int nit, int nc, int bestnc)
{
    if (tabulist[cur->id - 1][newcolor] > nit)
    {
        return true;
    }
    else
    {
        return false;
    }

    // 	if (tabulist[cur->id - 1][newcolor] != 0)
    // 	{
    // 		if (nit > tabulist[cur->id - 1][newcolor])
    // 		{
    // 			return false;
    // 		}
    // 		else
    // 		{
    // 			/*int profit = moveProfit(coloradj, cur, newcolor);
    // 
    // 			if (profit+nc < bestnc)
    // 			{
    // 				return false;
    // 			} 
    // 			else
    // 			{
    // 				return true;
    // 			}*/
    // 
    // 			return true;
    // 		}
    // 	} 
    // 	else
    // 	{
    // 		return false;
    // 	}
}

void GraphColoring::printConflictingNodesList(int **adjColors)
{
    Vertex* v = m_Graph->nodesList->next;

    while (v != m_Graph->nodesList)
    {
        if (adjColors[v->id - 1][v->color] > 0)
        {
            printf("%d(%d)\n", v->id, v->color);
        }
        v = v->next;
    }
}

void GraphColoring::printAdjacency(int **adjColors)
{
    Vertex *n;
    int i, j;

    for (i = 0; i < m_Graph->numNodes; i++)
    {
        n = m_Graph->GetVertexFromList(i + 1);
        printf("%d(%d,c:%d): ", n->id, n->color, adjColors[i][n->color]);
        for (j = 0; j < numColors; j++)
        {
            printf("%d ", adjColors[i][j]);
        }
        printf("\n");
    }
}

GraphColoring::~GraphColoring()
{
    delete m_Graph;
    delete[] m_bestmoves;
    delete[] m_tabubestmoves;
    m_Graph = hoNull;
}

HO_NAMESPACE_END