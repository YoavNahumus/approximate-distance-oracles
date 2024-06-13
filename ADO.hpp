#ifndef OUR_ADO_HPP
#define OUR_ADO_HPP

#include "graph.hpp"
#include <map>

using std::map;
using std::pair;

class ADO {
    private:
    Graph* graph;
    map<int, map<int, double>*>** hierarchy;
    map<int, double>** bunches;
    pair<int, double>** ps;
    const int k;

    void buildHierarchy();
    void buildPS();
    void buildClusters();
    void buildBunches();

    public:
    ADO(Graph* graph, int k);
    ~ADO();

    void preprocess();
    double query(int vertex1, int vertex2);
};

#endif