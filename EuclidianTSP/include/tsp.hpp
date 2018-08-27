#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <stack>
#include <chrono>

#include "tsp_util.hpp"

using City = Index;

struct Edge : array<City, 2> {
    Edge() {}
    Edge(const array<City, 2>& arr) : array<City, 2>(arr) {}
    Edge(City c_0, City c_1) : array<City, 2>({c_0, c_1}) {}
    Edge(initializer_list<City> list) : array<City, 2>({*list.begin(), *(list.begin()+1)}) {}

    bool hasCity(City c) const {
        return at(0) == c || at(1) == c;
    }

    City otherCity(City c) const {
        return at(0) == c ? at(1) : at(0);
    }
};

struct TSP {
    typedef double Distance;
    typedef int Count;

    struct DisjointSet {
        struct Record {
            Record(Index indexBeingChanged, bool didSizeIncrese) 
                : indexBeingChanged(indexBeingChanged), didSizeIncrese(didSizeIncrese) {}
            Index indexBeingChanged;
            bool didSizeIncrese;
        };
        
        vector<size_t> sz;
        vector<Index> pt; 
        stack<Record> records;
        
        DisjointSet(size_t count) : sz(count, 1), pt(count) {
            iota(pt.begin(), pt.end(), 0);
        }
        
        void unite(Index s_0, Index s_1) {
            Index p_0 = find(s_0);
            Index p_1 = find(s_1);
            
            if(sz[p_0] < sz[p_1]){
                swap(p_0, p_1);
            }
            // if sz[0] > sz[1]  
            pt[p_1] = p_0;
            Record r(p_1, false);
            if (sz[p_0] == sz[p_1]) {
                sz[p_0]++;
                r.didSizeIncrese = true;
            } 
            records.push(r);
        }
        
        bool isUnited(Index s_0, Index s_1) const {
            return find(s_0) == find(s_1);
        }
        
        Index find(Index p) const {
            while(p != pt[p]){
                p = pt[p];
            }
            return p;
        }
        
        Index operator[](Index i) const {
            return find(i);
        } 
        
        void undoUnite() {
            assert(!records.empty());
            auto &r = records.top();
            Index index = pt[r.indexBeingChanged];
            pt[r.indexBeingChanged] = r.indexBeingChanged;
            if (r.didSizeIncrese) sz[index]--;
            records.pop();
        }
    };

    virtual vector<City> solve(const vector<Point>& points) = 0;

    static vector<Edge> tourToEdges(const vector<City>& cities) {
        vector<Edge> edges;
        for (Index i = 0; i < cities.size(); i++) {
            edges.push_back(Edge(cities[i], cities[(i+1)%cities.size()]));   
        }
        return edges; 
    }
    
    static vector<City> edgesToTour(const vector<Edge>& edges) {
        vector<City> cities(edges.size());
        vector<Edge> es(edges);
        cities[0] = es[0][0];
        cities[1] = es[0][1];
        swap(es[0], es.back());
        es.pop_back();
        for (size_t i = 2; i < cities.size(); i++) {
            for (size_t e_i = 0; e_i < es.size(); e_i++) {
                bool found = false;
                const Edge &e = es[e_i];
                if (e[0] == cities[i-1]) {
                    cities[i] = e[1]; 
                    found = true;
                }
                if (e[1] == cities[i-1]) {
                    cities[i] = e[0]; 
                    found = true;
                }
                if (found) {
                    swap(es[e_i], es.back());
                    es.pop_back();
                    break;
                }
            }
        }
        return cities;
    }
    
    // returns random city in [0,bound)
    static City random(City bound) {
        default_random_engine rng(std::chrono::system_clock::now().time_since_epoch().count());
        uniform_int_distribution<City> d(0, bound-1);
        return d(rng);
    }
    
    static double random() {
        default_random_engine rng(std::chrono::system_clock::now().time_since_epoch().count());
        uniform_real_distribution<double> d(0, 1);
        return d(rng);
    }
};

typedef TSP TSP_Solver;

struct TSP_InsertionSolver : TSP_Solver {
    vector<City> startingTour(const vector<Point>& points) {
        return ConvexHull(points);
    }
};

struct TSP_Improver {
    virtual vector<City> improve(const vector<Point>& points, 
                                 const vector<City>& tour) = 0;
};