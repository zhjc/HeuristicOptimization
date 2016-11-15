#include "..\src\bu\p_center.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <limits.h>

//extern "C"{

int global_i = 0;
int ** disGraph = NULL;

int debug_info = 0;

typedef struct SwapPair{
	int facility;
	int vertex;
	struct SwapPair* next;
} SwapPair;

typedef struct FTable{
	int fir_facility;
	int sec_facility;
}FTable;

typedef struct DTable{
	int fir_distance;
	int sec_distance;
}DTable;

typedef struct pCenter{
	int ** distanceGraph; // 距离图
	int ** sortedGraph;   // 按距离排序图
	int ** disSeqGraph;   // 按距离排序图元素序号

	DTable * dTable;    // D表
	FTable * fTable;    // F表

	int * faclity;      // 当前服务点集合
	int * mf;
	int curNumOfF;      // 当前服务点个数

	int * listTabu;     // 禁忌表

	int numOfNodes;     // 结点个数
	int numOfFacility;  // 服务点个数
	int numOfEdges;     // 边数

	int objectFunction; // 目标函数

	pcInputType inputType;
	pcSearchType searchType;

	SwapPair* swapPair;
	SwapPair* tabuList;
	int tabuLen;
	int tabuCur;

	int * bestSol;
	int bestObj;
} pCenter;

void printMatrix(int** m, int nr, int nc)
{
	int i, j;
	printf("matrix:\n");
	for (i = 0; i < nr;++i)
	{
		for (j = 0; j < nc;++j)
		{
			printf("%d ", m[i][j]);
		}
		printf("\n");
	}
}

int compOfqsort(const void *a, const void *b)
{
	return disGraph[global_i][*(int*)a] - disGraph[global_i][*(int*)b];
}

void _pcReadInputFile(void* pData, char* pFlie)
{
	pCenter* pWorkSpace = (pCenter*)pData;
	FILE* pInputFile = fopen(pFlie, "r");

	//pWorkSpace->distanceGraph

	fclose(pInputFile);
}

void _pcGetShortPathByPloyd(pCenter* pWorkSpace)
{
	int i, j, k;
	int numNode = pWorkSpace->numOfNodes;
	int **graphMatrix = pWorkSpace->distanceGraph;

	for (i = 0; i < numNode; i++)
	{
		for (j = 0; j < numNode; j++)
		{
			if (i == j)
			{
				continue;
			}
			for (k = 0; k < numNode; k++)
			{
				if (j == k)
				{
					continue;
				}

				if (graphMatrix[j][i] && graphMatrix[i][k])
				{
					if (graphMatrix[j][k] == 0)
					{
						graphMatrix[j][k] = graphMatrix[j][i] + graphMatrix[i][k];
					}
					else
					{
						if (graphMatrix[j][i] + graphMatrix[i][k] < graphMatrix[j][k])
						{
							graphMatrix[j][k] = graphMatrix[j][i] + graphMatrix[i][k];
						}
					}
				}
			}
		}
	}
}

void _pcAddFacility(void* pData, int addNode)
{
	pCenter* pWorkSpace = (pCenter*)pData;

	int f = 0;
	int i = 0;

	pWorkSpace->faclity[pWorkSpace->curNumOfF++] = addNode;

	for (i = 0; i < pWorkSpace->numOfNodes; ++i)
	{
		int distance = pWorkSpace->distanceGraph[i][addNode];
		if (distance < pWorkSpace->dTable[i].fir_distance)
		{
			pWorkSpace->dTable[i].sec_distance = pWorkSpace->dTable[i].fir_distance;
			pWorkSpace->fTable[i].sec_facility = pWorkSpace->fTable[i].fir_facility;
			pWorkSpace->dTable[i].fir_distance = distance;
			pWorkSpace->fTable[i].fir_facility = addNode;
		}
		else if (distance < pWorkSpace->dTable[i].sec_distance)
		{
			pWorkSpace->dTable[i].sec_distance = distance;
			pWorkSpace->fTable[i].sec_facility = addNode;
		}

		// 更新目标函数值
		if (pWorkSpace->dTable[i].fir_distance > f)
		{
			f = pWorkSpace->dTable[i].fir_distance;
		}
	}

	pWorkSpace->objectFunction = f;
}

int _pcRemoveFacility(void* pData, int* maxLenNum, int deleteNode)
{
	pCenter* pWorkSpace = (pCenter*)pData;
	int i = 0;
	int k = 0;
	int f = 0;

	// S = S-f
	for (i = 0; i < pWorkSpace->curNumOfF; ++i)
	{
		if (deleteNode == pWorkSpace->faclity[i])
		{
			int j = 0;
			for (j = i; j < pWorkSpace->curNumOfF - 1; ++j)
			{
				pWorkSpace->faclity[j] = pWorkSpace->faclity[j + 1];
			}
			pWorkSpace->curNumOfF--;
			break;
		}
	}

	for (i = 0; i < pWorkSpace->numOfNodes; i++)
	{
		if (deleteNode == pWorkSpace->fTable[i].fir_facility)
		{
			pWorkSpace->dTable[i].fir_distance = pWorkSpace->dTable[i].sec_distance;
			pWorkSpace->fTable[i].fir_facility = pWorkSpace->fTable[i].sec_facility;

			// Find_Next(v)
			{
				int secDistance = 0;
				int secFacility = 0;
				for (k = 0; k < pWorkSpace->curNumOfF; ++k)
				{
					int curFacility = pWorkSpace->faclity[k];
					if (secDistance < pWorkSpace->distanceGraph[i][curFacility] &&
						curFacility != pWorkSpace->fTable[i].fir_facility)
					{
						secDistance = pWorkSpace->distanceGraph[i][curFacility];
						secFacility = curFacility;
					}
				}

				pWorkSpace->dTable[i].sec_distance = secDistance;
				pWorkSpace->fTable[i].sec_facility = secFacility;
			}
		}
		else if (deleteNode == pWorkSpace->fTable[i].sec_facility)
		{
			// Find_Next(v)
			int secDistance = 0;
			int secFacility = 0;
			for (k = 0; k < pWorkSpace->curNumOfF; ++k)
			{
				int curFacility = pWorkSpace->faclity[k];
				if (secDistance < pWorkSpace->distanceGraph[i][curFacility] &&
					curFacility != pWorkSpace->fTable[i].fir_facility)
				{
					secDistance = pWorkSpace->distanceGraph[i][curFacility];
					secFacility = curFacility;
				}
			}

			pWorkSpace->dTable[i].sec_distance = secDistance;
			pWorkSpace->fTable[i].sec_facility = secFacility;
		}

		if (f < pWorkSpace->dTable[i].fir_distance)
		{
			f = pWorkSpace->dTable[i].fir_distance;
			*maxLenNum = 1;
		}
		else if (f == pWorkSpace->dTable[i].fir_distance)
		{
			(*maxLenNum)++;
		}
	}

	return f;
}

void _pcFindPair(void* pData, int w, int dij)
{
	pCenter* pWorkSpace = (pCenter*)pData;

	int C = dij; // C = max(dij)
	int maxLenNum = 0;

	int i = 0; // i=Nwk
	int k = 0;
	int n = 0;
	int flag = 0;
	int curNode = 0;

	// 找出max(dij)
	int maxij = 0;
	int tmp = 0;
	for (i = 0; i < pWorkSpace->numOfNodes; ++i)
	{
		tmp = pWorkSpace->distanceGraph[i][pWorkSpace->disSeqGraph[i][pWorkSpace->numOfNodes - 1]];
		if (maxij < tmp)
		{
			maxij = tmp;
		}
	}

	C = maxij;

	// 搜索邻域Nwk
	for (i = 0; i < pWorkSpace->disSeqGraph[w][pWorkSpace->fTable[w].fir_facility]; ++i)
	{
		curNode = pWorkSpace->sortedGraph[w][i];

		// for debug
		if (debug_info)
		{
			fprintf(stdout, "Before add facility\n");
			pcPrintF_D_Table(pWorkSpace, stdout);
		}
		// end for debug

		_pcAddFacility(pWorkSpace, curNode);

		// for debug
		if (debug_info)
		{
			fprintf(stdout, "After add facility\n");
			pcPrintF_D_Table(pWorkSpace, stdout);
		}
		// end for debug

		for (k = 0; k < pWorkSpace->numOfNodes; ++k)
		{
			pWorkSpace->mf[k] = 0;
		}

		// v = V-S
		for (k = 0; k < pWorkSpace->numOfNodes; k++)
		{
			for (n = 0; n < pWorkSpace->curNumOfF; ++n)
			{
				if (k == pWorkSpace->faclity[n])
				{
					flag = 1;
					break;
				}
			}

			if (flag == 1)
			{
				flag = 0;
				continue;
			}

			/*if (min(pWorkSpace->distanceGraph[curNode][k], pWorkSpace->dTable[k].sec_distance) >
				pWorkSpace->mf[pWorkSpace->fTable[k].fir_facility])
			{
				pWorkSpace->mf[pWorkSpace->fTable[k].fir_facility] =
					min(pWorkSpace->distanceGraph[curNode][k], pWorkSpace->dTable[k].sec_distance);
			}*/
		}

		for (k = 0; k < pWorkSpace->curNumOfF; ++k)
		{
			if (curNode == pWorkSpace->faclity[k])
			{
				continue;
			}

			if (pWorkSpace->mf[pWorkSpace->faclity[k]] == C)
			{
				SwapPair* sp = (SwapPair*)malloc(sizeof(SwapPair));
				memset(sp, 0, sizeof(SwapPair));
				sp->facility = pWorkSpace->faclity[k];
				sp->vertex = curNode;
				sp->next = NULL;

				if (pWorkSpace->swapPair == NULL)
				{
					pWorkSpace->swapPair = sp;
				}
				else
				{
					pWorkSpace->swapPair->next = sp;
				}
			}
			else if (pWorkSpace->mf[pWorkSpace->faclity[k]] < C)
			{
				SwapPair* sp = (SwapPair*)malloc(sizeof(SwapPair));
				memset(sp, 0, sizeof(SwapPair));
				sp->facility = pWorkSpace->faclity[k];
				sp->vertex = curNode;
				sp->next = NULL;
				pWorkSpace->swapPair = sp;
				C = pWorkSpace->mf[pWorkSpace->faclity[k]];
			}
		}

		_pcRemoveFacility(pWorkSpace, &maxLenNum, curNode);

		// for debug
		if (debug_info)
		{
			fprintf(stdout, "After remove facility\n");
			pcPrintF_D_Table(pWorkSpace, stdout);
		}
		// end for debug
	}
}

void _pcLS(void* pData)
{
	pCenter* pWorkSpace = (pCenter*)pData;

	int numOfLS = 0;
	int i = 0;
	int j = 0;
	int maxLength = 0;
	int maxLengthNo = 0;
	int maxLengthNum = 0;

	while (numOfLS < 20)
	{
		// 找最长边，如果多条则随机选择一条
		maxLength = 0;
		for (i = 0; i < pWorkSpace->numOfNodes; ++i)
		{
			if (maxLength < pWorkSpace->dTable[i].fir_distance)
			{
				maxLength = pWorkSpace->dTable[i].fir_distance;
				maxLengthNo = i;
				j = 1;
			}
			else if (maxLength == pWorkSpace->dTable[i].fir_distance)
			{
				j++;
				if (!(rand() % j))
				{
					maxLengthNo = i;
				}
			}
		}

		_pcFindPair(pWorkSpace, maxLengthNo, maxLength);

		if (pWorkSpace->swapPair)
		{
			int len = 0;
			int bSwap = 1;
			for (len = 0; len < pWorkSpace->tabuLen; ++len)
			{
				SwapPair* pCurSP = pWorkSpace->swapPair;
				while (pCurSP)
				{
					if (pCurSP->facility == pWorkSpace->tabuList[i].facility &&
						pCurSP->vertex == pWorkSpace->tabuList[i].vertex)
					{
						bSwap = 0;
						break;
					}

					pCurSP = pCurSP->next;
				}

				if (bSwap == 0)
				{
					break;
				}
			}

			if (bSwap)
			{
				// 交换
				int maxLenNum = 0;
				_pcAddFacility(pWorkSpace, pWorkSpace->swapPair->vertex);
				_pcRemoveFacility(pWorkSpace, &maxLenNum, pWorkSpace->swapPair->facility);

				// 加入禁忌，禁忌列表为循环队列
				pWorkSpace->tabuList[pWorkSpace->tabuCur].facility = pWorkSpace->swapPair->facility;
				pWorkSpace->tabuList[pWorkSpace->tabuCur].vertex = pWorkSpace->swapPair->vertex;
				pWorkSpace->tabuCur = (pWorkSpace->tabuCur + 1) % pWorkSpace->tabuLen;

				// 更新历史值
				if (pWorkSpace->objectFunction < pWorkSpace->bestObj)
				{
					int i = 0;
					for (i = 0; i < pWorkSpace->numOfFacility; ++i)
					{
						pWorkSpace->bestSol[i] = pWorkSpace->faclity[i];
					}
					pWorkSpace->bestObj = pWorkSpace->objectFunction;
				}
			}
			else
			{
				// 没有找到解
				break;
			}
		}
		else
		{
			// 到达终止状态
			break;
		}

		numOfLS++;

		if (debug_info || 1)
		{
			int i = 0;
			printf("seq:%d\n", numOfLS);
			for (; i < pWorkSpace->curNumOfF; ++i)
			{
				printf("%d ", pWorkSpace->faclity[i]);
			}
			printf("\n");
		}
	}
}

void* pcInitialize(char* pFilePath)
{
	pCenter* pWorkSpace = (pCenter*)malloc(sizeof(pCenter));

	FILE* pFile = fopen(pFilePath, "r");

	int col1, col2, col3;

	memset(pWorkSpace, 0, sizeof(pCenter));


	// todo option
	pWorkSpace->inputType = ORLIB;
	pWorkSpace->searchType = NONE;

	if (pWorkSpace->inputType == ORLIB)
	{
		fscanf(pFile, "%d %d %d", &col1, &col2, &col3);
		printf("%d %d %d\n", col1, col2, col3);
	}
	else
	{
		// todo
	}

	pWorkSpace->numOfNodes = col1;
	pWorkSpace->numOfEdges = col2;
	pWorkSpace->numOfFacility = col3;

	pWorkSpace->distanceGraph = (int **)malloc(sizeof(int*)*pWorkSpace->numOfNodes);
	pWorkSpace->sortedGraph = (int **)malloc(sizeof(int*)*pWorkSpace->numOfNodes);
	pWorkSpace->disSeqGraph = (int **)malloc(sizeof(int*)*pWorkSpace->numOfNodes);
	if (pWorkSpace->distanceGraph != NULL && pWorkSpace->disSeqGraph != NULL &&
		pWorkSpace->disSeqGraph != NULL)
	{
		int i = 0;
		int j = 0;
		// 申请结点邻接矩阵内存
		for (; i < pWorkSpace->numOfNodes; ++i)
		{
			pWorkSpace->distanceGraph[i] = (int*)malloc(sizeof(int)*pWorkSpace->numOfNodes);
			pWorkSpace->sortedGraph[i] = (int*)malloc(sizeof(int)*pWorkSpace->numOfNodes);
			pWorkSpace->disSeqGraph[i] = (int*)malloc(sizeof(int)*pWorkSpace->numOfNodes);
			if (pWorkSpace->distanceGraph[i] == NULL || pWorkSpace->sortedGraph[i] == NULL
				|| pWorkSpace->disSeqGraph[i] == NULL)
			{
				return NULL;
			}
			memset(pWorkSpace->distanceGraph[i], 0, sizeof(int)*pWorkSpace->numOfNodes);
			memset(pWorkSpace->sortedGraph[i], 0, sizeof(int)*pWorkSpace->numOfNodes);
			memset(pWorkSpace->disSeqGraph[i], 0, sizeof(int)*pWorkSpace->numOfNodes);
		}

		pWorkSpace->dTable = (DTable *)malloc(sizeof(DTable)*pWorkSpace->numOfNodes);
		pWorkSpace->fTable = (FTable *)malloc(sizeof(FTable)*pWorkSpace->numOfNodes);
		pWorkSpace->faclity = (int *)malloc(sizeof(int)*pWorkSpace->numOfFacility + 1);
		pWorkSpace->bestSol = (int *)malloc(sizeof(int)*pWorkSpace->numOfFacility);
		pWorkSpace->mf = (int *)malloc(sizeof(int)*pWorkSpace->numOfFacility + 1);

		pWorkSpace->listTabu = (int*)malloc(sizeof(int)*pWorkSpace->numOfNodes);
		memset(pWorkSpace->listTabu, 0, sizeof(int)*pWorkSpace->numOfNodes);

		pWorkSpace->tabuLen = (int)sqrt((double)pWorkSpace->numOfNodes);
		pWorkSpace->tabuCur = 0;
		pWorkSpace->tabuList = (SwapPair*)malloc(sizeof(SwapPair)*pWorkSpace->tabuLen);
		memset(pWorkSpace->tabuList, 0, sizeof(SwapPair)*pWorkSpace->tabuLen);

		if (!pWorkSpace->dTable || !pWorkSpace->fTable || !pWorkSpace->faclity)
		{
			return NULL;
		}
		else
		{
			// 初始化D表
			int j = 0;
			for (; j < pWorkSpace->numOfNodes; ++j)
			{
				pWorkSpace->dTable[j].fir_distance =
					pWorkSpace->dTable[j].sec_distance = INT_MAX;
			}
		}

		// 读取数据
		while (!feof(pFile))
		{
			fscanf(pFile, "%d %d %d", &col1, &col2, &col3);
			pWorkSpace->distanceGraph[col1 - 1][col2 - 1] =
				pWorkSpace->distanceGraph[col2 - 1][col1 - 1] = col3;
		}

		// for debug
		if (debug_info)
		{
			int i = 0;
			int j = 0;

			fprintf(stdout, "   origin distance graph \n");
			for (i = 0; i < pWorkSpace->numOfNodes; ++i)
			{
				for (j = 0; j < pWorkSpace->numOfNodes; ++j)
				{
					fprintf(stdout, "%d ", pWorkSpace->distanceGraph[i][j]);
				}

				fprintf(stdout, "\n");
			}
			fprintf(stdout, "\n");
		}
		// end for debug

		_pcGetShortPathByPloyd(pWorkSpace);

		for (i = 0; i < pWorkSpace->numOfNodes; i++)
		{
			for (j = 0; j < pWorkSpace->numOfNodes; j++)
			{
				pWorkSpace->sortedGraph[i][j] = j;
			}
		}

		printMatrix(pWorkSpace->sortedGraph, pWorkSpace->numOfNodes, pWorkSpace->numOfNodes);

		disGraph = pWorkSpace->distanceGraph;
		for (global_i = 0; global_i < pWorkSpace->numOfNodes; global_i++)
		{
			qsort(&pWorkSpace->sortedGraph[global_i][0],
				pWorkSpace->numOfNodes, sizeof(int), compOfqsort);
		}

		printMatrix(pWorkSpace->sortedGraph, pWorkSpace->numOfNodes, pWorkSpace->numOfNodes);

		for (i = 0; i < pWorkSpace->numOfNodes; i++)
		{
			for (j = 0; j < pWorkSpace->numOfNodes; j++)
			{
#ifdef consider_same_distance_node
				//考虑相同距离点的版本
				pWorkSpace->disSeqGraph[i][pWorkSpace->sortedGraph[i][j]] = j;
#else
				// 相同距离的点的序号是相同的，方便之后进行局部搜索时探索更少的点
				//（相同距离的点是否进行探索还需要进一步的测试，简单测试表明不考虑相同距离的点更快一点）
				if (j && (pWorkSpace->distanceGraph[i][pWorkSpace->sortedGraph[i][j]] ==
					pWorkSpace->distanceGraph[i][pWorkSpace->sortedGraph[i][j - 1]]))
				{
					pWorkSpace->disSeqGraph[i][pWorkSpace->sortedGraph[i][j]] =
						pWorkSpace->disSeqGraph[i][pWorkSpace->sortedGraph[i][j - 1]];
				}
				else
				{
					pWorkSpace->disSeqGraph[i][pWorkSpace->sortedGraph[i][j]] = j;
				}
#endif // consider_same_distance_node
			}
		}
	}
	else
	{
		return NULL;
	}

	fclose(pFile);

	return pWorkSpace;
}

pcStatus pcSetOption(void* pData, int argc, int* argv[])
{
	return pcOK;
}

pcStatus pcLocalSearch(void* pData)
{
	pCenter* pWorkSpace = (pCenter*)pData;
	if (pWorkSpace == NULL)
	{
		return pcFatal;
	}

	// 随机选择第一个结点
	{
		time_t curTime;
		time(&curTime);
		srand((unsigned int)time(NULL));

		_pcAddFacility(pWorkSpace, rand() % pWorkSpace->numOfNodes);
	}

	// 填充其他服务点
	{
		while (pWorkSpace->curNumOfF < pWorkSpace->numOfFacility)
		{
			// 搜索最长边
			int i = 0;
			int numOfRepeat = 0;
			int maxLen = 0;
			int maxLenNode = 0;
			for (; i < pWorkSpace->numOfNodes; ++i)
			{
				if (maxLen < pWorkSpace->dTable[i].fir_distance)
				{
					maxLen = pWorkSpace->dTable[i].fir_distance;
					maxLenNode = i;
					numOfRepeat = 1;
				}
				else if (maxLen == pWorkSpace->dTable[i].fir_distance)
				{
					numOfRepeat++;

					// 随机选取一个领域内最长边的结点
					if (!(rand() % numOfRepeat))
					{
						maxLenNode = i;
					}
				}
			}

			_pcAddFacility(pWorkSpace, maxLenNode);
		}

		{
			// 初始化最优解
			int j = 0;
			for (j = 0; j < pWorkSpace->numOfNodes; ++j)
			{
				pWorkSpace->bestSol[j] = pWorkSpace->faclity[j];
			}
			pWorkSpace->bestObj = pWorkSpace->objectFunction;
		}
	}

	if (debug_info)
	{
		pcPrintResult(pWorkSpace, stdout);
		pcPrintF_D_Table(pWorkSpace, stdout);
	}

	// 执行局部搜索
	_pcLS(pWorkSpace);

	return pcOK;
}

pcStatus pcPrintResult(void* pData, FILE* pFile)
{
	pCenter* pWorkSpace = (pCenter*)pData;

	int i = 0;
	int j = 0;

	fprintf(pFile, "    distance graph \n");
	for (i = 0; i < pWorkSpace->numOfNodes; ++i)
	{
		for (j = 0; j < pWorkSpace->numOfNodes; ++j)
		{
			fprintf(pFile, "%d ", pWorkSpace->distanceGraph[i][j]);
		}

		fprintf(pFile, "\n");
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "    sorted graph \n");
	for (i = 0; i < pWorkSpace->numOfNodes; ++i)
	{
		for (j = 0; j < pWorkSpace->numOfNodes; ++j)
		{
			fprintf(pFile, "%d ", pWorkSpace->sortedGraph[i][j]);
		}

		fprintf(pFile, "\n");
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "    sequence graph \n");
	for (i = 0; i < pWorkSpace->numOfNodes; ++i)
	{
		for (j = 0; j < pWorkSpace->numOfNodes; ++j)
		{
			fprintf(pFile, "%d ", pWorkSpace->disSeqGraph[i][j]);
		}

		fprintf(pFile, "\n");
	}
	fprintf(pFile, "\n");

	return pcOK;
}

pcStatus pcPrintF_D_Table(void* pData, FILE* pFile)
{
	pCenter* pWorkSpace = (pCenter*)pData;
	int i = 0;

	fprintf(pFile, "F Table:\n");
	for (i = 0; i < pWorkSpace->numOfNodes; ++i)
	{
		fprintf(pFile, "%d %d %d\n", i + 1, pWorkSpace->fTable[i].fir_facility + 1,
			pWorkSpace->fTable[i].sec_facility + 1); // +1表示顺序列表顺序从1开始,矩阵存储时从0开始
	}

	fprintf(pFile, "\n");

	fprintf(pFile, "D Table:\n");
	for (i = 0; i < pWorkSpace->numOfNodes; ++i)
	{
		fprintf(pFile, "%d %d %d\n", i + 1,
			pWorkSpace->dTable[i].fir_distance, pWorkSpace->dTable[i].sec_distance);
	}

	fprintf(pFile, "\n");

	return pcOK;
}

pcStatus pcFinalize(void* pData)
{
	pCenter* pWorkSpace = (pCenter*)pData;

	free(pWorkSpace);

	return pcOK;
}

//}