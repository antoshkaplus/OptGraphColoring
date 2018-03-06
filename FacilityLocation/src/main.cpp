//
//  main.cpp
//  include
//
//  Created by Anton Logunov on 4/5/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include <iostream>

#include "MFL.h"
#include "MFL_TwoSwap.h"
#include "MFL_Gym.h"
#include "MFL_MIP.h"
#include "MFL_Mixed.h"

using namespace std;

int main(int argc, const char * argv[])
{
    const std::string root = "/Users/antoshkaplus/Documents"
                        "/Coursera/DiscreteOptimization/include/Test/";
    const std::string inputFile = root + "input.txt";
    const std::string outputFile = root + "output.txt";
    std::ifstream f_in(inputFile);
    std::ofstream f_out(outputFile);

    std::vector<Customer> customers;
    std::vector<Facility> facilities;
    Count customerCount, facilityCount;
    f_in >> facilityCount >> customerCount;
    customers.resize(customerCount);
    facilities.resize(facilityCount);
    
    std::cout << "customer count: " << customers.size() << std::endl;
    std::cout << "facility count: " << facilities.size() << std::endl;
    
    
    
    for (FacilityIndex i = 0; i < facilityCount; i++) {
        f_in >> facilities[i];
    }
    for (CustomerIndex i = 0; i < customerCount; i++) {
        f_in >> customers[i];
    }

//    MFL_MIP solver;
//    MFL_Greedy solver;
//    std::vector<FacilityIndex> customerFacilities = solver.solve(customers, facilities);
    
    
    
    
    MFL_Gym solver;
    std::vector<FacilityIndex> customerFacilities = solver.solve(customers, facilities);

    
    MFL_TwoSwap improver_0;
    customerFacilities = improver_0.improve(customers, facilities, customerFacilities);
    
    MFL_Nearest improver_4;
    customerFacilities = improver_4.improve(customers, facilities, customerFacilities);
    
    
    MFL_GreedyClosing improver;
    customerFacilities = improver.improve(customers, facilities, customerFacilities);
    
    MFL_TwoSwap improver_2;
    customerFacilities = improver_2.improve(customers, facilities, customerFacilities);
    
    MFL_Nearest improver_3;
    customerFacilities = improver_3.improve(customers, facilities, customerFacilities);    

    MFL_Mixed solver_2;
    customerFacilities = solver_2.improve(customers, facilities, customerFacilities);
    
    f_out << solver.objective(customers, facilities, customerFacilities) << " " << 0 << std::endl;    
    for (auto f : customerFacilities) {
        f_out << f << " ";
    }
    
    return 0;
}

