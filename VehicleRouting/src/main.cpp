//
//  main.cpp
//  VehicleRouting
//
//  Created by Anton Logunov on 4/30/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "VRP.h"
#include "VRP_ACS.h"
#include "VRP_AS.h"

string root = "/Users/antoshkaplus/Documents/Coursera/"
              "DiscreteOptimization/VehicleRouting/Test/";
ifstream input(root + "input.txt");
ofstream output(root + "output.txt"); 

int main(int argc, const char * argv[])
{
    Count 
    customer_count,
    vehicle_count,
    vehicle_capacity;
    Point warehouse_location;
    
    input >> customer_count >> vehicle_count >> vehicle_capacity;
    --customer_count; // first string is warehouse location
    vector<Customer> customers(customer_count);
    int zero;
    input >> zero >> warehouse_location.x >> warehouse_location.y;
    for (auto i = 0; i < customer_count; ++i) {
        Customer &c = customers[i];
        input >> c.demand >> c.location.x >> c.location.y;
    }
    
    VRP_AS solver;
    //VRP_SavingMethod solver;
    //VRP_ACS solver;
    //VRP_NearestNeighbour solver;
    VehicleRoutes routes, routes_new;
    for (auto c = 0; c < 10; ++c) {
        solver.rng.seed(chrono::system_clock::now().time_since_epoch().count());
        routes_new = solver.solve(warehouse_location, vehicle_count, vehicle_capacity, customers);
        if (c == 0 || VRP::objective(warehouse_location, customers, routes_new) < VRP::objective(warehouse_location, customers, routes)) {
             routes = routes_new;
        }
    }
    /*
    VRP_Swap improver;
    routes = improver.improve(routes, vehicle_capacity, customers, VRP::distanceMatrix(customers, warehouse_location));
    for (auto& r : routes) {
        r.push_back(customers.size());
        r = ant::opt::two_opt(r, VRP::distanceMatrix(customers, warehouse_location));
        auto wh = find(r.begin(), r.end(), customers.size());
        reverse(r.begin(), wh);
        reverse(wh, r.end());
        r.pop_back();
    }
    */
     
    output << VRP::objective(warehouse_location, customers, routes) << " " << 0 << endl;
    for (auto& r : routes) {
        output << 0 << " ";
        for (auto i : r) {
            output << i+1 << " ";
        }
        output << 0 << endl;
    }
    for (auto i  = routes.size(); i < vehicle_count; ++i) {
        output << 0 << " " << 0 << endl;
    }
    
    //cout << endl;
    
    // insert code here...
    return 0;
}

