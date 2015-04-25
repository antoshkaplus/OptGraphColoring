//
//  main.cpp
//  TravelingSalesman
//
//  Created by Anton Logunov on 6/22/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <stack>
#include <unordered_set>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <functional>

#include "ant/core.h"

#include "tsp_nearest_neighbor.hpp"
#include "tsp_shortest_edge.hpp"
#include "tsp_nearest_insertion.hpp"
#include "tsp_farthest_insertion.hpp"
#include "tsp_random_insertion.hpp"
#include "tsp_ant_colony.hpp"
#include "tsp_bb_cities.hpp"

using namespace std;

void printEdges(ostream& out, const vector<TSP::Edge>& edges) {
  for (int i = 0; i < edges.size(); i++) {
    out << edges[i][0] << " " << edges[i][1] << "\n";
  }
} 

enum class Method : int {
    ANT_COLONY,
    NEAREST_NEIGHBOR,
    SHORTEST_EDGE,
    NEAREST_INSERTION,
    FARTHEST_INSERTION,
    RANDOM_INSERTION
};



// you want read from file to be able to log
// in console 

// arguments for command line:
// 1) -i input.txt
// 2) -m method (ant, short_edge, near_neighbor, 
//      near_insert, far_insert, rand_insert)
// 3) -o output.txt
int main(int argc, const char * argv[])
{
    map<string, Method> methods = {
        {"ant", Method::ANT_COLONY},
        {"short_edge", Method::SHORTEST_EDGE},
        {"near_neighbor", Method::NEAREST_NEIGHBOR},
        {"near_insert", Method::NEAREST_INSERTION},
        {"far_insert", Method::FARTHEST_INSERTION},
        {"rand_insert", Method::RANDOM_INSERTION}
    };

    ant::command_line_parser parser(argv, argc);
    string m = parser.getValue("m");
    string i = parser.getValue("i");
    string o = parser.getValue("o");
    ifstream in(i);
    ofstream out(o);
    
    size_t cityCount;
    in >> cityCount;
    vector<Point> points(cityCount);
    for (int i = 0; i < cityCount; i++) {
        in >> points[i].x >> points[i].y;
    }
    
    TSP_Solver *solver;
    switch (methods[m]) {
        case Method::ANT_COLONY:
            solver = new TSP_AntColony();
            break;
        case Method::SHORTEST_EDGE:
            solver = new TSP_ShortestEdge();
            break;
        case Method::NEAREST_NEIGHBOR:
            solver = new TSP_NearestNeighbor();
            break;
        case Method::NEAREST_INSERTION:
            solver = new TSP_NearestInsertion();
            break;
        case Method::FARTHEST_INSERTION:
            solver = new TSP_FarthestInsertion();
            break;
        case Method::RANDOM_INSERTION:
            solver = new TSP_RandomInsertion();
            break;
        default:
            throw invalid_argument("method is unknown");
    }
    
    vector<City> cities = solver->solve(points);
    
    // feasibility check
    vector<bool> visited(cities.size(), false);
    for (City c : cities) {
        if (c < 0 || c >= cities.size()) {
            throw range_error("city is out of range");
        }
        if (visited[c]) {
            throw logic_error("should not visit one city twice");
        }
        visited[c] = true;
    }
    
    TSP_ThreeOpt improver;
    improver.swapEpsilon = 0.1;
    cities = improver.improve(points, cities);
    
    double perimeter = Perimeter(points, cities, true);
    out << perimeter << " " << endl;
    for (City c : cities) {
        out << c << " "; 
    }
    cout << "perimeter: " << perimeter << endl;
    cout.flush();
    return 0;
}

