#include "ADO.hpp"
#include "libs/fibo/fiboqueue.h"
#include <limits>
#include <execution>

using std::numeric_limits;
using std::for_each;
using std::execution::par;

ADO::ADO(Graph* graph, int k) : graph(graph), k(k) {
    hierarchy = new map<vertex, map<vertex, distance>*>*[k];
    for (auto &&p = hierarchy; p < hierarchy + k; ++p) {
        *p = new map<vertex, map<vertex, distance>*>();
    }
    bunches = new map<vertex, distance>*[graph->vertexCount];
    for (auto &&p = bunches; p < bunches + graph->vertexCount; ++p) {
        *p = new map<vertex, distance>();
    }
    ps = new pair<vertex, distance>*[graph->vertexCount];
    for (auto &&p = ps; p < ps + graph->vertexCount; ++p) {
        *p = new pair<vertex, distance>[k + 1];
    }
}

ADO::~ADO() {
    for (auto &&p = ps; p < ps + graph->vertexCount; delete[] *(p++));
    delete[] ps;

    for (auto &&p = bunches; p < bunches + graph->vertexCount; delete *(p++));
    delete[] bunches;

    if (hierarchy[0]->begin() != hierarchy[0]->end() && hierarchy[0]->begin()->second != nullptr) {
        for (auto &&p = hierarchy; p < hierarchy + k - 1; ++p) {
            for (auto &&q : **p) {
                delete q.second;
            }
        }
    }
    for (auto &&p = hierarchy; p < hierarchy + k - 1; delete *(p++));
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
    for (int j = 0; j < graph->vertexCount; ++j) {
        for (int i = 1; i < k + 1; ++i) {
            ps[j][i] = { -1, numeric_limits<distance>::max() };
        }
        ps[j][0] = { j, 0.0 };
    }
    for (int i = k - 1; i > 0; --i) {
        for (auto&& j : *hierarchy[i + 1]) {
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

        for (auto&& p = ps; p < ps + graph->vertexCount; ++p) {
            if ((*p)[i].first == (*p)[i + 1].first) {
                (*p)[i] = (*p)[i + 1];
            }
        }

        fibQueue->clear();
    }
    delete fibQueue;
}

void ADO::buildCluster(int i, pair<vertex, map<vertex, distance>*> q) {
    // make sure that the vertex is not in the next level (A_(i+1))
    if (i < k - 1 && hierarchy[i - 1]->count(q.first) == 0) {
        q.second = new map<vertex, distance>();
        FibQueue<distance, vertex>* fibQueue = new FibQueue<distance, vertex>();
        fibQueue->push(0.0, q.first);

        while (!fibQueue->empty()) {
            auto node = fibQueue->extract_min();
            auto p = q.second->find(node->value);
            for (auto&& e : graph->getEdges(node->value)) {
                distance alt = p->second + graph->getEdgeWeight(node->value, e.first);
                if (alt < ps[e.first][i + 1].second && (q.second->count(e.first) == 0 || alt < q.second->at(e.first))) {
                    q.second->insert_or_assign(e.first, alt);
                    fibQueue->decrease_key_or_push(alt, e.first);
                }
            }
        }
    }
}

void ADO::buildClusters() {
    for (int i = 0; i < k; ++i) {
        for_each(par, hierarchy[i]->begin(), hierarchy[i]->end(), [this, i](auto&& p) {
            buildCluster(i, {p.first, p.second});
        });
    }
}

void ADO::buildBunches() {
    for (auto&& p = hierarchy; p < hierarchy + k; ++p) {
        for (auto&& q : **p) {
            for (auto&& r : *q.second) {
                bunches[r.first]->insert_or_assign(q.first, r.second);
            }
        }
    }
}