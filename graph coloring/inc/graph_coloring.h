
#ifndef GRAPH_COLORING_H
#define GRAPH_COLORING_H

#include "config.h"
#include <string>

HO_NAMESPACE_BEGIN(gcp)

struct Vertex;
class Graph;

struct OneMove
{
	int id;
	int color;
	int bestnew;
};

class GraphColoring{
public:
	GraphColoring();

	~GraphColoring();

public:
	hoStatus TS();
	hoStatus VNS();
	hoStatus SA();

	hoStatus LoadInsFile(const std::string& insfile, int numc);

	hoStatus RandomInitColor();

	hoStatus PrintTabuSolInfo();

private:
	void buildColorAdj(int** coloradj);
	void savebestsol(int* sols);
	int nodesConficting(int** coloradj);
	OneMove* findBestOneMove(int** coloradj, int** tabulist, OneMove* move, int niter, int nc, int bestnc);
	int moveProfit(int** coloradj, Vertex* cur, int newcolor);
	bool isTabu(int** coloradj, int** tabulist, Vertex* cur, int newcolor, int nit, int nc, int bestnc);

	int setTabu(int **adjColors, int **tabuList, OneMove *move, int fixLong, float propLong, int nIt);
	void updateAdjacency(int **adjColors, OneMove *move);

	void printConflictingNodesList(int **adjColors);
	void printAdjacency(int **adjColors);
private:
	Graph* m_Graph;
	int numColors;

	int m_iternum;
	int m_maxiter;
	int m_enditer;
	int m_conflicts;
	double m_lstime;

	OneMove* m_bestmoves;
	int m_nbestmove;
	OneMove* m_tabubestmoves;
	int m_ntabubestmove;
};

HO_NAMESPACE_END

#endif