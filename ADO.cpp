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

/**
 * Constructor for the ADO class.
 * @param graph The graph.
 * @param k The size of the hierarchy.
 * @param isClassic A boolean value indicating whether the algorithm is classic or not.
 */
ADO::ADO(Graph* graph, int k, bool isClassic) : graph(graph), k(k), isClassic(isClassic) {
    // Initialize the hierarchy
    hierarchy = new unordered_map<vertex, unordered_map<vertex, distance>*>*[k];
    for (auto p = hierarchy; p < hierarchy + k; ++p) {
        *p = new unordered_map<vertex, unordered_map<vertex, distance>*>();
    }
    // Initialize the bunches
    bunches = new unordered_map<vertex, distance>*[graph->vertexCount];
    for (auto p = bunches; p < bunches + graph->vertexCount; ++p) {
        *p = new unordered_map<vertex, distance>();
    }
    // Initialize the ps array
    ps = new pair<vertex, distance>*[graph->vertexCount];
    for (auto p = ps; p < ps + graph->vertexCount; ++p) {
        *p = new pair<vertex, distance>[k + 1];
    }
    reduced = false;
}

/**
 * Destructor for the ADO class.
 */
ADO::~ADO() {
    // Delete the graph, hierarchy, bunches, and ps arrays
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

/**
 * Preprocesses the graph.
 */
void ADO::preprocess() {
    // Build the hirerachy
    buildHierarchy();
    // notify the user that the hierarchy has been built
    std::cout << "Hierarchy built" << std::endl;
    // Build the PS array
    buildPS();
    // notify the user that the PS array has been built
    std::cout << "PS built" << std::endl;
    // Build the clusters
    buildClusters();
    // notify the user that the clusters have been built
    std::cout << "Clusters built" << std::endl;
    // Build the bunches
    buildBunches();
    // notify the user that the bunches have been built
    std::cout << "Bunches built" << std::endl;
}

/**
 * Preprocesses the graph. This version of the function allows the user to specify the size of the hierarchy.
 * @param heirarchySizes The size of the hierarchy.
 */
void ADO::preprocess(int* heirarchySizes) {
    // Build the hierarchy with the specified sizes
    buildRandHierarchy(heirarchySizes);
    // notify the user that the hierarchy has been built
    std::cout << "Hierarchy built" << std::endl;
    // Build the PS array
    buildPS();
    // notify the user that the PS array has been built
    std::cout << "PS built" << std::endl;
    // Build the clusters
    buildClusters();
    // notify the user that the clusters have been built
    std::cout << "Clusters built" << std::endl;
    // Build the bunches
    buildBunches();
    // notify the user that the bunches have been built
    std::cout << "Bunches built" << std::endl;
}
/**
 * Queries the distance between two vertices.
 * @param vertex1 The first vertex.
 * @param vertex2 The second vertex.
 */
distance ADO::query(vertex vertex1, vertex vertex2) {
    // If the vertices are the same, return 0
    if (vertex1 == vertex2) return 0;
    // Return the minimum of the two asymmetric queries
    return min(asymetricQuery(vertex1, vertex2), asymetricQuery(vertex2, vertex1));
}

/**
 * Queries the distance between two vertices. This version of the function is used for asymmetric queries, a directed path from vertex1 to vertex2.
 * @param vertex1 The first vertex.
 * @param vertex2 The second vertex.
 */
distance ADO::asymetricQuery(vertex vertex1, vertex vertex2) {
    // If the graph has an edge between the two vertices, return the weight of the edge
    if (graph->hasEdge(vertex1, vertex2) > 0)
        return graph->getEdgeWeight(vertex1, vertex2);

    // Inintialize the variables
    vertex w = vertex1;
    vertex i = 0;
    // Set the number of the bunches depending on whether the hierarchy is classic or not
    int index = (k - 1) / 2;
    if (isClassic) index = k;
    
    while (bunches[vertex2]->count(w) == 0 && i <= index) {
        // Increment i
        ++i;
        // Swap the vertices if needed
        vertex1 ^= vertex2;
        vertex2 ^= vertex1;
        vertex1 ^= vertex2;
        // Set w to the vertex that is used now
        w = ps[vertex1][i].first;
    }
    // If i is greater than index, return the distance between the two vertices based on the bunches and their distances
    if (i > index) {
        auto pu = ps[vertex1][index];
        auto pv = ps[vertex2][k / 2];
        return pu.second + bunches[pv.first]->at(pu.first) + pv.second;
    }
    
    // Return the distance between the two vertices based on the bunches and their distances
    return ps[vertex1][i].second + bunches[vertex2]->at(w);
}

/**
 * Finds the hitting set of a set of sets.
 * @param sets The set of sets.
 * @param setCount The number of sets.
 */
template<class T>
set<T>* ADO::hittingSet(set<T>** sets, int setCount) {
    // Initialize the variables
    set<T>* result = new set<T>;
    set<pair<T, set<set<T>*>*>>** arr = new set<pair<T, set<set<T>*>*>>*[setCount];
    unordered_map<T, set<set<T>*>*>* inverse = new unordered_map<T, set<set<T>*>*>();
    // Iterate over the sets
    for (int i = 0; i < setCount; ++i) {
        // Initialize the array
        arr[i] = new set<pair<T, set<set<T>*>*>>;
        // Iterate over the elements of the set
        for (auto&& v : *sets[i]) {
            // Insert the element into the inverse map
            auto iter = inverse->find(v);
            // If the element is not in the map, insert it
            if (iter == inverse->end()) {
                set<set<T>*>* s = new set<set<T>*>;
                s->insert(sets[i]);
                inverse->insert({v, s});
            } else {
                // Otherwise, insert the set into the set of sets
                iter->second->insert(sets[i]);
            }
        }
    }

    // Iterate over the inverse map
    for (auto&& p : *inverse) {
        // Insert the element into the end of the array
        arr[p.second->size() - 1]->insert({p.first, p.second});
    }
    // Delete the inverse map
    delete inverse;

    // Initialize the alreadyHit set
    set<set<T>*>* alreadyHit = new set<set<T>*>;
    for (int i = setCount - 1; i > 1 && alreadyHit->size() != setCount; --i) {
        // Iterate over the elements of the array
        for (auto&& p : *arr[i]) {
            // Get the size of the set
            int size = p.second->size();
            // Iterate over the elements of the set
            for (auto&& s : *alreadyHit) {
                // If the set contains the element, erase it
                if (p.second->count(s) > 0) {
                    p.second->erase(s);
                }
            }
            // If the size of the set is the same as the size of the set of sets, insert the element into the result set
            if (size = p.second->size()) {
                result->insert(p.first);
                for (auto&& s : *p.second) {
                    alreadyHit->insert(s);
                }
            }
        }
    }
    // Clean up the memory
    for (int i = 0; i < setCount; ++i) {
        for (auto&& p : *arr[i]) {
            delete p.second;
        }
        delete arr[i];
    }

    delete alreadyHit;

    delete[] arr;
    // Return the result set
    return result;
}

/**
 * Builds the hierarchy.
 */
void ADO::buildHierarchy() {
    // If the algorithm is classic, build the hierarchy using the classic method
    if (isClassic) {
        buildRandHierarchy();
        return;
    }
    // Otherwise, build the hierarchy using the new method
    srand(time(nullptr));
    // Calculate the chance = graph->vertexCount^(-1/k)
    double chance = pow(graph->vertexCount, -1.0 / k);
    // Iterate over the vertices
    for (int j = 0; j < graph->vertexCount; ++j) {
        // Insert the vertex into the hierarchy
        hierarchy[0]->insert({j, nullptr});
        
    }
    // Initialize the sets
    set<vertex>** sets = new set<vertex>*[graph->vertexCount];
    // Calculate the size of the hitting set
    double s = pow(graph->edgeCount, 1.0 / k) * log(graph->vertexCount);
    // Iterate over the vertices
    #pragma omp parallel for
    for (int j = 0; j < graph->vertexCount; ++j) {
        // Initialize the set
        sets[j] = new set<vertex>;
        // Perform a dijkstra search on each vertex and insert the result into the set
        graph->dijkstra(j, s, [this, j, sets](vertex v, distance d) {
            sets[j]->insert(v);
        });
    }
    // Calculate the hitting set
    std::cout << "Hitting set" << std::endl;
    
    set<vertex>* hitting = hittingSet(sets, graph->vertexCount);
    // Insert the vertices of the hitting set into the hierarchy
    for (auto&& v : *hitting) {
        hierarchy[1]->insert({v, nullptr});
    }
    // Build the PS array
    buildPS();
    // Initialize the sets
    set<vertex>** sets2 = new set<vertex>*[graph->vertexCount];
    // Iterate over the vertices
    for (int j = 0; j < graph->vertexCount; ++j) {
        // Initialize the sets
        sets2[j] = new set<vertex>;
        // Perform a dijkstra search on each vertex and insert the result into the set
        graph->dijkstra(j, [this](vertex v, distance d) {
            return d < ps[v][1].second;
        }, [this, j, sets](vertex v, distance d) {
            // Iterate over the edges of the vertex, ordered by neighbor list
            for (auto&& e : graph->getEdges(v)) {
                // If the set does not contain the element, return true
                if (sets[j]->count(e.first) == 0){
                    return true;
                }
            }
            // Otherwise, return false
            return false;
        }, [this, j, sets2](vertex v, distance d) {
            sets2[j]->insert(v);
        });
    }

    delete hitting;
    // Calculate the hitting set
    hitting = hittingSet(sets2, graph->vertexCount);
    for (auto&& v : *hitting) {
        hierarchy[1]->insert({v, nullptr});
    }
    // Clean up the memory
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

/**
 * Builds the random hierarchy.
 */
void ADO::buildRandHierarchy() {
    srand(time(nullptr));
    // Calculate the chance = graph->vertexCount^(-1/k)
    double chance = pow(graph->vertexCount, -1.0 / k);
    // Iterate over the vertices
    for (int j = 0; j < graph->vertexCount; ++j) {
        // Insert the vertex into the hierarchy
        hierarchy[0]->insert({j, nullptr});
    }
    // Iterate over the hierarchy
    for (int i = 1; i < k; ++i) {
        for (auto&& j : *hierarchy[i - 1]) {
            // Insert the vertex into the hierarchy with a certain probability
            if (rand() < chance * RAND_MAX) {
                hierarchy[i]->insert(j);
            }
        }
    }
}

/**
 * Builds the random hierarchy.
 * @param heirarchySizes The size of the hierarchy.
 */
void ADO::buildRandHierarchy(int* heirarchySizes) {
    // Calculate the chance = graph->vertexCount^(-1/k)
    srand(time(nullptr));
    double chance = pow(graph->vertexCount, -1.0 / k);
    // Iterate over the vertices
    for (int j = 0; j < graph->vertexCount; ++j) {
        hierarchy[0]->insert({j, nullptr});
    }
    // Iterate over the hierarchy
    for (int i = 1; i < k; ++i) {
        // add heirarchySizes[i] vertices to the hierarchy
        do {
            auto iter = hierarchy[i - 1]->begin();
            std::advance(iter, rand() % hierarchy[i - 1]->size());
            hierarchy[i]->insert(*iter);
        } while (hierarchy[i]->size() != heirarchySizes[i]);
    }
}

/**
 * Builds the PS array.
 */
void ADO::buildPS() {
    // Initialize the Fibonacci queue
    FibQueue<distance, vertex>* fibQueue = new FibQueue<distance, vertex>();
    // Iterate over the vertices
    for (int j = 0; j < graph->vertexCount; ++j) {
        // Initialize the PS array
        for (int i = 1; i < k + 1; ++i) {
            ps[j][i] = { -1, numeric_limits<distance>::max() };
        }
        ps[j][0] = { j, 0.0 };
    }
    // Iterate over the hierarchy
    for (int i = k - 1; i > 0; --i) {
        // Iterate over the vertices
        for (auto&& j : *hierarchy[i]) {
            // Push the vertex into the Fibonacci queue
            fibQueue->push(0.0, j.first);
            ps[j.first][i] = {j.first, 0.0};
        }
        // Iterate over the vertices
        while (!fibQueue->empty()) {
            // Pop the vertex from the Fibonacci queue
            auto v = fibQueue->pop();
            auto p = ps[v][i];
            // Iterate over the edges of the vertex
            for (auto&& j : graph->getEdges(v)) {
                // Calculate the distance
                distance alt = p.second + j.second;
                // If the distance is less than the current distance, update the distance
                if (alt < ps[j.first][i].second) {
                    ps[j.first][i] = {p.first, alt};
                    fibQueue->decrease_key_or_push(alt, j.first);
                }
            }
        }
        // Iterate over the vertices
        for (auto p = ps; p < ps + graph->vertexCount; ++p) {
            if ((*p)[i].first == (*p)[i + 1].first) {
                (*p)[i] = (*p)[i + 1];
            }
        }
        // Clear the Fibonacci queue
        fibQueue->clear();
    }
    // Clean up the memory
    delete fibQueue;
}

/**
 * Builds the cluster.
 * @param i The index of the cluster.
 * @param q The pair of the vertex and the distance.
 */
void ADO::buildCluster(int i, pair<const vertex, unordered_map<vertex, distance>*>* q) {
    // If the cluster is the last cluster or the next cluster does not contain the vertex, build the cluster
    if ((i == k - 1 || hierarchy[i + 1]->count(q->first) == 0) && (i != (k - 1) / 2 || isClassic)) {
        // Initialize the cluster
        q->second = new unordered_map<vertex, distance>();
        graph->dijkstra(q->first, [this, i](vertex v, distance d) {
            return d < ps[v][i + 1].second;
        }, [this, i](vertex v, distance d) {
            return true;
        }, [this, q](vertex v, distance d) {
            q->second->insert_or_assign(v, d);
        });
    } else if (i == (k - 1) / 2 && !isClassic) {
        // Otherwise, build the cluster
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
/**
 * Builds the clusters.
 */
void ADO::buildClusters() {
    // Iterate over the hierarchy
    for (int i = k - 1; i >= 0; --i) {
        // Get the size of the hierarchy
        int size = hierarchy[i]->size();
        // Iterate over the hierarchy
        #pragma omp parallel for
        for (int j = 0; j < size; ++j) {
            auto iter = hierarchy[i]->begin();
            std::advance(iter, j);
            buildCluster(i, &(*iter));
        }
        // Notify the user that the cluster has been built
        std::cout << "Cluster " << i << " built" << std::endl;
        std::cout << "Heirarchy size: " << hierarchy[i]->size() << std::endl;
    }
}
/**
 * Builds the bunches.
 */
void ADO::buildBunches() {
    // Iterate over the hierarchy
    for (auto p = hierarchy; p < hierarchy + k; ++p) {
        // Iterate over the hierarchy
        for (auto&& q : **p) {
            // If the cluster is null, continue
            if (q.second == nullptr) {
                continue;
            }
            // Iterate over the edges of the cluster
            for (auto&& r : *q.second) {
                // Insert the edge into the bunches
                bunches[r.first]->insert_or_assign(q.first, r.second);
            }
        }
    }
}