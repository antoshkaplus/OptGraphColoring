//
//  VRP.h
//  VehicleRouting
//
//  Created by Anton Logunov on 4/30/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __VehicleRouting__VRP__
#define __VehicleRouting__VRP__

#include <iostream>
#include <set>
#include <cmath>
#include <assert.h>
#include <queue>
#include <list>

#include <armadillo>
#include <ant/core/core.hpp>
#include <ant/geometry/d2.hpp>

using namespace std;

using namespace ant;
using namespace ant::geometry::d2::f;

struct Customer {
    Count demand;
    Point location;
};

double distance(const Point& p_0, const Point& p_1) {
    return sqrt((p_0.x - p_1.x)*(p_0.x - p_1.x) + (p_1.y - p_0.y)*(p_1.y - p_0.y));
}

using VehicleRoutes = vector<vector<Index>>;
using Route = vector<Index>;

struct VRP {
    // vector of paths for each vehikle
    virtual VehicleRoutes solve(
        Point warehouse_location,
        Count vehicle_count, 
        Count vehicle_capacity, 
        const vector<Customer>& customers) = 0;
        
    static double objective(
        Point warehouse_location, 
        const vector<Customer>& customers, 
        const VehicleRoutes& routes) {
        
        Point p;
        double obj = 0;
        for (auto& r : routes) {
            p = warehouse_location;
            for (auto i : r) {
                obj += distance(p, customers[i].location);
                p = customers[i].location;
            }
            obj += distance(p, warehouse_location);
        }
        return obj;
    }
    
    // 0 element is an index if the depot // last index should be also a depot
    static VehicleRoutes constructRoutes(const vector<Index>& route) {
        VehicleRoutes vr;
        Index depot = route[0];
        vector<Index> r; 
        for (auto i : route) {
            if (i == depot) {
                vr.push_back(r);
                r.clear();
            }
            else {
                r.push_back(i);
            }
        }
        // first guy is just an empty cuz of 0 depot
        vr.front() = vr.back();
        vr.pop_back();
        
        return vr;
    }
    
    static vector<double> filling(const VehicleRoutes& vr, const vector<Customer>& cs) {
        vector<double> ff;
        for (auto& r : vr) {
            double w = 0;
            for (auto i_c : r) {
                w += cs[i_c].demand;
            }
            ff.push_back(w);
        }
        return ff;
    }
    
    static arma::mat distanceMatrix(const vector<Customer>& cs, Point wh_loc) {
        arma::mat ds(cs.size()+1, cs.size()+1);
        for (auto i = 0; i < cs.size(); ++i) {
            for (auto j = i; j < cs.size(); ++j) {
                ds(i, j) = ds(j, i) = distance(cs[i].location, cs[j].location);
            }
        }
        for (auto i = 0; i < cs.size(); ++i) {
            ds(cs.size(), i) = ds(i, cs.size()) = distance(wh_loc, cs[i].location);
        }
        ds(cs.size(), cs.size()) = 0.;
        return ds;
    }
};

struct VRP_Improver {
    virtual VehicleRoutes improve(
        const VehicleRoutes& vr_in,
        Count vehicle_capacity, 
        const vector<Customer>& customers,
        const arma::mat& distance,
        double epsilon=1.e-7) = 0;
};

struct VRP_Swap : VRP_Improver {
    // warehouse should be last guy
    VehicleRoutes improve(
        const VehicleRoutes& vr_in,
        Count vehicle_capacity, 
        const vector<Customer>& customers,
        const arma::mat& distance,
        double epsilon=1.e-7) override {
        
        auto vr = vr_in;
        
        #define DEMAND(i) (customers[i].demand)
        // first index of route // second index if customer in vr[i]
        #define PREV(i, i_c) (i_c == 0 ? customers.size() : vr[i][i_c-1])
        #define NEXT(i, i_c) (i_c == vr[i].size()-1 ? customers.size() : vr[i][i_c+1])
        
        vector<double> filling = VRP::filling(vr, customers);
        bool again = true;
        while (again) {
            again = false;
            for (auto i = 0; i < vr.size(); ++i) {
                for (auto j = i+1; j < vr.size(); ++j) {
                    for (auto i_c = 0; i_c < vr[i].size(); ++i_c) {
                        for (auto j_c = 0; j_c < vr[j].size(); ++j_c) {
                            Index cus_i = vr[i][i_c]; // this guy can be changed by iteration lol
                            Index cus_j = vr[j][j_c];
                            if (!(filling[i] - (double)DEMAND(cus_i) + DEMAND(cus_j) <= vehicle_capacity &&
                                  filling[j] - (double)DEMAND(cus_j) + DEMAND(cus_i) <= vehicle_capacity)) {
                                continue;
                            }
                            Index 
                            cus_i_prev = PREV(i, i_c),
                            cus_i_next = NEXT(i, i_c),
                            cus_j_prev = PREV(j, j_c),
                            cus_j_next = NEXT(j, j_c);
                            if (distance(cus_j, cus_j_next) + distance(cus_j, cus_j_prev) + 
                                distance(cus_i, cus_i_next) + distance(cus_i, cus_i_prev) - epsilon 
                                >
                                distance(cus_j, cus_i_prev) + distance(cus_j, cus_i_next) + 
                                distance(cus_i, cus_j_prev) + distance(cus_i, cus_j_next)) {
                                
                                filling[i] += -(double)DEMAND(cus_i) + DEMAND(cus_j);
                                filling[j] += -(double)DEMAND(cus_j) + DEMAND(cus_i);
                                
                                swap(vr[i][i_c], vr[j][j_c]);
                                
                                again = true;
                                //cout << "contgrat" << endl;
                            }
                        }
                    }
                }
            }
        }
        
        #undef DEMAND
        #undef PREV
        #undef NEXT
    
        return vr;
    }
    
};

struct VRP_NearestNeighbour : VRP {
    
    VehicleRoutes solve(
        Point warehouse_location,
        Count vehicle_count, 
        Count vehicle_capacity, 
        const vector<Customer>& customers) override {
        
        VehicleRoutes routes;
        // maybe vector is more appropriate
        set<Index> unvisited_customers;
        for (auto i = 0; i < customers.size(); ++i) unvisited_customers.insert(i);
        Index cur_vehicle = 0;
        Count cur_vehicle_filling = 0;
        Route cur_route;
        Point cur_locaiton = warehouse_location;
        while (true) { 
            if (cur_vehicle == vehicle_count) break;
            if (unvisited_customers.empty()) {
                if (!cur_route.empty()) {
                    routes.push_back(cur_route);
                }
                break;
            }
            
            auto c_it = min_element(
                unvisited_customers.begin(), 
                unvisited_customers.end(), 
                [&](Index i_0, Index i_1) {
                    return 
                        customers[i_0].demand + cur_vehicle_filling <= vehicle_capacity && 
                       (distance(cur_locaiton, customers[i_0].location) < 
                        distance(cur_locaiton, customers[i_1].location) ||
                        customers[i_1].demand + cur_vehicle_filling >  vehicle_capacity) ;
           });
           Index c_ind = *c_it;
           
           if (customers[c_ind].demand + cur_vehicle_filling > vehicle_capacity) {
               for (auto i : unvisited_customers) {
                   assert(customers[i].demand + cur_vehicle_filling > vehicle_capacity);
               }
               
               routes.push_back(cur_route);
               cur_route.clear();
               cur_locaiton = warehouse_location;
               ++cur_vehicle;
               cur_vehicle_filling = 0;
           } 
           else {
               cur_route.push_back(c_ind);
               unvisited_customers.erase(c_it);
               cur_vehicle_filling += customers[c_ind].demand;
               cur_locaiton = customers[c_ind].location;
           }
        }
        
        if (cur_vehicle == vehicle_count && !unvisited_customers.empty()) {
            cout << unvisited_customers.size() << endl;
            cout << "no vehicle left" << endl;
        }
        return routes;
    }
    
};

struct VRP_CustomerConnector {
    
    VRP_CustomerConnector(const vector<Customer>& customers, ant::Int vehicle_capacity)
    :   customers_(&customers),
        vehicle_capacity_(vehicle_capacity),
        filling_(customers.size()),
        degree_(customers.size(), 0),
        dj_set_(customers.size()) {

        for (auto i = 0; i < customers.size(); ++i) {
            filling_[i] = customers[i].demand;
        }
        routes_.resize(customers.size());
        for (auto i = 0; i < customers.size(); ++i) {
            routes_[i].clear();
            routes_[i].push_back(i);
        }
    }

    VehicleRoutes vehicleRoutes() {
        // now need to construct routes from sets
        VehicleRoutes rs;
        for (Index i = 0; i < customers_->size(); ++i) {
            if (routes_[i].empty()) continue;
            rs.emplace_back(routes_[i].begin(), routes_[i].end());
        }
        return rs;
        return VehicleRoutes();
    }

    // canConnect and this method will use to many same shit
    // and will be called one after another
    bool connect(Index c_0, Index c_1) {
        if (degree_[c_0] == 2 || degree_[c_1] == 2) return false;
        Index
        r_0 = dj_set_.root(c_0),
        r_1 = dj_set_.root(c_1);
        if (!dj_set_.is_separate(r_0, r_1) ||
            filling_[r_0] + filling_[r_1] > vehicle_capacity_) return false;

        // can assign only one below... but fuck it
        filling_[r_0] = filling_[r_1] = filling_[r_0] + filling_[r_1];
        dj_set_.Unite(r_0, r_1);
        ++degree_[c_0];
        ++degree_[c_1];
        // r_0 should be target
        if (dj_set_.root(r_1) != r_0) {
            swap(r_0, r_1);
            swap(c_0, c_1);
        }
        connectRoutes(r_0, c_0, r_1, c_1);
        return true;
    }

    bool canConnect(Index c_0, Index c_1) {
        if (degree_[c_0] == 2 || degree_[c_1] == 2) return false;
        Index
        r_0 = dj_set_.root(c_0),
        r_1 = dj_set_.root(c_1);
        if (!dj_set_.is_separate(r_0, r_1) ||
            filling_[r_0] + filling_[r_1] > vehicle_capacity_) return false;
        return true;
    }

private:
    void connectRoutes(Index i_target, Index c_t, Index i_source, Index c_s) {
        // everything goes to r_1
        list<Index>
        &r_t = routes_[i_target],
        &r_s = routes_[i_source];
        bool
        r_t_back  = r_t.back() == c_t,
        r_t_front = r_t.front() == c_t,
        r_s_back  = r_s.back() == c_s,
        r_s_front = r_s.front() == c_s;

        if (r_t_front) {
            if (r_s_front)  r_s.reverse();
            r_t.splice(r_t.begin(), r_s);
        } else if (r_t_back) { // r_t_back
            if (r_s_back)   r_s.reverse();
            r_t.splice(r_t.end(), r_s);
        } else {
            throw "fuck you!";
        }
    }

    ant::Int vehicle_capacity_;
    const vector<Customer> *customers_;
    DisjointSet dj_set_;
    vector<Count> degree_;
    vector<list<Index>> routes_;
    vector<ant::Int> filling_;
};


struct VRP_SavingMethod : VRP {
    struct Record {
        Index i_c_0, i_c_1;    
        double savings;
        bool operator<(const Record& r) const {
            return savings < r.savings;
        }
    };
    
    void init(Point warehouse_location, const vector<Customer>& customers) {
        warehouse_location_ = warehouse_location;
        customers_ = &customers;
        computeCustomerWarehouseDistance();
        constructSavingQueue();
    }
    
    void constructSavingQueue() {
        // maybe will be not empty
        while (!customer_pairs_.empty()) customer_pairs_.pop();
        auto &cs = *customers_;
        auto &dw = distance_to_warehouse_;
        for (Index i = 0; i < cs.size(); ++i) {
            for (Index j = i+1; j < cs.size(); ++j) {
                double d = dw[i] + dw[j]
                         - ::distance(cs[i].location, cs[j].location);
                customer_pairs_.push(Record{i, j, d});
            }
        }
    }
    
    void computeCustomerWarehouseDistance() {
        auto &cs = *customers_;
        distance_to_warehouse_.resize(cs.size());
        for (auto i = 0; i < cs.size(); ++i) { 
            distance_to_warehouse_[i] = ::distance(cs[i].location, warehouse_location_);
        } 
    }

    VehicleRoutes solve(
        Point warehouse_location,
        Count vehicle_count, 
        Count vehicle_capacity, 
        const vector<Customer>& customers) override {
        
        VRP_CustomerConnector cc(customers, vehicle_capacity);
        
        init(warehouse_location, customers);
        
        while (!customer_pairs_.empty()) {
            Record p = customer_pairs_.top();
            customer_pairs_.pop();
            cc.connect(p.i_c_0, p.i_c_1);
        }
        return cc.vehicleRoutes();
    }
    
    
    Point warehouse_location_;
    const vector<Customer>* customers_;
    // max queue is good // better just make sorted array of it
    priority_queue<Record> customer_pairs_;
    vector<double> distance_to_warehouse_;
};




#endif /* defined(__VehicleRouting__VRP__) */
