#include "ADO.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
    
    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " <graph file> <K> <vertex count> <test count>" << std::endl;
        return 1;
    }

    int k = atoi(argv[2]);
    vertex vertexCount = atoi(argv[3]);
    long tests = atol(argv[4]);
    

    Graph* g = new Graph(argv[1], false);
    Graph* temp = g->reduceGraph(vertexCount);
    delete g;
    g = temp;
    std::cout << "The Graph has " << g->vertexCount << " vertices and " << g->edgeCount << " edges" << std::endl;
    
    ADO* classic = new ADO(g, k, false, false);
    ADO* edgeSetOnly = new ADO(g, k, true, false);
    ADO* middleLevelOnly = new ADO(g, k, false, true);
    ADO* allNew = new ADO(g, k, true, true);


    allNew->preprocess();
    edgeSetOnly->preprocess();
    int sizes[k];
    for (int i = 0; i < k; i++) {
        sizes[i] = allNew->hierarchy[i]->size();
    }
    middleLevelOnly->preprocess(sizes);
    classic->preprocess(sizes);
    
    distance sumClassic = 0;
    distance sumEdgeSetOnly = 0;
    distance sumMiddleLevelOnly = 0;
    distance sumAllNew = 0;
    distance worstClassic = 0;
    distance worstEdgeSetOnly = 0;
    distance worstMiddleLevelOnly = 0;
    distance worstAllNew = 0;
    distance largestDiffPosAllNew = 0;
    distance largestDiffNegAllNew = 0;
    distance largestDiffPosEdgeSetOnly = 0;
    distance largestDiffNegEdgeSetOnly = 0;
    distance largestDiffPosMiddleLevelOnly = 0;
    distance largestDiffNegMiddleLevelOnly = 0;
    ulong countWorseAllNew = 0;
    ulong countWorseEdgeSetOnly = 0;
    ulong countWorseMiddleLevelOnly = 0;
    for (long i = 0; i < tests; i++) {
        vertex v1 = rand() % g->vertexCount;
        vertex v2 = rand() % g->vertexCount;
        distance dClassic = classic->query(v1, v2);
        distance dEdgeSetOnly = edgeSetOnly->query(v1, v2);
        distance dMiddleLevelOnly = middleLevelOnly->query(v1, v2);
        distance dAllNew = allNew->query(v1, v2);
        if (dClassic > worstClassic) {
            worstClassic = dClassic;
        }
        if (dEdgeSetOnly > worstEdgeSetOnly) {
            worstEdgeSetOnly = dEdgeSetOnly;
        }
        if (dMiddleLevelOnly > worstMiddleLevelOnly) {
            worstMiddleLevelOnly = dMiddleLevelOnly;
        }
        if (dAllNew > worstAllNew) {
            worstAllNew = dAllNew;
        }

        if (dClassic < dAllNew) {
            if (dAllNew - dClassic > largestDiffNegAllNew) {
                largestDiffNegAllNew = dAllNew - dClassic;
            }
            countWorseAllNew++;
        } else if (dClassic - dAllNew > largestDiffPosAllNew) {
            largestDiffPosAllNew = dClassic - dAllNew;
        }

        if (dClassic < dEdgeSetOnly) {
            if (dEdgeSetOnly - dClassic > largestDiffNegEdgeSetOnly) {
                largestDiffNegEdgeSetOnly = dEdgeSetOnly - dClassic;
            }
            countWorseEdgeSetOnly++;
        } else if (dClassic - dEdgeSetOnly > largestDiffPosEdgeSetOnly) {
            largestDiffPosEdgeSetOnly = dClassic - dEdgeSetOnly;
        }

        if (dClassic < dMiddleLevelOnly) {
            if (dMiddleLevelOnly - dClassic > largestDiffNegMiddleLevelOnly) {
                largestDiffNegMiddleLevelOnly = dMiddleLevelOnly - dClassic;
            }
            countWorseMiddleLevelOnly++;
        } else if (dClassic - dMiddleLevelOnly > largestDiffPosMiddleLevelOnly) {
            largestDiffPosMiddleLevelOnly = dClassic - dMiddleLevelOnly;
        }
        
        sumClassic += dClassic;
        sumAllNew += dAllNew;
        sumEdgeSetOnly += dEdgeSetOnly;
        sumMiddleLevelOnly += dMiddleLevelOnly;
    }
    std::cout << "Sum Classic: " << sumClassic << std::endl;
    std::cout << "Sum Edge Set Only: " << sumEdgeSetOnly << std::endl;
    std::cout << "Sum Middle Level Only: " << sumMiddleLevelOnly << std::endl;
    std::cout << "Sum All New: " << sumAllNew << std::endl;

    std::cout << "Worst Classic: " << worstClassic << std::endl;
    std::cout << "Worst Edge Set Only: " << worstEdgeSetOnly << std::endl;
    std::cout << "Worst Middle Level Only: " << worstMiddleLevelOnly << std::endl;
    std::cout << "Worst All New: " << worstAllNew << std::endl;

    std::cout << "Average Classic: " << sumClassic / tests << std::endl;
    std::cout << "Average Edge Set Only: " << sumEdgeSetOnly / tests << std::endl;
    std::cout << "Average Middle Level Only: " << sumMiddleLevelOnly / tests << std::endl;
    std::cout << "Average All New: " << sumAllNew / tests << std::endl;

    std::cout << "Improvement Edge Set Only: " << (sumClassic - sumEdgeSetOnly) / sumClassic << std::endl;
    std::cout << "Improvement Middle Level Only: " << (sumClassic - sumMiddleLevelOnly) / sumClassic << std::endl;
    std::cout << "Improvement All New: " << (sumClassic - sumAllNew) / sumClassic << std::endl;

    std::cout << "Improved Edge Set Only: " << ((double)(tests - countWorseEdgeSetOnly) * 100) / tests << "%" << std::endl;
    std::cout << "Improved Middle Level Only: " << ((double)(tests - countWorseMiddleLevelOnly) * 100) / tests << "%" << std::endl;
    std::cout << "Improved All New: " << ((double)(tests - countWorseAllNew) * 100) / tests << "%" << std::endl;

    std::cout << "Largest diff pos Edge Set Only: " << largestDiffPosEdgeSetOnly << std::endl;
    std::cout << "Largest diff neg Edge Set Only: " << largestDiffNegEdgeSetOnly << std::endl;
    std::cout << "Largest diff pos Middle Level Only: " << largestDiffPosMiddleLevelOnly << std::endl;
    std::cout << "Largest diff neg Middle Level Only: " << largestDiffNegMiddleLevelOnly << std::endl;
    std::cout << "Largest diff pos All New: " << largestDiffPosAllNew << std::endl;
    std::cout << "Largest diff neg All New: " << largestDiffNegAllNew << std::endl;

    delete allNew;
    delete edgeSetOnly;
    delete middleLevelOnly;
    delete classic;

    delete g;

    return 0;
}
