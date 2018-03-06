//
//  VRP_ACS.h
//  VehicleRouting
//
//  Created by Anton Logunov on 4/30/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __VehicleRouting__VRP_ACS__
#define __VehicleRouting__VRP_ACS__

#include <iostream>
#include <random>

#include <armadillo>

#include "VRP.h"


struct VRP_ACS : VRP {
    struct Ant {
        void init(VRP_ACS& system) {
            system_ = &system;
            visited.resize(system_->customers_->size());
        }
        
        // will take warehouse location from system
        void prepare() {
            auto &s = *system_;
            auto &c = *s.customers_;
            vehicle_filling = 0;
            fill(visited.begin(), visited.end(), false);
            route.clear();
            route.push_back(system_->customers_->size());
            distance = 0;
            
            customers_left = c.size();
            vehicles_left = s.vehicle_count_;
        }
        
        // we start from depo and will be back in depo. 
        // if we can't go out from depo again ant finished
        void makeMove() {
            if (didFinish()) return;
            auto &s = *system_;
            auto &cs = *s.customers_;
            auto capacity = s.vehicle_capacity_;
            // first of all go to warehouse
            Index i_best = cs.size();
            Index i_prev = route.back();
            // need to check par
            
            vector<Index> candidates;
            for (auto i : s.nearest_neighbor_list[i_prev]) {
                if (i == cs.size() || (!visited[i] && cs[i].demand + vehicle_filling <= capacity)) {
                    candidates.push_back(i);
                }
            }
            
            // if anything in candidates lol
            
            if (!candidates.empty()) {
                
                if (s.random_distribution(s.rng) > s.par_curiosity) {
                    i_best = *max_element(candidates.begin(), candidates.end(), [&, i_prev](Index i_0, Index i_1) {
                        return s.total(i_prev, i_0) < s.total(i_prev, i_1);
                    });
                    assert(i_best >= 0 && i_best <= cs.size());
                } else {
                    vector<double> weights(candidates.size());
                    for (auto i = 0; i < candidates.size(); ++i) {
                        weights[i] = s.total(i_prev, i);
                    }
                    i_best = candidates[discrete_distribution<Index>(weights.begin(), weights.end())(s.rng)];
                    assert(i_best >= 0 && i_best <= cs.size());
                }
            
            }
            
            assert(i_best >= 0 && i_best <= cs.size());
            
            if (i_best == cs.size()) {
                --vehicles_left;
                vehicle_filling = 0;   
                if (i_prev == i_best || (vehicles_left == 0 && customers_left != 0)) {
                    vehicles_left = 0;   
                    distance = s.approximate_objective; // probably need just make it big enough
                    return;
                }
            }
            else {
                --customers_left; 
                vehicle_filling += cs[i_best].demand;  
                visited[i_best] = true;
            }
            
            s.localPheromoneUpdate(i_prev, i_best);
            route.push_back(i_best);
            distance += s.distance(i_prev, i_best);
        }
        
        // no more vehicles or no more customers left
        bool didFinish() {
            return vehicles_left == 0 || (customers_left == 0 && route.back() == route.front());
        }
        
        vector<Index> route;
        
        Count vehicles_left;
        Count customers_left;
        
        VRP_ACS* system_;
        Count vehicle_filling;
        vector<bool> visited;
        // when can't finish route distance goes to MAXFLOAF
        double distance;
        
    }; // ant definition finished


    VehicleRoutes solve(
        Point warehouse_location,
        Count vehicle_count, 
        Count vehicle_capacity, 
        const vector<Customer>& customers) override {
        
        VehicleRoutes best_routes;
        double best_distance = std::numeric_limits<double>::max();
        
        warehouse_location_ = warehouse_location;
        vehicle_count_ = vehicle_count;
        vehicle_capacity_ = vehicle_capacity;
        customers_ = &customers;
        initDistance();
        initPheromone();
        initTotal();
        // uses distance matrix!
        initNearestNeighbourList();
        
        ants.resize(ant_count);
        for (auto& a : ants) a.init(*this);
        
        best_so_far_ant = &ants.front();
        for (auto iteration = 0; iteration < iteration_count; ++iteration) {
            for (auto& a : ants) a.prepare();
            Count ready = 0;
            while (ready != ant_count) {
                ready = 0;
                for (auto& a : ants) {
                    a.makeMove();
                    if (a.didFinish()) ++ready;
                }
            }
        
            auto iteration_best_ant = min_element(ants.begin(), ants.end(), [](const Ant& a_0, const Ant& a_1){
                return a_0.distance < a_1.distance;
            });
            
            if (iteration_best_ant->distance < best_distance) {
                best_so_far_ant = iteration_best_ant.base();
                best_distance = best_so_far_ant->distance;
                best_routes = constructRoutes(best_so_far_ant->route);
                cout << best_so_far_ant->distance << " " << best_so_far_ant->vehicles_left << " " << best_so_far_ant->customers_left << endl;
            }
            
            globalPheromoneUpdate();

        }
        return best_routes; 
    } 
    
    // ant did choose some edge to go to
    void localPheromoneUpdate(Index i_prev, Index i) {
        pheromone(i_prev, i) = 
        pheromone(i, i_prev) = (1. - par_evaporation) * pheromone(i_prev, i) + 
                                     par_evaporation * 1./(approximate_objective);
        total(i_prev, i) = 
        total(i, i_prev) = pow(pheromone(i_prev, i), par_pheromone) * 
                           pow( distance(i_prev, i), par_distance);
    }
    
    void globalPheromoneUpdate() {
        Index i_prev, first = true;
        double objective = best_so_far_ant->distance;
        for (auto i : best_so_far_ant->route) {
            if (first) {
                first = false;
                goto update_prev;
            }
            
            pheromone(i_prev, i) = 
            pheromone(i, i_prev) = (1. - par_evaporation) * pheromone(i, i_prev) + 
                                         par_evaporation * 1./objective;
            total(i_prev, i) = 
            total(i, i_prev) = pow(pheromone(i_prev, i), par_pheromone) * 
                               pow( distance(i_prev, i), par_distance);
            update_prev:
            i_prev = i;
        }
    }
    
    void initNearestNeighbourList() {
        auto &d = distance;
        auto &cs = *customers_;
        auto &nn = nearest_neighbor_list;
        vector<Index> indices(cs.size()+1);
        iota(indices.begin(), indices.end(), 0);
        // setted count may be too big, lol, don't count same index 
        Count nn_i_count = min(nearest_neighbor_count, static_cast<Count>(cs.size()));
        nn.resize(cs.size()+1);
        for (auto i = 0; i <= cs.size(); ++i) {
            nn[i].resize(nn_i_count);
            // don't need partial sort actually
            // don't compute same element as nn
            partial_sort_copy(indices.begin(), indices.end(), 
                              nn[i].begin(), nn[i].end(), 
                              [&d, i](Index i_0, Index i_1) {
                                    return i_0 != i && (d(i_0, i) < d(i_1, i) || i_1 == i);
                              });
        }
    }
    
    void initDistance() {
        Point p_r, p_c;
        auto &cs = *customers_;
        distance.resize(cs.size()+1, cs.size()+1);
        for (auto r = 0; r <= cs.size(); ++r) {
            for (auto c = r; c <= cs.size(); ++c) {
                // preventing going back
                if (r == c) distance(r, c) = distance(c, r) = std::numeric_limits<double>::max();
                else {
                    p_r = r == cs.size() ? warehouse_location_ : cs[r].location;
                    p_c = c == cs.size() ? warehouse_location_ : cs[c].location;
                    distance(r, c) = distance(c, r) = ::distance(p_r, p_c);
                }
            }
        }
    }
    
    void initPheromone() {
        VRP_NearestNeighbour solver;
        VehicleRoutes vr = solver.solve(warehouse_location_, vehicle_count_, vehicle_capacity_, *customers_);
        approximate_objective = objective(warehouse_location_, *customers_, vr);
        
        auto &cs = *customers_;
        // diagonals can have same pheromone, distance will back up
        pheromone.resize(cs.size()+1, cs.size()+1);
        pheromone.fill(1./(customers_->size()*approximate_objective));
    }
    
    void initTotal() {
        auto &cs = *customers_;
        total.resize(cs.size()+1, cs.size()+1);
        for (auto r = 0; r <= cs.size(); ++r) {
            for (auto c = r; c <= cs.size(); ++c) {
                // preventing going back                
                total(r, c) = total(c, r) = pow(pheromone(r, c), par_pheromone) * 
                                            pow( distance(r, c), par_distance);
            }
        }
    }
    
    Count ant_count{10};
    Count iteration_count{100000};
    
    Ant *best_so_far_ant, 
        *iteration_best_ant;
    
    double par_evaporation{0.1};
    double par_distance{0};
    double par_pheromone{1}; // possibly to use 2
    double approximate_objective;
    double par_curiosity{0.1};
    
    vector<Ant> ants;
    
    Point warehouse_location_;
    Count vehicle_count_;
    Count vehicle_capacity_;
    const vector<Customer>* customers_;
    // customer_count + 1 (last one is warehouse) 
    arma::mat pheromone;
    arma::mat distance;
    arma::mat total;
    
    Count nearest_neighbor_count{50};
    // for each customer there are some nearest sorted neighbours
    vector<vector<Index>> nearest_neighbor_list;
    uniform_real_distribution<double> random_distribution{0., 1.};
    default_random_engine rng;
};

#endif /* defined(__VehicleRouting__VRP_ACS__) */
