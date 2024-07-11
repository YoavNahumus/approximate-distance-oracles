#include "ADO.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
    Graph* graph = new Graph(10);
    graph->addEdge(0, 1, 1);
    graph->addEdge(0, 2, 2);
    graph->addEdge(0, 3, 3);
    graph->addEdge(1, 2, 4);
    graph->addEdge(1, 3, 5);
    graph->addEdge(2, 3, 6);
    graph->addEdge(2, 4, 7);
    graph->addEdge(3, 4, 8);
    graph->addEdge(3, 5, 9);
    graph->addEdge(4, 5, 10);
    graph->addEdge(5, 6, 1);
    graph->addEdge(5, 7, 2);
    graph->addEdge(5, 8, 3);
    graph->addEdge(6, 7, 4);
    graph->addEdge(6, 8, 5);
    graph->addEdge(7, 8, 6);
    graph->addEdge(7, 9, 7);
    graph->addEdge(8, 9, 8);
    ADO* ado = new ADO(graph, 3, false);
    ado->preprocess(true);

    for (int i = 0; i < graph->vertexCount; i++) {
        for (int j = 0; j < graph->vertexCount; j++) {
            std::cout << "Distance from " << i << " to " << j << " is " << ado->query(i, j) << std::endl;
        }
    }

    delete ado;
    delete graph;

    return 0;
}
