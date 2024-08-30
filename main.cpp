#include "ADO.hpp"
#include <iostream>

#define TESTS 1000000

int main(int argc, char const *argv[]) {
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <graph file>" << std::endl;
        return 1;
    }

    Graph* g = new Graph(argv[1], false);
    Graph* temp = g->reduceGraph(10000);
    delete g;
    g = temp;
    std::cout << "The Graph has " << g->vertexCount << " vertices and " << g->edgeCount << " edges" << std::endl;
    
    ADO* ado = new ADO(g, 3, true);
    ADO* ado2 = new ADO(g, 3, false);
    ado2->preprocess();
    int sizes[3] = {(int)ado2->hierarchy[0]->size(), (int)ado2->hierarchy[1]->size(), (int)ado2->hierarchy[2]->size()};
    ado->preprocess(sizes);
    
    distance sum1 = 0;
    distance sum2 = 0;
    distance worst11 = 0;
    distance worst12 = 0;
    distance worst21 = 0;
    distance worst22 = 0;
    ulong count_worse = 0;
    for (int i = 0; i < TESTS; i++) {
        vertex v1 = rand() % g->vertexCount;
        vertex v2 = rand() % g->vertexCount;
        distance d1 = ado->query(v1, v2);
        distance d2 = ado2->query(v1, v2);
        if (d1 > worst11) {
            worst11 = d1;
            worst12 = d2;
        }
        if (d2 > worst21) {
            worst21 = d2;
            worst22 = d1;
        }
        if (d1 < d2) {
            count_worse++;
        }
        sum1 += d1;
        sum2 += d2;
    }
    std::cout << "Sum1: " << sum1 << std::endl;
    std::cout << "Sum2: " << sum2 << std::endl;

    std::cout << "Worst1: " << worst11 << " " << worst12 << std::endl;
    std::cout << "Worst2: " << worst21 << " " << worst22 << std::endl;

    std::cout << "Average1: " << sum1 / TESTS << std::endl;
    std::cout << "Average2: " << sum2 / TESTS << std::endl;

    std::cout << "Improvement: " << (sum1 - sum2) / sum1 << std::endl;

    std::cout << "Improved: " << ((double)(TESTS - count_worse) * 100) / TESTS << "%" << std::endl;

    delete ado2;
    delete ado;

    delete g;

    return 0;
}
