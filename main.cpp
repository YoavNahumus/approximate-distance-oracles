#include "ADO.hpp"
#include <iostream>

#define TESTS 1000000
#define K 3

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
    
    ADO* ado = new ADO(g, K, true);
    ADO* ado2 = new ADO(g, K, false);
    ado2->preprocess();
    int sizes[K];
    for (int i = 0; i < K; i++) {
        sizes[i] = ado2->hierarchy[i]->size();
    }
    ado->preprocess(sizes);
    
    distance sum1 = 0;
    distance sum2 = 0;
    distance worst1 = 0;
    distance worst2 = 0;
    distance largest_diff_pos = 0;
    distance largest_diff_neg = 0;
    ulong count_worse = 0;
    for (int i = 0; i < TESTS; i++) {
        vertex v1 = rand() % g->vertexCount;
        vertex v2 = rand() % g->vertexCount;
        distance d1 = ado->query(v1, v2);
        distance d2 = ado2->query(v1, v2);
        if (d1 > worst1) {
            worst1 = d1;
        }
        if (d2 > worst2) {
            worst2 = d1;
        }
        if (d1 < d2) {
            if (d2 - d1 > largest_diff_neg) {
                largest_diff_neg = d2 - d1;
            }
            count_worse++;
        } else if (d1 - d2 > largest_diff_pos) {
            largest_diff_pos = d1 - d2;
        }
        sum1 += d1;
        sum2 += d2;
    }
    std::cout << "Sum1: " << sum1 << std::endl;
    std::cout << "Sum2: " << sum2 << std::endl;

    std::cout << "Worst1: " << worst1 << std::endl;
    std::cout << "Worst2: " << worst2 << std::endl;

    std::cout << "Average1: " << sum1 / TESTS << std::endl;
    std::cout << "Average2: " << sum2 / TESTS << std::endl;

    std::cout << "Improvement: " << (sum1 - sum2) / sum1 << std::endl;

    std::cout << "Improved: " << ((double)(TESTS - count_worse) * 100) / TESTS << "%" << std::endl;

    std::cout << "Largest diff pos: " << largest_diff_pos << std::endl;
    std::cout << "Largest diff neg: " << largest_diff_neg << std::endl;

    delete ado2;
    delete ado;

    delete g;

    return 0;
}
