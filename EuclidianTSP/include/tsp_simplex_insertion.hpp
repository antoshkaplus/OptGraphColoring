#pragma once

#include "ant/grid/grid.hpp"

#include "tsp.hpp"

using namespace ant::grid;


// not quite tsp
// here we try to reduce area as much as possible

// city - index type
// point - Point type
struct TSP_SimplexInsertion : TSP_Solver {
    
    struct Item {
        Edge edge;
        City city;
        double profit;
        
        Item(Edge e, City c, double p) 
        : edge(e), city(c), profit(p) {}
        
        bool operator<(const Item t) const {
            return profit < t.profit;
        }
    };
    
    const vector<Point> *points;
    Count city_count;
    Count closest_count = 10;
    // get matrix for each point get 10-20 closest, sort them
    // row - each city, col - closest cities
    Grid<City> closest_cities; 
    Grid<char> edge_exists;
    vector<bool> visited;
    priority_queue<Item> insertion_queue;
    
    double Distance(City c_0, City c_1) {
        return (*points)[c_0].Distance((*points)[c_1]);
    }
    
    bool Exists(Edge e) {
        return edge_exists(e[0], e[1]);
    }
    
    double Profit(Edge e, City v) {
        double a = Distance(e[0], e[1]);
        double b = Distance(e[0], v);
        double c = Distance(e[1], v);
//        double p = (a + b + c)/2.;
        return b + c - a; // p*(p-a)*(p-b)*(p-c);
    }
    
    void Remove(Edge e) {
        edge_exists(e[0], e[1]) = edge_exists(e[1], e[0]) = false;
    }
    
    void Add(Edge e) {
        edge_exists(e[0], e[1]) = edge_exists(e[1], e[0]) = true; 
    }
    
    void UpdateQueue(City new_v, City e_0, City e_1) {
        for (int j = 0; j < closest_cities.col_count(); ++j) {
            int k = closest_cities(new_v, j);
            if (visited[k]) continue;
            // queue is max
            insertion_queue.emplace(Edge{e_0, new_v}, k,
                                    -Profit({e_0, new_v}, k));
            insertion_queue.emplace(Edge{e_1, new_v}, k, 
                                    -Profit({e_1, new_v}, k));
        }
    }
    
    vector<City> GenerateStartingTour() {
        vector<City> tour;
        uniform_int_distribution<City> distr(0, city_count-1);
        default_random_engine rng;
        int city = distr(rng);
        tour.push_back(city);
        tour.push_back(closest_cities(city, 0));
        tour.push_back(closest_cities(city, 1));
        Add(Edge{tour[0], tour[1]});
        Add(Edge{tour[1], tour[2]});
        Add(Edge{tour[2], tour[0]});
        visited[tour[0]] = visited[tour[1]] = visited[tour[2]] = true;
        return tour;
    }
    
    void set_closest_count(Count count) {
        closest_count = count;
    }
    
    
    vector<City> solve(const vector<Point>& points) override {
        city_count = points.size();
        visited.resize(city_count, false);
        edge_exists.resize(city_count, city_count);
        edge_exists.fill(false);
        this->points = &points;
        closest_cities.resize(city_count, closest_count);
        for (City c = 0; c < city_count; ++c) {
            vector<City> cities(city_count);
            iota(cities.begin(), cities.end(), 0);
            swap(cities[c], cities[city_count-1]);
            cities.pop_back();
            K_NearestPoints(points, points[c], cities, closest_count);
            for (int i = 0; i < cities.size(); ++i) {
                closest_cities(c, i) = cities[i];
            }
        }
        // three points
        vector<int> tour = GenerateStartingTour();
        UpdateQueue(tour[0], tour[1], tour[2]);
        UpdateQueue(tour[1], tour[0], tour[2]);
        UpdateQueue(tour[2], tour[1], tour[0]);
        
        while (!insertion_queue.empty()) {
            Item t = insertion_queue.top();
            insertion_queue.pop();
            if (visited[t.city] || !Exists(t.edge)) continue;
            Remove(t.edge);
            Add(Edge{t.edge[0], t.city});
            Add(Edge{t.edge[1], t.city});
            visited[t.city] = true;
            UpdateQueue(t.city, t.edge[0], t.edge[1]);
        }
        tour.resize(city_count);
        City cur = 0;
        City prev = -1;
        for (int i = 0; i < city_count; ++i) {
            tour[i] = cur;
            for (int j = 0; j < city_count; ++j) {
                if (Exists(Edge{cur, j}) && j != prev) {
                    prev = cur;
                    cur = j;
                    break;
                }
            }
        }
        return tour;
    }
    
};