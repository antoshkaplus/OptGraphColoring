//
//  GraphColoring.cpp
//  GraphColoring
//
//  Created by Anton Logunov on 6/25/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include "ColoredGraph.h"


// should be in cpp file as static method unfortunately 
AdjacencyList Graph::edgesToAdjacencyList(const vector<Edge>& edges, size_t nodeCount) {
    AdjacencyList adjList(nodeCount);
    for (const Edge& p : edges) {
        adjList[p.first].push_back(p.second);
        adjList[p.second].push_back(p.first);
    }
    return adjList;
}
