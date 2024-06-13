#include "ADO.hpp"
#include "libs/fibo/fiboqueue.h"

ADO::ADO(Graph* graph, int k) : graph(graph), k(k) {
    hierarchy = new map<vertex, map<vertex, distance>*>*[k];
    bunches = new map<vertex, distance>*[graph->vertexCount];
    ps = new pair<vertex, distance>*[graph->vertexCount];
    for (auto &&p = ps; p < ps + graph->vertexCount; ++p) {
        *p = new pair<vertex, distance>[k];
    }
}

ADO::~ADO() {
    for (auto &&p = ps; p < ps + graph->vertexCount; delete[] *(p++));
    delete[] ps;

    if (bunches[0] != nullptr)
        for (auto &&p = bunches; p < bunches + graph->vertexCount; delete *(p++));
    delete[] bunches;

    if (hierarchy[0] != nullptr) {
        if (hierarchy[0]->begin()->second != nullptr)
            for (auto &&p : *hierarchy[0])
                delete p.second;
        for (auto &&p = hierarchy; p < hierarchy + k; delete *(p++));
    }
    delete[] hierarchy;
}

void ADO::preprocess() {
    buildHierarchy();
    buildPS();
    buildClusters();
    buildBunches();
}

distance ADO::query(vertex vertex1, vertex vertex2) {
    // TODO: Implement this function
    return 0.0;
}

void ADO::buildHierarchy() {
    // TODO: Implement this function
}

void ADO::buildPS() {
    // TODO: Implement this function
}

void ADO::buildClusters() {
    // TODO: Implement this function
}

void ADO::buildBunches() {
    // TODO: Implement this function
}