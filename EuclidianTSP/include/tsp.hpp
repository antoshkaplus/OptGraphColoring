#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <stack>
#include <chrono>

#include "tsp_util.hpp"

typedef int City;

using namespace std;

struct TSP {
    typedef double Distance;
    typedef int Count;
    
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

struct TSP_TwoOpt : TSP_Improver {
    typedef size_t Index;
    
    double swapEpsilon;

    TSP_TwoOpt() : swapEpsilon(1e-7) {}
    
    TSP_TwoOpt(double swap_epsilon) {
        swapEpsilon = swap_epsilon;
    }
    
    vector<City> improve(const vector<Point>& ps,
                         const vector<City>& in_tour) override {
        vector<City> tour = in_tour;
        
        #define next(i, t) (((i)+1)%(t).size())
        #define distance(i_0, i_1) (ps[tour[i_0]].Distance(ps[tour[i_1]]))
//        function<Index(Index)> next = [&](Index i) { 
//            return (i+1)%tour.size(); 
//        };
        // you need to store those distances
//        function<double(Index, Index)> distance = [&](Index i0, Index i1) {
//            return ps[tour[i0]].distance(ps[tour[i1]]);
//        };
//        
        Index reverseCount = 0, printedReverseCount = 0;
        bool start_again;
        do {
            start_again = false;
            Index a1 = 0, a2, b1, b2;
            while ((a2 = next(a1, tour)) != tour.size()-1) { 
                b1 = next(a2, tour);
                b2 = next(b1, tour);
                while (b1 != 0) {
                    if (distance(a1, a2)+distance(b1, b2) > distance(a1, b1) + distance(a2, b2) + swapEpsilon) {
                        reverse(tour.begin()+a2, tour.begin()+b1+1);
                        reverseCount++;
                        start_again = true;
                    }
                    b1 = b2;
                    b2 = next(b1, tour);
                }
                a1 = a2;
            }
            if (reverseCount > printedReverseCount+1000) {
                cout << "reverce count: " << reverseCount << " obj: " << Perimeter(ps, tour, true)  << endl;
                printedReverseCount = reverseCount;
            }
        } while (start_again);
        #undef next
        #undef distance
        return tour;
    }
}; 

struct TSP_TwoOptAndHalf : TSP_Improver {
    typedef size_t Index;
    
    double swapEpsilon;
    
    TSP_TwoOptAndHalf() : swapEpsilon(1e-7) {}
    
    vector<City> improve(const vector<Point>& ps,
                         const vector<City>& in_tour) override {
        vector<City> tour = in_tour;
        function<Index(Index)> next = [&](Index i) { 
            return (i+1)%tour.size(); 
        };
        function<double(Index, Index)> distance = [&](Index i0, Index i1) {
            return ps[tour[i0]].Distance(ps[tour[i1]]);
        };
        
        Index reverseCount = 0, printedReverseCount = 0;
        bool start_again;
        do {
            start_again = false;
            Index a0, a1 = 0, a2, b1, b2;
            while ((a2 = next(a1)) != tour.size()-1) { 
                b1 = next(a2);
                b2 = next(b1);
                while (b1 != 0) {
                    if (distance(a1, a2) + distance(b1, b2) > distance(a1, b1) + distance(a2, b2) + swapEpsilon) {
                        reverse(tour.begin()+a2, tour.begin()+b1+1);
                        reverseCount++;
                        start_again = true;
                    }
                    else {
                        a0 = (a1 + tour.size() - 1)%tour.size();
                        if (distance(a0, a2) + distance(b1, a1) + distance(a1, b2) < 
                            distance(a0, a1) + distance(a1, a2) + distance(b1, b2) - swapEpsilon) {
                            //cout << "before: " << perimeter(ps, tour, true);
                            City c = tour[a1];
                            tour.erase(tour.begin() + a1);
                            // cuz of erase shift
                            tour.insert(tour.begin() + b2-1, c);
                            //cout << " after: " << perimeter(ps, tour, true);
                        }
                    } 
                    b1 = b2;
                    b2 = next(b1);
                }
                a1 = a2;
            }
            if (reverseCount > printedReverseCount+1000) {
                cout << "reverce count: " << reverseCount << " obj: " << Perimeter(ps, tour, true)  << endl;
                printedReverseCount = reverseCount;
            }
        } while (start_again);
        return tour;
    }

};

struct TSP_ThreeOpt : TSP_Improver {
    
    double swapEpsilon;
    
    TSP_ThreeOpt() : swapEpsilon(1e-7) {}
    
    TSP_ThreeOpt(double swap_epsilon) {
        swapEpsilon = swap_epsilon;
    }
    
    vector<City> improve(const vector<Point>& ps,
                         const vector<City>& in_tour) override {
        vector<City> tour = in_tour;
        function<Index(Index)> next = [&](Index i) { 
            return (i+1)%tour.size(); 
        };
        function<double(Index, Index)> distance = [&](Index i0, Index i1) {
            return ps[tour[i0]].Distance(ps[tour[i1]]);
        };
        bool start_again;
        do {
            start_again = false;
            Index a1 = 0, a2, b1, b2, c1, c2;
            while ((a2 = a1+1) != tour.size()-1-1) {
                b1 = a2;
                while ((b2 = b1+1) != tour.size()-1) {
                    c1 = b2;
                    // want to capture first-last edge
                    while ((c2 = (c1+1)%tour.size()) != 1) {
                        bool did_transform = true;
                        // 4 possible permutations
                        double d_a = distance(a1, a2),
                               d_b = distance(b1, b2),
                               d_c = distance(c1, c2),
                               total = d_a + d_b + d_c;
                        if (total > distance(a1, c1) + distance(a2, b2) + distance(b1, c2) + swapEpsilon) {
                            reverse(tour.begin()+a2, tour.begin()+b1+1);
                            reverse(tour.begin()+a2, tour.begin()+c1+1);
                            //cout << 0 << endl;
                        } else
                        if (total > distance(a1, b2) + distance(a2, c2) + distance(b1, c1) + swapEpsilon) {
                            reverse(tour.begin()+b2, tour.begin()+c1+1);
                            reverse(tour.begin()+a2, tour.begin()+c1+1);
                            //cout << 1 << endl;
                        } else
                        if (total > distance(a1, b2) + distance(a2, c1) + distance(b1, c2) + swapEpsilon) {
                            reverse(tour.begin()+a2, tour.begin()+b1+1);
                            // now we have variant 2 just copy paste
                            reverse(tour.begin()+b2, tour.begin()+c1+1);
                            reverse(tour.begin()+a2, tour.begin()+c1+1);
                            //cout << 2 << endl;
                        } else
                        if (total > distance(a1, b1) + distance(a2, c1) + distance(b2, c2) + swapEpsilon) {
                            reverse(tour.begin()+a2, tour.begin()+b1+1);
                            reverse(tour.begin()+b2, tour.begin()+c1+1);
                            //cout << 3 << endl;
                        
                        } else did_transform = false;
                        
                        if (did_transform) {
                            start_again = true;
                        //    cout << "obj: " << perimeter(ps, tour, true)  << endl;
                        }
                        c1 = c2;
                    }
                    b1 = b2;
                }
                a1 = a2;
            }
        } while(start_again);
        
        return tour;
    }
};
