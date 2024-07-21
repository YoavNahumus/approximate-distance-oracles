#include "graph.hpp"

Graph::Graph(vertex vertexCount) : vertexCount(vertexCount), edgeCount(0) {
    neighborsList = new map<vertex, distance>*[vertexCount];
    for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; *(ptr++) = new map<vertex, distance>());
}

Graph::Graph(const std::string &filename, int vertexCount, bool weighted) : vertexCount(vertexCount) {
    this->loadFromCSV(filename, vertexCount, weighted);
}

void Graph::loadFromCSV(const std::string &filename, int vertexCount, bool weighted) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    if (!weighted) {
        std::string line;
        std::getline(inputFile, line); // Skip the header line if there is one

        vertexCount = 0;

        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string token;
            vertex node1, node2;

            if (!std::getline(iss, token, ',')) continue;
            node1 = std::stoi(token);

            if (!std::getline(iss, token, ',')) continue;
            node2 = std::stoi(token);

            // Update vertexCount if needed
            if (node1 >= vertexCount) {
                vertexCount = node1 + 1;
            }
            if (node2 >= vertexCount) {
                vertexCount = node2 + 1;
            }
        }

        // Initialize neighborsList
        if (vertexCount > 0) {
            this->neighborsList = new map<vertex, distance>*[vertexCount];
            for (int i = 0; i < vertexCount; ++i) {
                neighborsList[i] = new map<vertex, distance>();
            }
        }

        // Reopen the file to add edges
        inputFile.clear();
        inputFile.seekg(0);
        std::getline(inputFile, line); // Skip the header line again

        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string token;
            vertex node1, node2;

            if (!std::getline(iss, token, ',')) continue;
            node1 = std::stoi(token);

            if (!std::getline(iss, token, ',')) continue;
            node2 = std::stoi(token);

            addEdge(node1, node2, 1);
        }

        inputFile.close();
        return;
    }

    std::string line;
    std::getline(inputFile, line); // Skip the header line if there is one

    // Initialize vertexCount
    vertexCount = 0;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string token;
        vertex node1, node2;
        distance weight;

        if (!std::getline(iss, token, ',')) continue;
        node1 = std::stoi(token);

        if (!std::getline(iss, token, ',')) continue;
        node2 = std::stoi(token);

        if (!std::getline(iss, token, ',')) continue;
        weight = std::stod(token);

        // Skip the fourth column
        std::getline(iss, token, ',');

        // Update vertexCount if needed
        if (node1 >= vertexCount) {
            vertexCount = node1 + 1;
        }
        if (node2 >= vertexCount) {
            vertexCount = node2 + 1;
        }
    }

    // Initialize neighborsList with the updated vertexCount
    if (vertexCount > 0) {
        neighborsList = new map<vertex, distance>*[vertexCount];
        for (int i = 0; i < vertexCount; ++i) {
            neighborsList[i] = new map<vertex, distance>();
        }
    }

    // Reopen the file to add edges
    inputFile.clear(); // Clear EOF flag
    inputFile.seekg(0); // Rewind file
    std::getline(inputFile, line); // Skip the header line again

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string token;
        vertex node1, node2;
        distance weight;

        if (!std::getline(iss, token, ',')) continue;
        node1 = std::stoi(token);

        if (!std::getline(iss, token, ',')) continue;
        node2 = std::stoi(token);

        if (!std::getline(iss, token, ',')) continue;
        weight = std::stod(token);

        // Skip the fourth column
        std::getline(iss, token, ',');

        addEdge(node1, node2, weight); // Add the edge to the graph
    }

    inputFile.close();
}

Graph::~Graph() {
    for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; delete *(ptr++));
    delete[] neighborsList;
}

double Graph::getEdgeWeight(vertex vertex1, vertex vertex2) {
    return neighborsList[vertex1]->at(vertex2);
}

const map<vertex, distance>& Graph::getEdges(vertex vertex) {
    return *neighborsList[vertex];
}

void Graph::addEdge(vertex vertex1, vertex vertex2, distance weight) {
    neighborsList[vertex1]->insert_or_assign(vertex2, weight);
    neighborsList[vertex2]->insert_or_assign(vertex1, weight);
    edgeCount++;
}

bool Graph::hasEdge(vertex vertex1, vertex vertex2) {
    return neighborsList[vertex1]->count(vertex2) > 0;
}