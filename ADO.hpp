#ifndef OUR_ADO_HPP
#define OUR_ADO_HPP

#include "graph.hpp"
#include <map>

using std::map;
using std::pair;

class ADO {
    public:
    Graph* graph;
    map<vertex, map<vertex, distance>*>** hierarchy;
    map<vertex, distance>** bunches;
    pair<vertex, distance>** ps;
    const int k;
    const bool isClassic;

    void buildCluster(int i, pair<const vertex, map<vertex, distance>*>* q);

    void buildHierarchy();
    void buildRandHierarchy();
    void buildPS();
    void buildClusters();
    void buildBunches();

    public:
    ADO(Graph* graph, int k, bool isClassic);
    ~ADO();

    void preprocess();
    void preprocess(bool random);
    distance query(vertex vertex1, vertex vertex2);
    distance asymetricQuery(vertex vertex1, vertex vertex2);
};

#endif