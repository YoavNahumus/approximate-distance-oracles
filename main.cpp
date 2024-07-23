#include "ADO.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
    
    Graph* g = new Graph("roadNet-PA.csv", false);
    std::cout << "The Graph has " << g->vertexCount << " vertices and " << g->edgeCount << " edges" << std::endl;
    ADO* ado = new ADO(g, 3, true);
    ado->preprocess();
    std::cout << ado->query(0, 1) << std::endl;

    delete ado;

    delete g;

    return 0;
}
