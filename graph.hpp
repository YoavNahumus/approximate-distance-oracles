#ifndef OUR_GRAPH_HPP
#define OUR_GRAPH_HPP

#include <map>

using std::map;

class Graph {
private:
    map<int, double>** neighborsList;
public:
    const int vertexCount;

    Graph(int vertexCount);
    ~Graph();

    double getEdgeWeight(int vertex1, int vertex2);
    const map<int, double>& getEdges(int vertex);

    void addEdge(int vertex1, int vertex2, double weight);
};

#endif