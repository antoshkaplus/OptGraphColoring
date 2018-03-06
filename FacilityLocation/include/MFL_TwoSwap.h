//
//  MFL_TwoSwap.h
//  FacilityLocation
//
//  Created by Anton Logunov on 4/27/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FacilityLocation__MFL_TwoSwap__
#define __FacilityLocation__MFL_TwoSwap__

#include <iostream>

#include "MFL.h"

struct MFL_TwoSwap : MFL_Improver {
    std::vector<FacilityIndex> improve(const std::vector<Customer>& customers, 
                                       const std::vector<Facility>& facilities,
                                       const std::vector<FacilityIndex>& customerFacilities) {
        vector<double> facility_filling = facilityFilling(customers, customerFacilities, facilities.size());
        
        vector<FacilityIndex> customer_facilities = customerFacilities;
        
        start_again:
        for (auto c_0_i = 0; c_0_i < customers.size(); ++c_0_i) {
            auto f_0_i = customer_facilities[c_0_i];
            const Customer &c_0 = customers[c_0_i];
            const Facility &f_0 = facilities[f_0_i];
            for (auto f_1_i = 0; f_1_i < facilities.size(); ++f_1_i) {
                const Facility &f_1 = facilities[f_1_i];
                if (f_0_i != f_1_i && 
                    facility_filling[f_1_i] > 0 && 
                    distance(f_0, c_0) > distance(f_1, c_0)) {
                    
                    for (auto c_1_i = 0; c_1_i < customers.size(); ++c_1_i) {
                        const Customer &c_1 = customers[c_1_i];
                        if (customer_facilities[c_1_i] != f_1_i || 
                            f_1.capacity < facility_filling[f_1_i] - c_1.demand + c_0.demand) {
                            
                            continue;   
                        }
                        
                        facility_filling[f_0_i] -= c_0.demand;
                        double ff = facility_filling[f_1_i];
                        facility_filling[f_1_i] = f_1.capacity;
                        
                        FacilityIndex f_new_i = nearestCompatibleFacility(facilities, facility_filling, c_1);
                        const Facility &f_new = facilities[f_new_i];
                         
                        facility_filling[f_1_i] = ff;
                        facility_filling[f_0_i] += c_0.demand;
                        
                        if (f_new_i == facilities.size()) {
                            continue;
                        }
                        
                        if (distance(c_0, f_1) + distance(c_1, f_new) < distance(c_0, f_0) + distance(c_1, f_1) 
//                        &&
//                        f_0.capacity >= facility_filling[f_0_i] - c_0.demand + c_1.demand &&
//                        f_1.capacity >= facility_filling[f_1_i] - c_1.demand + c_0.demand
                        ) {
                            
                            facility_filling[f_0_i] += - c_0.demand;
                            facility_filling[f_1_i] += - c_1.demand + c_0.demand;
                            facility_filling[f_new_i] += c_1.demand;
                            
                            customer_facilities[c_0_i] = f_1_i;
                            customer_facilities[c_1_i] = f_new_i;
                            
                            goto start_again;
                        }
                    
                    }
                
                }
            }
        }
        
        return customer_facilities;
    }
};


#endif /* defined(__FacilityLocation__MFL_TwoSwap__) */
