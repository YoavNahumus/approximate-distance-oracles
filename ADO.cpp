#include "ADO.hpp"
#include "libs/fibo/fiboqueue.h"
#include <limits>
#include <execution>
#include <algorithm>
#include <set>

using std::numeric_limits;
using std::for_each;
using std::execution::par;
using std::min;
using std::set;

ADO::ADO(Graph* graph, int k, bool isClassic) : graph(graph), k(k), isClassic(isClassic) {
    hierarchy = new map<vertex, map<vertex, distance>*>*[k];
    for (auto p = hierarchy; p < hierarchy + k; ++p) {
        *p = new map<vertex, map<vertex, distance>*>();
    }
    bunches = new map<vertex, distance>*[graph->vertexCount];
    for (auto p = bunches; p < bunches + graph->vertexCount; ++p) {
        *p = new map<vertex, distance>();
    }
    ps = new pair<vertex, distance>*[graph->vertexCount];
    for (auto p = ps; p < ps + graph->vertexCount; ++p) {
        *p = new pair<vertex, distance>[k + 1];
    }
}

ADO::~ADO() {
    for (auto p = ps; p < ps + graph->vertexCount; delete[] *(p++));
    delete[] ps;

    for (auto p = bunches; p < bunches + graph->vertexCount; delete *(p++));
    delete[] bunches;

    for (auto p = hierarchy; p < hierarchy + k; ++p) {
        for (auto &&q : **p) {
            if (q.second != nullptr)
                delete q.second;
        }
    }
    for (auto p = hierarchy; p < hierarchy + k; delete *(p++));
    delete[] hierarchy;
}

void ADO::preprocess() {
    buildHierarchy();
    buildPS();
    buildClusters();
    buildBunches();
}

void ADO::preprocess(bool random) {
    if (!random && !isClassic) {
        preprocess();
        return;
    }
    buildRandHierarchy();
    buildPS();
    buildClusters();
    buildBunches();
}

distance ADO::query(vertex vertex1, vertex vertex2) {
    if (vertex1 == vertex2) return 0;
    return min(asymetricQuery(vertex1, vertex2), asymetricQuery(vertex2, vertex1));
}

distance ADO::asymetricQuery(vertex vertex1, vertex vertex2) {
    if (graph->hasEdge(vertex1, vertex2) > 0)
        return graph->getEdgeWeight(vertex1, vertex2);

    vertex w = vertex1;
    vertex i = 0;
    int index = (k - 1) / 2;
    if (isClassic) index = k;
    while (bunches[vertex2]->count(w) == 0 && i <= index) {
        ++i;
        vertex1 ^= vertex2;
        vertex2 ^= vertex1;
        vertex1 ^= vertex2;
        w = ps[vertex1][i].first;
    }
    if (i > index) {
        auto pu = ps[vertex1][index];
        auto pv = ps[vertex2][index + 1 - k % 2];
        return pu.second + bunches[pv.first]->at(pu.first) + pv.second;
    }
        
    return ps[vertex1][i].second + bunches[vertex2]->at(w);
}

void ADO::buildHierarchy() {
    // TODO: Implement this function
}

void ADO::buildRandHierarchy() {
    srand(time(nullptr));
    double chance = pow(graph->vertexCount, -1.0 / k);
    for (int j = 0; j < graph->vertexCount; ++j) {
        hierarchy[0]->insert({j, nullptr});
    }
    for (int i = 1; i < k; ++i) {
        for (auto&& j : *hierarchy[i - 1]) {
            if (rand() < chance * RAND_MAX) {
                hierarchy[i]->insert(j);
            }
        }
    }

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
        for (auto&& j : *hierarchy[i]) {
            fibQueue->push(0.0, j.first);
            ps[j.first][i] = {j.first, 0.0};
        }

        while (!fibQueue->empty()) {
            auto v = fibQueue->pop();
            auto p = ps[v][i];
            for (auto&& j : graph->getEdges(v)) {
                distance alt = p.second + j.second;
                if (alt < ps[j.first][i].second) {
                    ps[j.first][i] = {p.first, alt};
                    fibQueue->decrease_key_or_push(alt, j.first);
                }
            }
        }

        for (auto p = ps; p < ps + graph->vertexCount; ++p) {
            if ((*p)[i].first == (*p)[i + 1].first) {
                (*p)[i] = (*p)[i + 1];
            }
        }

        fibQueue->clear();
    }
    delete fibQueue;
}

void ADO::buildCluster(int i, pair<const vertex, map<vertex, distance>*>* q) {
    // make sure that the vertex is not in the next level (A_(i+1))
    if ((i < k - 1 && hierarchy[i + 1]->count(q->first) == 0) || i == k - 1) {
        q->second = new map<vertex, distance>();
        FibQueue<distance, vertex>* fibQueue = new FibQueue<distance, vertex>();
        fibQueue->push(0.0, q->first);
        q->second->insert_or_assign(q->first, 0.0);
        set<vertex>* to_remove = nullptr;
        int index = 0;
        if (i >= (k - 1) / 2 && !isClassic) {
            to_remove = new set<vertex>();
            index = (k - 1) / 2 + 1 - k % 2;
        }

        while (!fibQueue->empty()) {
            auto v = fibQueue->pop();
            auto p = q->second->find(v);
            for (auto&& e : graph->getEdges(v)) {
                distance alt = p->second + e.second;
                if (alt <= ps[e.first][i + 1].second && (q->second->count(e.first) == 0 || alt < q->second->at(e.first))) {
                    q->second->insert_or_assign(e.first, alt);
                    fibQueue->decrease_key_or_push(alt, e.first);
                } else if (to_remove != nullptr && (q->second->count(e.first) == 0 || alt < q->second->at(e.first))) {
                    if (hierarchy[index]->count(e.first) != 1) {
                        to_remove->insert(e.first);
                    }
                    q->second->insert_or_assign(e.first, alt);
                    fibQueue->decrease_key_or_push(alt, e.first);
                }
            }
        }

        if (to_remove != nullptr) {
            for (auto&& v : *to_remove) {
                q->second->erase(v);
            }
            delete to_remove;
        }

        delete fibQueue;
    }
}

void ADO::buildClusters() {
    for (int i = 0; i < k; ++i) {
        for_each(par, hierarchy[i]->begin(), hierarchy[i]->end(), [this, i](auto&& p) {
            buildCluster(i, &p);
        });
    }
}

void ADO::buildBunches() {
    for (auto p = hierarchy; p < hierarchy + k; ++p) {
        for (auto&& q : **p) {
            if (q.second == nullptr) {
                continue;
            }
            for (auto&& r : *q.second) {
                bunches[r.first]->insert_or_assign(q.first, r.second);
            }
        }
    }
}