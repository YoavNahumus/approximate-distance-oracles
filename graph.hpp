#ifndef OUR_GRAPH_HPP
#define OUR_GRAPH_HPP

#include <map>

using std::map;

typedef int vertex;
typedef double distance;

class Graph {
private:
    map<vertex, distance>** neighborsList;
public:
    const vertex vertexCount;
    vertex edgeCount;

    Graph(vertex vertexCount);
    ~Graph();

    double getEdgeWeight(vertex vertex1, vertex vertex2);
    const map<vertex, distance>& getEdges(vertex vertex);
    bool hasEdge(vertex vertex1, vertex vertex2);

    void addEdge(vertex vertex1, vertex vertex2, distance weight);
};

#endif