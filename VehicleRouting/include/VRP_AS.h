//
//  VRP_AS.h
//  VehicleRouting
//
//  Created by Anton Logunov on 5/2/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __VehicleRouting__VRP_AS__
#define __VehicleRouting__VRP_AS__

#include <iostream>
#include <random>
#include <list>

#include <armadillo>

#include <ant/optimization/tsp/tsp.hpp>

#include "VRP.h"

struct VRP_AS : VRP {
    
    using Pair = pair<Index, Index>;
    
    struct Ant {
        
        void init( VRP_AS& master) {
            master_ = &master;
        }
        
        VehicleRoutes constructRoutes() {            
            VRP_CustomerConnector customer_connector(*master_->customers_, master_->vehicle_capacity_);
            
            auto &ps = master_->customer_pairs;
            std::list<pair<Index, Index>> pairs(ps.begin(), ps.end());
            vector<double> weights(master_->par_neighborhood_count);
            while (!pairs.empty()) {
                // first k solutions are feasible if you find those
                auto it = pairs.begin();
                for (auto &w : weights) {
                    if (it != pairs.end()) {
                        w = master_->total(it->first, it->second);
                        ++it;
                    }
                    else w = 0.;
                }
                std::discrete_distribution<> distribution(weights.begin(), weights.end());
                Index i = distribution(master_->rng);
                assert(weights[i] != 0.);
                it = pairs.begin();
                while (i--) ++it; 
                
                assert(customer_connector.connect(it->first, it->second));
                
                it = pairs.begin();
                Count n = 0;
                while (it != pairs.end() && n != master_->par_neighborhood_count) {
                    if (!customer_connector.canConnect(it->first, it->second) || master_->total(it->first, it->second) == 0.) {
                        auto ittt = it++;
                        pairs.erase(ittt);
                    }
                    else {++it; ++n;}
                }
            }
            return customer_connector.vehicleRoutes();
        }
        
        VRP_AS* master_;
    };
    
    void init(Point warehouse_location, Count vehicle_count, Count vehicle_capacity, const vector<Customer>& customers) {
        Count c_count = customers.size();
        par_pheromone = 5; 
        par_saving = 5;
        par_trail_persistance = .95;
        par_elitist_count = 6;
        par_neighborhood_count = customers.size()/4;
        par_iteration_count = 2*customers.size();
        par_ant_count = customers.size();
        
        vehicle_capacity_ = vehicle_capacity;
        customers_ = &customers;
        
        ant_routes.resize(par_ant_count);
        ant_objs.resize(par_ant_count);
        ant_order.resize(par_ant_count);
        
        distance.resize(c_count, c_count);
        for (auto i = 0; i < c_count; ++i) {
            for (auto j = i; j < c_count; ++j) {
                distance(i, j) = distance(j, i) = 
                ::distance(customers[i].location, customers[j].location); 
            }
        }
        
        vector<double> distance_to_warehouse(c_count);
        for (auto i = 0; i < c_count; ++i) {
            distance_to_warehouse[i] = ::distance(warehouse_location, 
                                                  customers[i].location);
        }
        saving.resize(c_count, c_count);
        for (auto i = 0; i < c_count; ++i) {
            for (auto j = i; j < c_count; ++j) {
                double d = distance_to_warehouse[i] + distance_to_warehouse[j]
                           - ::distance(customers[i].location, customers[j].location);
                saving(i, j) = saving(j, i) = d < 0. ? std::numeric_limits<double>::max() : d;
            }
        }
        Count c = 0;
        double d = min(vectorise(saving));
        for (auto i = 0; i < c_count; ++i) {
            for (auto j = i; j < c_count; ++j) { 
                if (saving(i, j) == std::numeric_limits<double>::max()) {
                    saving(i, j) = saving(j, i) = d;
                    ++c;
                }
            }
        }
        cout << c << endl;
        assert(arma::any(arma::vectorise(saving) < 0.) == false);
        
        pheromone_iteration.resize(c_count, c_count);
        pheromone.resize(c_count, c_count);
        VRP_SavingMethod solver;
        double approximate = objective(
            warehouse_location, customers, 
            solver.solve(warehouse_location, vehicle_count, vehicle_capacity, customers));
        
        pheromone.fill((par_ant_count*par_ant_count)/(approximate));
        
        // element wise multiplication
        total = pow(pheromone, par_pheromone) % pow(saving, par_saving);
        assert(arma::any(arma::vectorise(total) < 0.) == false);
        
        
        customer_pairs.clear();
        for (auto i = 0; i < c_count; ++i) {
            for (auto j = i+1; j < c_count; ++j) {
                customer_pairs.push_back({i, j});
            }
        }
        sort(customer_pairs.begin(), customer_pairs.end(), 
             [&](const pair<Index, Index>& p_0, const pair<Index, Index>& p_1) {
            
            return saving(p_0.first, p_0.second) > saving(p_1.first, p_1.second);
        });
    }
    
    void updatePheromone() {
        pheromone_iteration.fill(0.);
        // trail will write to pheromone iteration
        trail(best_ant_route, best_ant_obj, par_elitist_count);
        for (auto i = 0, n = 0; i < par_ant_count && n < par_elitist_count-1; ++i) {
            if (abs(ant_objs[ant_order[i]] - best_ant_obj) < 1.e-8) { 
                continue;
            }
            trail(ant_routes[ant_order[i]], ant_objs[ant_order[i]], par_elitist_count-n-1);
            ++n;
        }
        pheromone *= par_trail_persistance;
        pheromone += pheromone_iteration;
        total = pow(pheromone, par_pheromone) % pow(saving, par_saving);
    }
    
    void trail(const VehicleRoutes& vr, double obj, Count n) {
        Index i_prev;
        for (auto& r : vr) {
            for (auto i : r) {
                if (i == r.front()) goto next;
                pheromone_iteration(i_prev, i) = pheromone_iteration(i, i_prev) += n/obj;
                next:
                i_prev = i;
            }
        }
    }
    
    VehicleRoutes solve(
        Point warehouse_location,
        Count vehicle_count, 
        Count vehicle_capacity, 
        const vector<Customer>& customers) override {
                
        init(warehouse_location, vehicle_count, vehicle_capacity, customers);
        
        best_ant_obj = std::numeric_limits<double>::max();
        best_feasible_ant_obj = std::numeric_limits<double>::max();
    
        vector<Ant> ants(par_ant_count);
        for (auto& a : ants) a.init(*this);
        
        VRP_Swap swap;
        auto ddd = distanceMatrix(customers, warehouse_location);
        auto iteration = 0;
        for (; iteration < par_iteration_count; ++iteration) {
            auto repeat = 0;
            for (auto i = 0; i < ants.size(); ++i) {
                ant_routes[i] = ants[i].constructRoutes();
                ant_objs[i] = objective(warehouse_location, customers, ant_routes[i]);
//                if (any_of(ant_objs.begin(), ant_objs.begin()+i, [&, i] (double obj) { return abs(obj-ant_objs[i]) < 1e-7; })) {
//                    ++repeat;
//                    --i;
//                }
            }
            if (repeat) cout << repeat << endl;
            for (auto i = 0; i < ants.size(); ++i) {
                ant_routes[i] = swap.improve(ant_routes[i], vehicle_capacity, customers, ddd);
                for (auto& v : ant_routes[i]) {
                    v.push_back(customers.size());

                    // TODO figure out how to replace
                    // v = ant::opt::tsp::TSP_TwoOpt(v, ddd);

                    auto wh = find(v.begin(), v.end(), customers.size());
                    reverse(v.begin(), wh);
                    reverse(wh, v.end());
                    v.pop_back();                    
                }
                ant_objs[i] = objective(warehouse_location, customers, ant_routes[i]);
            }
            
            iota(ant_order.begin(), ant_order.end(), 0);
            // small objective is good
            sort(ant_order.begin(), ant_order.end(), [&](Index i_0, Index i_1) {
                return ant_objs[i_0] < ant_objs[i_1];
            });
            
            if (ant_objs[ant_order[0]] < best_ant_obj) {
                best_ant_route = ant_routes[ant_order[0]];
                best_ant_obj = ant_objs[ant_order[0]];
                cout << "it: " << iteration << " new obj: " << best_ant_obj << endl;
            }
            
            for (auto i : ant_order) {
                if (ant_routes[i].size() <= vehicle_count && ant_objs[i] < best_feasible_ant_obj) {
                    best_feasible_ant_route = ant_routes[i];
                    best_feasible_ant_obj = ant_objs[i];
                    cout << "it: " << iteration << " new feasible obj: " << best_feasible_ant_obj << endl;
                }
            }
            
            updatePheromone();
        }
        //return best_feasible_ant_route;
        return best_ant_route;
    }
    
    
    
    double par_trail_persistance; 
    double par_pheromone;
    double par_saving;
    Count par_neighborhood_count;
    Count par_elitist_count;
    Count par_iteration_count;
    Count par_ant_count;  
    
    // probably just make Container of ants
    VehicleRoutes best_ant_route;
    double best_ant_obj;
    VehicleRoutes best_feasible_ant_route;
    double best_feasible_ant_obj;
    vector<VehicleRoutes> ant_routes;
    vector<double> ant_objs;
    vector<Index> ant_order;
    
    // don't forget to initialize
    const vector<Customer> *customers_;
    Count vehicle_capacity_;
    
    // sorted by saving in decreasing order
    vector<pair<Index, Index>> customer_pairs;
    arma::mat saving;
    arma::mat pheromone;
    arma::mat pheromone_iteration;
    arma::mat total; 
    arma::mat distance;
    default_random_engine rng;
    // i feel like it should be inside distance matrix
    //vector<double> distance_to_warehouse;
};  



#endif /* defined(__VehicleRouting__VRP_AS__) */
