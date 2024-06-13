#include "ADO.hpp"
#include "libs/fibonacci.hpp"

ADO::ADO(Graph* graph, int k) : graph(graph), k(k) {
    hierarchy = new map<int, map<int, double>*>*[k];
    bunches = new map<int, double>*[graph->vertexCount];
    ps = new pair<int, double>*[graph->vertexCount];
    for (auto &&p = ps; p < ps + graph->vertexCount; ++p) {
        *p = new pair<int, double>[k];
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

double ADO::query(int vertex1, int vertex2) {
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