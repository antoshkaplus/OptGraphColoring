//
//  TSP_BnB_Cities.h
//  TravelingSalesman
//
//  Created by Anton Logunov on 4/2/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __TravelingSalesman__TSP_BnB_Cities__
#define __TravelingSalesman__TSP_BnB_Cities__

#include <iostream>

#include "tsp.hpp"
/*
struct TSP_BnB_Cities : TSP_Solver {
    
    size_t cityCount; 
    vector<size_t> degree;
    DisjointSet groups;
    vector<Edge> edges;
    vector<array<double, 2>> cityMinDistances;
    // don't really need this
    size_t currentEdgeCount;
    double totalDistance;
    
    
    
    
    void next_1(City i) {
        for (size_t i_0 = i+1; i_0 < cityCount; i_0++) {
            if (degree[i_0] == 2 || (groups[i_0] == groups[i] && currentEdgeCount != cityCount-1)) continue;
            
            edges.push_back(Edge(i, i_0));
            currentEdgeCount++;
            groups.unite(i, i_0);
            degree[i_0]++;
            
            totalDistance -= cityMinDistances[i_0][0] + cityMinDistances[i][0];
            
            
            
            if (currentEdgeCount == cityCount) {
                
            } else {
            
            size_t i_next = i;
            while (degree[i_next] == 2) i_next++;
            if (degree[i_next] == 0) next_2(i_next);
            else next_1(i_next);     
            
            }
            edges.pop_back();
            currentEdgeCount--;
            groups.undoUnite();
            degree[i_0]--;
        }
    }
    
    void next_2(City i) {
        
        for (size_t i_0 = i+1; i_0 < cityCount; i_0++) {
            if (degree[i_0] == 2 || (groups[i_0] == groups[i] && currentEdgeCount != cityCount-1)) continue;
            
            edges.push_back(Edge(i, i_0));
            currentEdgeCount++;
            groups.unite(i, i_0);
            degree[i_0]++;
            
            for (size_t i_1 = i_0+1; i_1 < cityCount; i_1++) {
                if (degree[i_1] == 2 || (groups[i_1] == groups[i] && currentEdgeCount != cityCount-1)) continue;
                        
                edges.push_back(Edge(i, i_1));
                currentEdgeCount++;
                groups.unite(i, i_1);
                degree[i_1]++;
                
                if (currentEdgeCount == cityCount) {
                    
                    
                } else {
                
                size_t i_next = i;
                while (degree[i_next] == 2) i_next++;
                if (degree[i_next] == 0) next_2(i_next);
                else next_1(i_next);                
                
                }
                
                edges.pop_back();
                currentEdgeCount--;
                groups.undoUnite();
                degree[i_1]--;
                
            }
            
            edges.pop_back();
            currentEdgeCount--;
            groups.undoUnite();
            degree[i_0]--;
        }
    }
    
    
    
    vector<City> solve(const vector<Point>& points) {
        vector<array<double, 2>> cityMinDistances(points.size());
        for (size_t i = 0; i < points.size(); i++) {
            vector<Index> inds = kNearestPoints(points, points[i], 2);
            cityMinDistances[i][0] = points[inds[0]].distance(points[i]);
            cityMinDistances[i][1] = points[inds[1]].distance(points[i]);
        } 

        vector<Edge> edges(points.size());
        for (size_t i = 0; i < points.size(); i++) {
            
        }
        
        
        
        
            
        //return ;
    }
    
    
    
};
*/

#endif /* defined(__TravelingSalesman__TSP_BnB_Cities__) */
