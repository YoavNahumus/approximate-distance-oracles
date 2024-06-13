#ifndef OUR_ADO_HPP
#define OUR_ADO_HPP

#include "graph.hpp"
#include <map>

using std::map;
using std::pair;

class ADO {
    private:
    Graph* graph;
    map<vertex, map<vertex, distance>*>** hierarchy;
    map<vertex, distance>** bunches;
    pair<vertex, distance>** ps;
    const int k;

    void buildCluster(int i, pair<vertex, map<vertex, distance>*> q);

    void buildHierarchy();
    void buildPS();
    void buildClusters();
    void buildBunches();

    public:
    ADO(Graph* graph, int k);
    ~ADO();

    void preprocess();
    double query(vertex vertex1, vertex vertex2);
};

#endif