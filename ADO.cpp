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
using std::function;
using std::unordered_map;

ADO::ADO(Graph* graph, int k, bool isClassic) : graph(graph), k(k), isClassic(isClassic) {
    hierarchy = new unordered_map<vertex, unordered_map<vertex, distance>*>*[k];
    for (auto p = hierarchy; p < hierarchy + k; ++p) {
        *p = new unordered_map<vertex, unordered_map<vertex, distance>*>();
    }
    bunches = new unordered_map<vertex, distance>*[graph->vertexCount];
    for (auto p = bunches; p < bunches + graph->vertexCount; ++p) {
        *p = new unordered_map<vertex, distance>();
    }
    ps = new pair<vertex, distance>*[graph->vertexCount];
    for (auto p = ps; p < ps + graph->vertexCount; ++p) {
        *p = new pair<vertex, distance>[k + 1];
    }
    reduced = false;
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
    if (reduced) delete graph;
}

void ADO::preprocess() {
    buildHierarchy();
    std::cout << "Hierarchy built" << std::endl;
    buildPS();
    std::cout << "PS built" << std::endl;
    buildClusters();
    std::cout << "Clusters built" << std::endl;
    buildBunches();
    std::cout << "Bunches built" << std::endl;
}

void ADO::preprocess(int* heirarchySizes) {
    buildRandHierarchy(heirarchySizes);
    std::cout << "Hierarchy built" << std::endl;
    buildPS();
    std::cout << "PS built" << std::endl;
    buildClusters();
    std::cout << "Clusters built" << std::endl;
    buildBunches();
    std::cout << "Bunches built" << std::endl;
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
        auto pv = ps[vertex2][k / 2];
        return pu.second + bunches[pv.first]->at(pu.first) + pv.second;
    }
        
    return ps[vertex1][i].second + bunches[vertex2]->at(w);
}

template<class T>
set<T>* ADO::hittingSet(set<T>** sets, int setCount) {
    set<T>* result = new set<T>;
    set<pair<T, set<set<T>*>*>>** arr = new set<pair<T, set<set<T>*>*>>*[setCount];
    unordered_map<T, set<set<T>*>*>* inverse = new unordered_map<T, set<set<T>*>*>();
    for (int i = 0; i < setCount; ++i) {
        arr[i] = new set<pair<T, set<set<T>*>*>>;
        for (auto&& v : *sets[i]) {
            auto iter = inverse->find(v);
            if (iter == inverse->end()) {
                set<set<T>*>* s = new set<set<T>*>;
                s->insert(sets[i]);
                inverse->insert({v, s});
            } else {
                iter->second->insert(sets[i]);
            }
        }
    }

    for (auto&& p : *inverse) {
        arr[p.second->size() - 1]->insert({p.first, p.second});
    }
    delete inverse;

    set<set<T>*>* alreadyHit = new set<set<T>*>;
    for (int i = setCount - 1; i > 1 && alreadyHit->size() != setCount; --i) {
        for (auto&& p : *arr[i]) {
            int size = p.second->size();
            for (auto&& s : *alreadyHit) {
                if (p.second->count(s) > 0) {
                    p.second->erase(s);
                }
            }
            if (size = p.second->size()) {
                result->insert(p.first);
                for (auto&& s : *p.second) {
                    alreadyHit->insert(s);
                }
            }
        }
    }

    for (int i = 0; i < setCount; ++i) {
        for (auto&& p : *arr[i]) {
            delete p.second;
        }
        delete arr[i];
    }

    delete alreadyHit;

    delete[] arr;
    
    return result;
}

void ADO::buildHierarchy() {
    if (isClassic) {
        buildRandHierarchy();
        return;
    }
    srand(time(nullptr));
    double chance = pow(graph->vertexCount, -1.0 / k);
    for (int j = 0; j < graph->vertexCount; ++j) {
        hierarchy[0]->insert({j, nullptr});
        
    }
    set<vertex>** sets = new set<vertex>*[graph->vertexCount];
    double s = pow(graph->edgeCount, 1.0 / k) * log(graph->vertexCount);
    #pragma omp parallel for
    for (int j = 0; j < graph->vertexCount; ++j) {
        sets[j] = new set<vertex>;
        graph->dijkstra(j, s, [this, j, sets](vertex v, distance d) {
            sets[j]->insert(v);
        });
    }

    std::cout << "Hitting set" << std::endl;

    set<vertex>* hitting = hittingSet(sets, graph->vertexCount);
    for (auto&& v : *hitting) {
        hierarchy[1]->insert({v, nullptr});
    }
    buildPS();

    set<vertex>** sets2 = new set<vertex>*[graph->vertexCount];
    for (int j = 0; j < graph->vertexCount; ++j) {
        sets2[j] = new set<vertex>;
        graph->dijkstra(j, [this](vertex v, distance d) {
            return d < ps[v][1].second;
        }, [this, j, sets](vertex v, distance d) {
            for (auto&& e : graph->getEdges(v)) {
                if (sets[j]->count(e.first) == 0){
                    return true;
                }
            }
            return false;
        }, [this, j, sets2](vertex v, distance d) {
            sets2[j]->insert(v);
        });
    }

    delete hitting;
    hitting = hittingSet(sets2, graph->vertexCount);
    for (auto&& v : *hitting) {
        hierarchy[1]->insert({v, nullptr});
    }
    for (int i = 0; i < graph->vertexCount; ++i) {
        delete sets2[i];
        delete sets[i];
    }
    delete[] sets2;
    delete[] sets;
    delete hitting;

    for (int i = 2; i < k; ++i) {
        for (auto&& j : *hierarchy[i - 1]) {
            if (rand() < chance * RAND_MAX) {
                hierarchy[i]->insert(j);
            }
        }
    }
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

void ADO::buildRandHierarchy(int* heirarchySizes) {
    srand(time(nullptr));
    double chance = pow(graph->vertexCount, -1.0 / k);
    for (int j = 0; j < graph->vertexCount; ++j) {
        hierarchy[0]->insert({j, nullptr});
    }
    for (int i = 1; i < k; ++i) {
        // add heirarchySizes[i] vertices to the hierarchy
        do {
            auto iter = hierarchy[i - 1]->begin();
            std::advance(iter, rand() % hierarchy[i - 1]->size());
            hierarchy[i]->insert(*iter);
        } while (hierarchy[i]->size() != heirarchySizes[i]);
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

void ADO::buildCluster(int i, pair<const vertex, unordered_map<vertex, distance>*>* q) {
    if ((i == k - 1 || hierarchy[i + 1]->count(q->first) == 0) && (i != (k - 1) / 2 || isClassic)) {
        q->second = new unordered_map<vertex, distance>();
        graph->dijkstra(q->first, [this, i](vertex v, distance d) {
            return d < ps[v][i + 1].second;
        }, [this, i](vertex v, distance d) {
            return true;
        }, [this, q](vertex v, distance d) {
            q->second->insert_or_assign(v, d);
        });
    } else if (i == (k - 1) / 2 && !isClassic) {
        q->second = new unordered_map<vertex, distance>();
        graph->dijkstra(q->first, [this, i](vertex v, distance d) {
            return true;
        }, [this, i](vertex v, distance d) {
            return d < ps[v][i + 1].second || hierarchy[k / 2]->count(v) == 1;
        }, [this, q](vertex v, distance d) {
                q->second->insert_or_assign(v, d);
        });
    }
}

void ADO::buildClusters() {
    for (int i = k - 1; i >= 0; --i) {
        int size = hierarchy[i]->size();
        #pragma omp parallel for
        for (int j = 0; j < size; ++j) {
            auto iter = hierarchy[i]->begin();
            std::advance(iter, j);
            buildCluster(i, &(*iter));
        }
        std::cout << "Cluster " << i << " built" << std::endl;
        std::cout << "Heirarchy size: " << hierarchy[i]->size() << std::endl;
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