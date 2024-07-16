#include "graph.hpp"

Graph::Graph(vertex vertexCount) : vertexCount(vertexCount), edgeCount(0) {
    neighborsList = new map<vertex, distance>*[vertexCount];
    for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; *(ptr++) = new map<vertex, distance>());
}

Graph::~Graph() {
    for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; delete *(ptr++));
    delete[] neighborsList;
}

double Graph::getEdgeWeight(vertex vertex1, vertex vertex2) {
    return neighborsList[vertex1]->at(vertex2);
}

const map<vertex, distance>& Graph::getEdges(vertex vertex) {
    return *neighborsList[vertex];
}

void Graph::addEdge(vertex vertex1, vertex vertex2, distance weight) {
    neighborsList[vertex1]->insert_or_assign(vertex2, weight);
    neighborsList[vertex2]->insert_or_assign(vertex1, weight);
    edgeCount++;
}

bool Graph::hasEdge(vertex vertex1, vertex vertex2) {
    return neighborsList[vertex1]->count(vertex2) > 0;
}