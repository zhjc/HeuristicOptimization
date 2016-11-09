
#ifndef _TS_GRAPH_H
#define _TS_GRAPH_H

#include "config.h"
#include <string>

HO_NAMESPACE_BEGIN(gcp)

struct Vertex;
class GraphColoring;

struct Adjacent{
public:
    Adjacent(Vertex* v) :vertex(v), prev(hoNull), next(hoNull){}

    ~Adjacent(){ prev = hoNull; next = hoNull; }

    Vertex* vertex;

    Adjacent* prev;
    Adjacent* next;
};

struct Vertex{
public:
    Vertex(int id);
    ~Vertex();

    void appendAdj(Adjacent* adj);
    void insertEdges(Adjacent* adj, Adjacent* curadj);
    void deleteEdges(Adjacent** adj);

    int id;
    int color;

    Vertex* prev;
    Vertex* next;

    Adjacent* adja;
    int numAdj;
};

class Graph{
public:
    Graph();

    hoStatus Init();
    hoStatus InitGraphFromFile(const std::string& strfile);

    virtual ~Graph();

public:
    void AppendVertex(Vertex* v);
    void InsertVertex(Vertex* v, Vertex* curv);
    void deleteVertex(Vertex** v);

    bool emptyNodes();
    Vertex* firstNodes();
    Vertex* nextNodes(Vertex* cur);

    void PringGraph();

    Vertex* GetVertexFromList(int id);

private:

    void InitVertex();

private:
    int numNodes;
    int numEdges;

    Vertex* nodesList;

    friend class GraphColoring;
};

HO_NAMESPACE_END

#endif