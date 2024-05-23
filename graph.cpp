#include "graph.hpp"

Graph::Graph(int vertexCount) : vertexCount(vertexCount) {
    neighborsList = new map<int, double>*[vertexCount];
    for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; *(ptr++) = new map<int, double>());
}

Graph::~Graph() {
    for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; delete *(ptr++));
    delete neighborsList;
}

double Graph::getEdgeWeight(int vertex1, int vertex2) {
    return neighborsList[vertex1]->at(vertex2);
}

const map<int, double>& Graph::getEdges(int vertex) {
    return *neighborsList[vertex];
}

void Graph::addEdge(int vertex1, int vertex2, double weight) {
    neighborsList[vertex1]->insert_or_assign(vertex2, weight);
    neighborsList[vertex2]->insert_or_assign(vertex1, weight);
}