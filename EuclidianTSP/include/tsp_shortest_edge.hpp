//
//  TSP_ShortestEdge.h
//  TravelingSalesman
//
//  Created by Anton Logunov on 3/30/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __TravelingSalesman__TSP_ShortestEdge__
#define __TravelingSalesman__TSP_ShortestEdge__

#include <functional>
#include <iostream>
#include <queue>
#include <cassert>
#include <numeric>

#include "tsp.hpp"

void printEdges(ostream& out, const vector<TSP::Edge>& edges);

struct TSP_ShortestEdge : TSP {
    size_t maxCityCount = 8000;

    struct Record {
        array<City, 2> city;
        Distance dist;
        
        Record(City c_0, City c_1, Distance d)
            : city({c_0, c_1}), dist(d) {}
        
        struct Cmp {
            // "greater" cuz of max priority queue 
            bool operator()(const Record& r_0, const Record& r_1) {
                return r_0.dist > r_1.dist;
            }
        };
    };

    vector<City> solve(const vector<Point>& points) override {
        if (points.size() > maxCityCount) throw "can't solve with so many cities";
        
        priority_queue<Record, vector<Record>, Record::Cmp> pr_queue;
        for (City c_0 = 0; c_0 < points.size(); c_0++) {
            for (City c_1 = c_0+1; c_1 < points.size(); c_1++) {
                pr_queue.push(Record(
                    c_0, 
                    c_1, 
                    points[c_0].distance(points[c_1])));
            }
        }
        
        vector<Edge> edges;
        vector<size_t> degree(points.size(), 0);
        vector<size_t> group(points.size());
        iota(group.begin(), group.end(), 0);
        
        size_t totalDegree = 0;
        while (totalDegree < 2*points.size()) {
            assert(pr_queue.size() > 0);
            Record r = pr_queue.top();
            pr_queue.pop();
            if (degree[r.city[0]] < 2 && degree[r.city[1]] < 2 && 
                (group[r.city[0]] != group[r.city[1]] || totalDegree == 2*points.size()-2)) {

                edges.push_back(Edge(r.city));
                // merge two groups
                size_t gr_0 = group[r.city[0]],
                       gr_1 = group[r.city[1]];
                for (size_t i = 0; i < points.size(); i++) {
                    if (group[i] == gr_1) group[i] = gr_0;
                } 
                degree[r.city[0]]++;
                degree[r.city[1]]++;
                totalDegree += 2;
            }
        }
        return edgesToTour(edges);
    }
};

#endif /* defined(__TravelingSalesman__TSP_ShortestEdge__) */
