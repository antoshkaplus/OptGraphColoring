//
//  TSP_ConvexHull.h
//  TravelingSalesman
//
//  Created by Anton Logunov on 3/29/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __TravelingSalesman__TSP_ConvexHull__
#define __TravelingSalesman__TSP_ConvexHull__

#include <iostream>

#include "tsp.hpp"

struct TSP_ConvexHull : TSP {
    vector<City> solve(const vector<Point>& cities) override {
        
        
        struct P {
            double distance;
            Edge edge;
        };
        
        
        // need to iterate over edges. 
        // should be able to have polygon type
        double distance_3(int k, int i, int j) {
            return 0;
        }
        
        double dd(int i, int j) {
            return 1;
        }
        
        double dd(Edge e) {
            return 1;
        }
        
        vector<Index> ConvexHullHamiltonCycle(const vector<Point>& points) {
            //Matrix
            
            
            //better to compute distance for each vertex
            
            auto hull = ConvexHull(points);
            CircularList<Index> free;
            // better create edges out of it
            vector<Edge> edges;
            // can use iterators for manipulation
            
            map<Index, P> table;
            // would be nice to have circular lists here
            Index f;
            for (Index k = 0; k < free.size(); ++k) {
                // can get iterator on f
                for (Index i = 0; i < hull.size(); ++i) {
                    // can optimize this one
                    Index j = (i+1)%hull.size();
                    if (table[f].distance > distance_3(f, i, j)) {
                        table[f].distance = distance_3(f, i, j);
                        table[f].edge = Edge{i, j};
                    }
                }
            }
            // now i have everything set.
            while (table.empty()) {
                pair<Index, P> m;
                double m_D;
                for (Index k = 0; k < table.size(); ++k) {
                    pair<Index, P> p; // current from table
                    double D = dd(p.second.edge.first, p.first) + dd(p.second.edge.second, p.first) 
                    / dd(p.second.edge);
                    if (D < m_D) {
                        m = p;
                        m_D = D;
                    }
                }
                table.erase(m.first);
                // insert into...
                // create edge i, k and j, k, delete edge i, j
                // should we have a set of edges or list of vertices
                int i, j, k;
                for (auto& p : table) {
                    // trying to update with new edges
                    if (distance_3(p.first, i, k) < p.second.distance) {
                        p.second.edge = Edge{i, k};
                        p.second.distance = distance_3(p.first, i, k);
                    }
                    if (distance_3(p.first, j, k) < p.second.distance) {
                        p.second.edge = Edge{j, k};
                        p.second.distance = distance_3(p.first, j, k);
                    }
                    if (p.second.edge == Edge{i, j}) {
                        // find new edge
                        // very big number
                        p.second.distance = 3204256356102;
                        for (auto e : edges) {
                            if (distance_3(p.first, e.first, e.second) < p.second.distance) {
                                p.second.edge = e;
                                p.second.distance = distance_3(p.first, e.first, e.second);
                            }
                        }
                    }
                    
                }
            }
            // enjoy your life with what you have
            // Edges to HamiltonCycle
            return;
        }
        
        
        

        
        
        
    }
};



#endif /* defined(__TravelingSalesman__TSP_ConvexHull__) */
