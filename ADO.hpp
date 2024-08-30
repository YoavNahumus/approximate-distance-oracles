#ifndef OUR_ADO_HPP
#define OUR_ADO_HPP

#include "libs/graphs/graph.h"
#include <map>
#include <functional>
#include <set>

using std::unordered_map;
using std::pair;
using std::function;
using std::set;

class ADO {
    public:
    Graph* graph;
    unordered_map<vertex, unordered_map<vertex, distance>*>** hierarchy;
    unordered_map<vertex, distance>** bunches;
    pair<vertex, distance>** ps;
    const int k;
    const bool isClassic;
    bool reduced;

    void buildCluster(int i, pair<const vertex, unordered_map<vertex, distance>*>* q);

    void buildHierarchy();
    void buildRandHierarchy();
    void buildRandHierarchy(int* heirarchySizes);
    void buildPS();
    void buildClusters();
    void buildBunches();
    template<class T>
    set<T>* hittingSet(set<T>** sets, int setCount);

    public:
    ADO(Graph* graph, int k, bool isClassic);
    ~ADO();

    void preprocess();
    void preprocess(int* heirarchySizes);
    distance query(vertex vertex1, vertex vertex2);
    distance asymetricQuery(vertex vertex1, vertex vertex2);
};

#endif