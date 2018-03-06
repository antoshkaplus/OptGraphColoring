//
//  MFL_Gym.h
//  FacilityLocation
//
//  Created by Anton Logunov on 4/28/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FacilityLocation__MFL_Gym__
#define __FacilityLocation__MFL_Gym__

#include <iostream>
#include <algorithm>
#include <random>
#include <map>
#include <fstream>

#include "MFL.h"

struct MFL_Gym : MFL_Solver {
    
    struct Repetition {
        Count count;
        vector<CustomerIndex> customers; 
    };
    
    Count iteration_count_{10000};
    double evolution_speed_{0.005};
    
    const std::vector<Customer>* customers_;
    const std::vector<Facility>* facilities_;
    // customer index , facility index
    arma::mat local_cost_;
    arma::mat global_cost_;
    arma::mat distances_;
        
    vector<FacilityIndex> customer_assignment_;
    vector<double> facility_filling_;

    map<FacilityIndex, Repetition> assignment_repetition_;
    // after some unimproving iteration will open facility again 
    ant::CountMap<FacilityIndex> closed_facilities_;
    
    bool did_objective_improve_;
    
    double approximate_objective_;
    double current_objective_;
    
    void assignCustomers() {
        auto &customers = *customers_;
        auto &facilities = *facilities_;
        fill(facility_filling_.begin(), facility_filling_.end(), 0);
        
        vector<CustomerIndex> indices(customers.size());
        iota(indices.begin(), indices.end(), 0);
        shuffle(indices.begin(), indices.end(), std::default_random_engine());
        //for (auto c_i = 0; c_i < customers.size(); ++c_i) {
        
        for (auto c_i : indices) {
            double best_cost = std::numeric_limits<double>::max(), cost;
            FacilityIndex f_i_best = 0;
            for (auto f_i = 0; f_i < facilities.size(); ++f_i) {
                if (closed_facilities_.count(f_i) != 0 ||
                    facility_filling_[f_i] + customers[c_i].demand > facilities[f_i].capacity) continue;
                // need to charge new price right in assignment time
                if ((cost = (distances_(c_i, f_i) + local_cost_(c_i, f_i) + global_cost_(c_i, f_i)/10)) < best_cost) {
                    f_i_best = f_i;
                    best_cost = cost;
                } 
            }
            customer_assignment_[c_i] = f_i_best;
            facility_filling_[f_i_best] += customers[c_i].demand;
        }
    }
    
    void updateCosts() {
        auto &customers = *customers_;
        auto &facilities = *facilities_;
        FacilityIndex f_i;
        // reduce old impact
        for (auto c_i = 0; c_i < customers.size(); ++c_i) {
            for (auto f_i = 0; f_i < facilities.size(); ++f_i) {
                auto &f = facilities[f_i];
                auto &c = customers[c_i];
                local_cost_(c_i, f_i) += 0.5*evolution_speed_*(f.cost*c.demand/f.capacity - local_cost_(c_i, f_i));
                //global_cost_(c_i, f_i) += 0.5*evolution_speed_*(approximate_objective_/customers.size() - global_cost_(c_i, f_i));
            }
        }
        
        // new impact
        for (auto c_i = 0; c_i < customers.size(); ++c_i) {
            f_i = customer_assignment_[c_i];
            auto &f = facilities[f_i];
            auto &c = customers[c_i];
            double ff = facility_filling_[f_i];
            if (ff > f.capacity) {
                throw "wtf";
            }
            else { // ff < f.capacity // fewer guys should cover everything
                local_cost_(c_i, f_i) += evolution_speed_*(f.cost*c.demand/ff - local_cost_(c_i, f_i));
                global_cost_(c_i, f_i) += evolution_speed_*(current_objective_/customers.size() - global_cost_(c_i, f_i));
            }
        }
        
        
    }
    
    bool isValidAssignment() {
        auto &facilities = *facilities_;
        for (auto f_i = 0; f_i < facilities.size(); ++f_i) {
            if (facility_filling_[f_i] > facilities[f_i].capacity) {
                return false;
            }
        }
        return true;
    }
    
    Count overfullFacilityCount() {
        Count count = 0;
        auto &facilities = *facilities_;
        for (auto f_i = 0; f_i < facilities.size(); ++f_i) {
            if (facility_filling_[f_i] > facilities[f_i].capacity) {
                ++count;
            }
        }
        return count;
    }
    
    Count openFacilitiesCount() {
        Count count = 0;
        for (auto& ff : facility_filling_) {
            if (ff > 0) ++count;
        }
        return count;
    }
    
    void updateRepetitions() {
        if (did_objective_improve_) {
            assignment_repetition_.clear();
            return;
        }
        
        map<FacilityIndex, vector<CustomerIndex>> assignment;
        for (auto c_i = 0; c_i < customer_assignment_.size(); ++c_i) {
            assignment[customer_assignment_[c_i]].push_back(c_i);
        }
        
        for (auto &p : assignment) {
            if (assignment_repetition_[p.first].customers == p.second) {
                ++assignment_repetition_[p.first].count;
            }
            else {
                assignment_repetition_[p.first] = Repetition{1, p.second};
            }
        }
    }
    
    void updateClosedFacilities() {
        if (did_objective_improve_) {
            for (auto &p : closed_facilities_) {
                p.second = 0;
            }
            return;
        }
        
        vector<FacilityIndex> open;
        for (auto &p : closed_facilities_) {
            ++p.second;
            if (p.second == 50) {
                open.push_back(p.first);
            }
        }
        for (auto i : open) closed_facilities_.erase(i);
        
        vector<FacilityIndex> close;
        for (auto &r : assignment_repetition_) {
            if (r.second.count == 50 /*&& closed_facilities_.size() < 5*/) {
                closed_facilities_[r.first] = 0;
                close.push_back(r.first);
                break;
            }
        }
        for (auto i : close) assignment_repetition_.erase(i);

        
    }    
    
    std::vector<FacilityIndex> solve(const std::vector<Customer>& customers, 
                                     const std::vector<Facility>& facilities) override {
        ofstream obj_out("obj_out.txt");
        
        customers_ = &customers;
        facilities_ = &facilities;
        facility_filling_.resize(facilities.size(), 0);
        customer_assignment_.resize(customers.size(), 0);
        distances_.resize(customers.size(), facilities.size());
        local_cost_.resize(customers.size(), facilities.size());
        global_cost_.resize(customers.size(), facilities.size());
        MFL_Greedy gr;
        customer_assignment_ = gr.solve(customers, facilities);
        approximate_objective_ = objective(customers, facilities, customer_assignment_);
        for (auto c_i = 0; c_i < customers.size(); ++c_i) {
            for (auto f_i = 0; f_i < facilities.size(); ++f_i) {
                distances_(c_i, f_i) = distance(customers[c_i], facilities[f_i]);
                local_cost_(c_i, f_i) = customers[c_i].demand*facilities[f_i].cost/facilities[f_i].capacity;
                global_cost_(c_i, f_i) = approximate_objective_/customers.size();
            }
        }
        
        
        vector<FacilityIndex> best_assignment;
        double best_obj = std::numeric_limits<double>::max(), obj;
        Count opened_count = 0;
        for (auto current_iteration = 0; current_iteration < iteration_count_; ++current_iteration) {
            assignCustomers();
            opened_count = openFacilitiesCount();
            obj = objective(customers, facilities, customer_assignment_);
            
            obj_out << current_iteration << " " << obj << endl;
            
            if (obj < best_obj) {
                cout << "it: " << current_iteration << " obj: " << obj << " opened: " << opened_count << endl;
                best_obj = obj;
                best_assignment = customer_assignment_;
                did_objective_improve_ = true;
            }
            else did_objective_improve_ = false;
            
            updateRepetitions();
            updateClosedFacilities();
            
            current_objective_ = obj;
            updateCosts();
        }
        
        return best_assignment;
    }
    
};


#endif /* defined(__FacilityLocation__MFL_Gym__) */
