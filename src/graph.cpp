
#include "graph.h"
#include <fstream>
#include <iostream>

using namespace std;

HO_NAMESPACE_BEGIN(gcp)

Vertex::Vertex(int id)
: id(id)
, color(-1)
, prev(hoNull)
, next(hoNull)
, adja(hoNull)
, numAdj(0)
{
	// 建立头结点
	adja = new Adjacent(this);
	adja->prev = adja;
	adja->next = adja;
}

Vertex::~Vertex()
{
	Adjacent* cur = adja->next;
	while (cur != adja)
	{
		Adjacent* del = cur;

		cur = cur->next;
		delete del;
	}
}

void Vertex::appendAdj(Adjacent* adj)
{
	Adjacent* head = adja;
	
	adj->prev = head;
	adj->next = head->next;
	head->next->prev = adj;
	head->next = adj;

	numAdj++;
}

void Vertex::insertEdges(Adjacent* adj, Adjacent* curadj)
{
	adj->prev = curadj->prev;
	curadj->prev->next = adj;
	curadj->prev = adj;
	adj->next = curadj;
}

void Vertex::deleteEdges(Adjacent** adj)
{
	Adjacent* a = *adj;

	a->prev->next = a->next;
	a->next->prev = a->prev;

	*adj = a->next;
	delete a;
}

Graph::Graph()
	: numNodes(0)
	, numEdges(0)
	, nodesList(hoNull)
{
	// 建立顶点表的头结点
	nodesList = new Vertex(0);
	nodesList->prev = nodesList;
	nodesList->next = nodesList;
}

hoStatus Graph::Init()
{
	nodesList = new Vertex(0);

	return hoOK;
}

hoStatus Graph::InitGraphFromFile(const string& strfile)
{
	fstream in(strfile);
	if (!in.is_open())
	{
		return hoFileOpenError;
	}

	cout << "Graph File: " << strfile << endl;

	// 解析实例文件
	int node1 = 0;
	int node2 = 0;
	char buffer[BUFFER_SIZE];
	in >> buffer;
	while (!in.eof())
	{
		if (strcmp(buffer, "p") == 0)
		{
			char edges[64];
			in >> edges >> numNodes >> numEdges;

			// 初始化顶点
			cout << "Graph nodes and edges: " << numNodes << " " << numEdges << endl;

			InitVertex();
		}
		else if (strcmp(buffer, "e") == 0)
		{
			in >> node1 >> node2;
			if (node1 == node2)
			{
				continue;
			}

			//cout << "Edges: " << node1 << " " << node2 << endl;

			Vertex* v1 = GetVertexFromList(node1);
			Vertex* v2 = GetVertexFromList(node2);
			Adjacent* adj1 = new Adjacent(v1);
			Adjacent* adj2 = new Adjacent(v2);

			v1->appendAdj(adj2);
			v2->appendAdj(adj1);
		}

		in >> buffer;
	}

	cout << "Load graph file succeed!\n";

	in.close();

	//PringGraph();

	return hoOK;
}

void Graph::AppendVertex(Vertex* v)
{
	Vertex* head = nodesList;

	v->prev = head;
	v->next = head->next;
	head->next->prev = v;
	head->next = v;
}

void Graph::InsertVertex(Vertex* v, Vertex* curv)
{
	v->prev = curv->prev;
	curv->prev->next = v;
	v->next = curv;
	curv->prev = v;
}

bool Graph::emptyNodes()
{
	return nodesList->prev==nodesList && nodesList->next==nodesList;
}

Vertex* Graph::firstNodes()
{
	if (emptyNodes())
	{
		return hoNull;
	}

	return nodesList->next;
}

Vertex* Graph::nextNodes(Vertex* cur)
{
	if (emptyNodes() || cur==hoNull)
	{
		return hoNull;
	}

	return cur->next;
}

void Graph::deleteVertex(Vertex** v)
{
	Vertex* n = *v;

	n->next->prev = n->prev;
	n->prev->next = n->next;

	*v = n->next;

	delete n;
}

void Graph::PringGraph()
{
	cout << "Graph: " << numNodes << " " << numEdges << endl;

	Vertex* cur = nodesList->next;
	while (cur != nodesList)
	{
		Adjacent* curadj = cur->adja->next;

		while (curadj != cur->adja)
		{
			cout << "Edges: " << cur->id << " " << curadj->vertex->id << endl;
			
			curadj = curadj->next;
			
		}

		cur = cur->next;
	}
}

Vertex* Graph::GetVertexFromList(int id)
{
	Vertex* cur = nodesList;

	while (cur != hoNull)
	{
		if (id == cur->id)
		{
			return cur;
		}

		cur = cur->next;
	}

	return hoNull;
}

void Graph::InitVertex()
{
	for (int i = 1; i < numNodes+1; i++)
	{
		Vertex* v = new Vertex(i);
		AppendVertex(v);
	}
}

Graph::~Graph()
{
	Vertex* cur = nodesList->next;
	while (cur != nodesList)
	{
		Vertex* del = cur;
		cur = cur->next;
		delete del;
	}
}

HO_NAMESPACE_END