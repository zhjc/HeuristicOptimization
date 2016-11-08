
#include "graph_coloring.h"
#include "graph.h"
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

#define HO_DEBUG 1

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
		if (tabulist[i]==hoNull || coloradj[i]==hoNull)
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

	m_bestmoves = new OneMove[5000];

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

	while (m_conflicts>0 /*&& niter<m_enditer*/)
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

#if HO_DEBUG
		cout << "move node " << move->id << " color " << move->color << " to new color " << move->bestnew << "\n";
		//printAdjacency(coloradj);
#endif
		tabuT = setTabu(coloradj, tabulist, move, fixlong, proplong, niter);
#if HO_DEBUG
		cout << "iter " << niter << " tabu tenure " << tabuT << "\n";
#endif
		updateAdjacency(coloradj, move);
		m_conflicts = nodesConficting(coloradj);
#if HO_DEBUG
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
	int propValue = (int)floor(propLong*(nodesConficting(adjColors)));

	tabuList[move->id - 1][move->color] = nIt + rand()%fixLong + propValue;

	return tabuList[move->id - 1][move->color];
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

	if (numColors==-1)
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
		if (coloradj[curv->id-1][curv->color] > 0)
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

	OneMove bestmove;
	bestmove.id = 1;
	bestmove.color = 0;
	bestmove.bestnew = 1;

	int bestcount = 0;

	bool choose = false;

	Vertex* cur = m_Graph->nodesList->next;
	while (cur != m_Graph->nodesList)
	{
		if (coloradj[cur->id-1][cur->color] > 0)
		{
			for (i = 0; i < numColors; ++i)
			{
				if (i != cur->color)
				{
					if (!isTabu(coloradj, tabulist, cur, i, niter, nc, bestnc))
					{
						profit = moveProfit(coloradj, cur, i);
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

							if (m_nbestmove+1 > 5000)
							{
								break;
							}
						}

						/*else if (profit == best)
						{
							bestcount++;
							if (rand()%bestcount == 0)
							{
								bestmove.id = cur->id;
								bestmove.color = cur->color;
								bestmove.bestnew = i;
							}
						}*/
					}
				}
			}
		}

		cur = cur->next;
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
	}

	if (m_nbestmove > 1)
	{
		int k = rand() % m_nbestmove;
		move->id = m_bestmoves[k].id;
		move->color = m_bestmoves[k].color;
		move->bestnew = m_bestmoves[k].bestnew;
	}

	m_nbestmove = 0;

	return move;
}

int GraphColoring::moveProfit(int** coloradj, Vertex* cur, int newcolor)
{
	int profit = 0;

	profit = -coloradj[cur->id - 1][cur->color];
	profit += coloradj[cur->id - 1][newcolor];

	return profit;
}

bool GraphColoring::isTabu(int** coloradj, int** tabulist, Vertex* cur, int newcolor, int nit, int nc, int bestnc)
{
	if (tabulist[cur->id - 1][newcolor] != 0)
	{
		if (nit > tabulist[cur->id - 1][newcolor])
		{
			return false;
		}
		else
		{
			int profit = moveProfit(coloradj, cur, newcolor);

			if (profit+nc < bestnc)
			{
				return false;
			} 
			else
			{
				return true;
			}

			return true;
		}
	} 
	else
	{
		return false;
	}
}

void GraphColoring::printConflictingNodesList(int **adjColors)
{
	Vertex* v = m_Graph->nodesList->next;

	while (v != m_Graph->nodesList)
	{
		if (adjColors[v->id-1][v->color]>0)
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
	m_Graph = hoNull;
}

HO_NAMESPACE_END