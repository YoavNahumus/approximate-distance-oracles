#ifndef OUR_GRAPH_HPP
#define OUR_GRAPH_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <utility>
#include "../fibo/fiboqueue.h"
#include <functional>

using std::unordered_map;
using std::function;

typedef int vertex;
typedef double distance;

class Graph {
private:
    unordered_map<vertex, distance>** neighborsList;
    /**
     * Loads the graph from a CSV file.
     * @param filename The name of the CSV file.
     * @param weighted A boolean value indicating whether the graph is weighted or not.
     */
    void loadFromCSV(const std::string &filename, bool weighted) {
        // Open the file
        std::ifstream inputFile(filename);
        if (!inputFile) {
            std::cerr << "Error opening file!" << std::endl;
            return;
        }
        // Read the file
        if (!weighted) {
            // If the graph is unweighted, read the file line by line
            std::string line;
            std::getline(inputFile, line); // Skip the header line if there is one

            this->vertexCount = 0;
            // Read the file to determine the number of vertices
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
                this->neighborsList = new unordered_map<vertex, distance>*[vertexCount];
                for (int i = 0; i < vertexCount; ++i) {
                    neighborsList[i] = new unordered_map<vertex, distance>();
                }
            }

            // Reopen the file to add edges
            inputFile.clear();
            inputFile.seekg(0);
            std::getline(inputFile, line); // Skip the header line again
            // Read the file to add edges
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
            // Close the file
            inputFile.close();
            return;
        }
        // If the graph is weighted, read the file line by line
        std::string line;
        std::getline(inputFile, line); // Skip the header line if there is one

        // Initialize vertexCount
        vertexCount = 0;
        // Read the file to determine the number of vertices
        while (std::getline(inputFile, line)) {
            // Parse the line
            std::istringstream iss(line);
            std::string token;
            vertex node1, node2;
            distance weight;
            // Read the first column
            if (!std::getline(iss, token, ',')) continue;
            node1 = std::stoi(token);
            // Read the second column
            if (!std::getline(iss, token, ',')) continue;
            node2 = std::stoi(token);
            // Read the third column
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
            neighborsList = new unordered_map<vertex, distance>*[vertexCount];
            for (int i = 0; i < vertexCount; ++i) {
                neighborsList[i] = new unordered_map<vertex, distance>();
            }
        }

        // Reopen the file to add edges
        inputFile.clear(); // Clear EOF flag
        inputFile.seekg(0); // Rewind file
        std::getline(inputFile, line); // Skip the header line again
        // Read the file to add edges
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string token;
            vertex node1, node2;
            distance weight;
            // Read the first column
            if (!std::getline(iss, token, ',')) continue;
            node1 = std::stoi(token);
            // Read the second column
            if (!std::getline(iss, token, ',')) continue;
            node2 = std::stoi(token);
            // Read the third column
            if (!std::getline(iss, token, ',')) continue;
            weight = std::stod(token);

            // Skip the fourth column
            std::getline(iss, token, ',');
            // Add the edge to the graph
            addEdge(node1, node2, weight); // Add the edge to the graph
        }
        // Close the file
        inputFile.close();
    }

public:
    vertex vertexCount;
    vertex edgeCount;

    /**
     * Constructor for the Graph class.
     * @param vertexCount The number of vertices in the graph.
     */
    Graph(vertex vertexCount) : vertexCount(vertexCount), edgeCount(0) {
        neighborsList = new unordered_map<vertex, distance>*[vertexCount];
        for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; *(ptr++) = new unordered_map<vertex, distance>());
    }

    /**
     * Constructor for the Graph class.
     * @param filename The name of the CSV file.
     * @param weighted A boolean value indicating whether the graph is weighted or not.
     */
    Graph(const std::string &filename, bool weighted) {
        this->loadFromCSV(filename, weighted);
    }

    /**
     * Destructor for the Graph class.
     */
    ~Graph() {
        for (auto ptr = neighborsList; ptr < neighborsList + vertexCount; delete *(ptr++));
        delete[] neighborsList;
    }

    /**
     * Prints the edges of the graph.
     */
    void printEdges() const {
        // Iterate over all vertices and their neighbors
        for (int i = 0; i < vertexCount; ++i) {
            const auto& edges = *neighborsList[i];
            for (const auto& edge : edges) {
                std::cout << "Edge from " << i << " to " << edge.first
                        << " with weight " << edge.second << std::endl;
            }
        }
    }
    /**
     * Returns the weight of the edge between two vertices.
     * @param vertex1 The first vertex.
     * @param vertex2 The second vertex.
     * @return The weight of the edge.
     */
    double getEdgeWeight(vertex vertex1, vertex vertex2) {
        return neighborsList[vertex1]->at(vertex2);
    }
    /**
     * Returns the neighbors of a vertex.
     * @param vertex The vertex.
     */
    const unordered_map<vertex, distance>& getEdges(vertex vertex) {
        return *neighborsList[vertex];
    }
    /**
     * Returns the number of neighbors of a vertex.
     * @param vertex1 The vertex.
     * @param vertex2 The vertex.
     * @return The number of neighbors of the vertex.
     */
    bool hasEdge(vertex vertex1, vertex vertex2) {
        return neighborsList[vertex1]->count(vertex2) > 0;
    }

    /**
     * Adds an edge to the graph.
     * @param vertex1 The first vertex.
     * @param vertex2 The second vertex.
     * @param weight The weight of the edge.
     */
    void addEdge(vertex vertex1, vertex vertex2, distance weight) {
        if (!hasEdge(vertex1, vertex2)) edgeCount++;
        neighborsList[vertex1]->insert_or_assign(vertex2, weight);
        neighborsList[vertex2]->insert_or_assign(vertex1, weight);
    }

    /**
     * Dijkstra's algorithm for finding the shortest path from a vertex to all other vertices in the graph.
     * @param origin The origin vertex.
     * @param shouldCheck A function that determines whether a vertex should be checked.
     * @param shouldInsert A function that determines whether a vertex should be inserted.
     * @param insertDistance A function that inserts the distance of a vertex.
     */
    void dijkstra(vertex origin, function<bool(vertex, distance)> shouldCheck, function<bool(vertex, distance)> shouldInsert, function<void(vertex, distance)> insertDistance) {
        // Initialize the Fibonacci heap and the distances map
        FibQueue<distance, vertex>* fibQueue = new FibQueue<distance, vertex>();
        fibQueue->push(0.0, origin);
        // Initialize the distances map with the origin vertex
        unordered_map<vertex, distance>* distances = new unordered_map<vertex, distance>();
        distances->insert_or_assign(origin, 0.0);
        // Main loop
        while (!fibQueue->empty()) {
            // Pop the vertex with the smallest distance from the Fibonacci heap
            auto v = fibQueue->pop();
            auto p = distances->at(v);
            // Insert the vertex and its distance into the result set
            for (auto&& e : getEdges(v)) {
                distance alt = p + e.second;
                // If the vertex has not been visited yet or the new distance is smaller than the previous one, update the distance
                if (shouldCheck(e.first, alt) && (distances->count(e.first) == 0 || alt < distances->at(e.first))) {
                    fibQueue->decrease_key_or_push(alt, e.first);
                    distances->insert_or_assign(e.first, alt);
                }
            }
        }

        // Insert the distances into the result set
        for (auto&& p : *distances) {
            if (shouldInsert(p.first, p.second)) {
                insertDistance(p.first, p.second);
            }
        }

        // Clean up
        delete distances;
        delete fibQueue;
    }

    /**
     * Dijkstra's algorithm for finding the shortest path from a vertex to all other vertices in the graph.
     * @param origin The origin vertex.
     * @param maxCount The maximum number of vertices to visit.
     * @param insertDistance A function that inserts the distance of a vertex.
     */
    void dijkstra(vertex origin, vertex maxCount, function<void(vertex, distance)> insertDistance) {
        // Initialize the Fibonacci heap and the distances map
        FibQueue<distance, vertex>* fibQueue = new FibQueue<distance, vertex>();
        fibQueue->push(0.0, origin);
        // Initialize the distances map with the origin vertex
        unordered_map<vertex, distance>* distances = new unordered_map<vertex, distance>();
        distances->insert_or_assign(origin, 0.0);
        int count = 0;
        // Main loop
        while (!fibQueue->empty() && count++ < maxCount) {
            // Pop the vertex with the smallest distance from the Fibonacci heap
            auto v = fibQueue->pop();
            auto p = distances->at(v);
            insertDistance(v, p);
            // Insert the vertex and its distance into the result set
            for (auto&& e : getEdges(v)) {
                distance alt = p + e.second;
                if (distances->count(e.first) == 0 || alt < distances->at(e.first)) {
                    fibQueue->decrease_key_or_push(alt, e.first);
                    distances->insert_or_assign(e.first, alt);
                }
            }
        }

        // Clean up
        delete distances;
        delete fibQueue;
    }
    
    /**
     * Reduces the graph to a subgraph with a given maximum number of vertices.
     * @param maxCount The maximum number of vertices in the subgraph.
     * @param origin The origin vertex for the Dijkstra's algorithm.
     * @return The reduced graph.
     */
    Graph* reduceGraph(vertex maxCount, vertex origin = 0) {
        // Create a mapping from the original vertices to the new vertices
        unordered_map<vertex, vertex>* mapping = new unordered_map<vertex, vertex>();
        dijkstra(origin, maxCount, [mapping](vertex v, distance d) {
            mapping->insert({v, mapping->size()});
        });

        // Create the new graph
        Graph* newGraph = new Graph(maxCount);
        std::cout << mapping->size() << std::endl;
        for (auto&& p : *mapping) {
            for (auto&& q : getEdges(p.first)) {
                if (mapping->count(q.first) > 0) {
                    newGraph->addEdge(p.second, mapping->at(q.first), q.second);
                }
            }
        }

        // Clean up
        delete mapping;

        return newGraph;
    }
};

#endif