#include "ADO.hpp"
#include "libs/fibo/fiboqueue.h"
#include <limits>

using std::numeric_limits;

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
    FibQueue<distance, vertex>* fibQueue = new FibQueue<distance, vertex>();
    for (auto &&p = ps; p < ps + graph->vertexCount; ++p) {
        for (int i = 0; i < k; ++i) {
            *p[i] = { -1, numeric_limits<distance>::max() };
        }
    }
    for (int i = 0; i < k; ++i) {
        for (auto&& j : *hierarchy[i]) {
            fibQueue->push(0.0, j.first);
            ps[j.first][i] = {j.first, 0.0};
        }

        while (!fibQueue->empty()) {
            auto node = fibQueue->extract_min();
            auto p = ps[node->value][i];
            for (auto&& j : graph->getEdges(node->value)) {
                distance alt = p.second + graph->getEdgeWeight(node->value, j.first);
                if (alt < ps[j.first][i].second) {
                    ps[j.first][i] = {p.first, alt};
                    fibQueue->decrease_key_or_push(alt, j.first);
                }
            }
        }

        fibQueue->clear();
    }
    delete fibQueue;
}

void ADO::buildClusters() {
    // TODO: Implement this function
}

void ADO::buildBunches() {
    // TODO: Implement this function
}