//
//  MFL.h
//  FacilityLocation
//
//  Created by Anton Logunov on 4/7/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FacilityLocation__MFL__
#define __FacilityLocation__MFL__

#include <iostream>


#include <vector>
#include <iostream>
#include <numeric>
#include <set>
#include <functional>
#include <stack>
#include <map>
#include <cassert>

#include <armadillo>

#include <ant/core/core.hpp>

using namespace std;

struct Point : arma::vec2 {
    static double distance(const Point p_0, const Point p_1) {
        return sqrt(pow(p_0[0] - p_1[0], 2) + pow(p_0[1] - p_1[1], 2));
    }
};

struct Facility {
    double cost;
    uint capacity;
    Point location;
    
};

struct Customer {
    uint demand;
    Point location;
};

typedef uint CustomerIndex;
typedef uint FacilityIndex;
typedef uint Index;
typedef uint Weight;
typedef uint Count;

std::istream& operator>>(std::istream& in, Customer& c) {
    return in >> c.demand >> c.location[0] >> c.location[1]; 
}

std::istream& operator>>(std::istream& in, Facility& f) {
    return in >> f.cost >> f.capacity >> f.location[0] >> f.location[1];
}

double distance(const Facility& f, const Customer& c) {
    return Point::distance(f.location, c.location);
}

double distance(const Customer& c, const Facility& f) {
    return distance(f, c);
}

bool isCompatible(const Facility& f, Weight filling, const Customer& c) {
    return f.capacity - filling >= c.demand;
}

// facility index => filling // need to know count of facilities
vector<double> facilityFilling(const std::vector<Customer>& customers,
                               const std::vector<FacilityIndex>& customerFacilities,
                               Count facility_count) {
    vector<double> filling(facility_count, 0);
    for (auto i = 0; i < customers.size(); ++i) {
        filling[customerFacilities[i]] += customers[i].demand;
    }
    return filling;
}


// if no facility found return lastIndex
FacilityIndex nearestCompatibleFacility(const std::vector<Facility>& facilities, 
                                        const std::vector<double>& facility_filling,
                                        const Customer& customer) {
    FacilityIndex best_i = facilities.size();
    double best_distance = std::numeric_limits<double>::max(), distance;
    for (auto i = 0; i < facilities.size(); ++i) {
        if (facility_filling[i] > 0 && facilities[i].capacity >= facility_filling[i] + customer.demand &&  
            (distance = ::distance(facilities[i], customer)) < best_distance) {
            best_distance = distance;
            best_i = i;
        }
    }
    return best_i;
}



struct MFL_Solver {
    
    static double objective(const std::vector<Customer>& customers, 
                            const std::vector<Facility>& facilities,
                            const std::vector<FacilityIndex>& customerFacilites) {
        double obj = 0;
        std::vector<bool> openedFacilities(facilities.size(), false);
        for (CustomerIndex c_i = 0; c_i < customers.size(); c_i++) {
            obj += Point::distance(customers[c_i].location, 
                                   facilities[customerFacilites[c_i]].location);
            openedFacilities[customerFacilites[c_i]] = true;
        }  
        for (auto f_i = 0; f_i < facilities.size(); ++f_i) {
            if (openedFacilities[f_i]) obj += facilities[f_i].cost;
        }
        /*
         for (FacilityIndex f_i = 0; f_i < facilities.size(); f_i++) {
         if (openedFacilities[f_i]) obj += facilities[f_i].cost;
         }
         */
        return obj;
    } 
    
    // returns facility for each customer
    virtual std::vector<FacilityIndex> solve(const std::vector<Customer>& customers, 
                                             const std::vector<Facility>& facilities) = 0;
};

struct MFL_Improver {
    virtual std::vector<FacilityIndex> improve(const std::vector<Customer>& customers, 
                                               const std::vector<Facility>& facilities,
                                               const std::vector<FacilityIndex>& customerFacilities) = 0;
};

struct MFL_Greedy : MFL_Solver {
    std::vector<FacilityIndex> solve(const std::vector<Customer>& customers, 
                                     const std::vector<Facility>& facilities) {
        
        /*
         order all customers by increasing demand 
         for every customer assign nearest possible facility 
         
         while possible take facility with smallest number of customers and try to empty it
         
         */        
        auto cmp_customers = [&](CustomerIndex i_0, CustomerIndex i_1) {
            return //i_0 < i_1;
                    customers[i_0].demand <  customers[i_1].demand || 
                   (customers[i_0].demand == customers[i_1].demand && i_0 < i_1);
        };
        std::set<CustomerIndex, decltype(cmp_customers)> unassignedCustomers(cmp_customers); 
        for (CustomerIndex i = 0; i < customers.size(); i++) {
            unassignedCustomers.insert(i);
        }
        
        std::map<int, int> m;
        m.emplace(std::pair<int, int>(1,1));
        
        std::vector<Weight> facilityFilling(facilities.size(), 0);
        std::vector<FacilityIndex> customerFacilities(customers.size(), 1);
        std::vector<FacilityIndex> facilityIndices(facilities.size());
        std::iota(facilityIndices.begin(), facilityIndices.end(), 0);
        
        Index k = 0; // nearest neighbor to use
        Index c = 0;
        for (; unassignedCustomers.size() && k != facilities.size(); k++) {
            std::stack<CustomerIndex> assignedCustomers;
            for (auto i : unassignedCustomers) {
                nth_element(
                            facilityIndices.begin(), 
                            facilityIndices.begin()+k, 
                            facilityIndices.end(), 
                            [&, i](FacilityIndex i_0, FacilityIndex i_1) {
                                const Facility 
                                    &f_0 = facilities[i_0], 
                                    &f_1 = facilities[i_1];
                                const Customer 
                                    &c = customers[i];
                                return 
                                    // trying to take cheap facilities first?
                                    distance(f_0, c) + f_0.cost*c.demand/f_0.capacity <
                                    distance(f_1, c) + f_1.cost*c.demand/f_1.capacity;
                                        
                                
//                                    Point::distance(facilities[i_0].location, customers[i].location) < 
//                                    Point::distance(facilities[i_1].location, customers[i].location);
                            });
                FacilityIndex f_i = facilityIndices[k];
                if (facilities[f_i].capacity - facilityFilling[f_i] >= customers[i].demand) {
                    c++;
                    //cout << i << " : " << f_i << endl;
                    customerFacilities[i] = f_i;
                    facilityFilling[f_i] += customers[i].demand;
                    assignedCustomers.push(i);
                }   
            }
            while (assignedCustomers.size()) {
                unassignedCustomers.erase(assignedCustomers.top());
                assignedCustomers.pop();
            }
        }
        if (k == facilities.size()) std::cout << "oops" << std::endl;
        
        return customerFacilities;
    }
};


struct MFL_GreedyClosing : MFL_Improver {
    
    struct FacilityToClose {
        FacilityIndex index;
        std::map<CustomerIndex, FacilityIndex> redistribution;
    };
    
    ant::CountMap<FacilityIndex> facilityFilling;
    // do i really want to store the whole vector???
    std::vector<FacilityIndex> currentCustomerFacilities;
    const std::vector<Customer>* customers;
    const std::vector<Facility>* facilities;
    std::vector<CustomerIndex> droppedCustomers;
    std::vector<CustomerIndex> assignedCustomers;
    double currentObjective;
    
    
    void dropFacility(FacilityIndex f_i) {
        for (auto c_i = 0; c_i < customers->size(); ++c_i) {
            if (currentCustomerFacilities[c_i] == f_i) {
                droppedCustomers.push_back((uint)c_i);
                currentObjective -= distance(customers->at(c_i), facilities->at(f_i));
            }
        }
        facilityFilling.erase(f_i);
        currentObjective -= facilities->at(f_i).cost;
    }
    
    void backupFacility(FacilityIndex f_i) {
        auto &cs = *customers;
        auto &fs = *facilities;
        for (auto c_i : assignedCustomers) {
            auto f_a = currentCustomerFacilities[c_i];
            facilityFilling.decrease(f_a, cs[c_i].demand);
            currentObjective -= distance(cs[c_i], fs[f_a]);
            
            currentCustomerFacilities[c_i] = f_i;
        }
        for (auto c_i : droppedCustomers) {
            facilityFilling.increase(f_i, cs[c_i].demand);
            currentObjective += distance(cs[c_i], fs[f_i]);
        }
        assignedCustomers.clear();
        droppedCustomers.clear();
        currentObjective += fs[f_i].cost;
    }
    
    void assignCustomerToFacility(CustomerIndex c_i, FacilityIndex f_i) {
        currentCustomerFacilities[c_i] = f_i;
        facilityFilling.increase(f_i, customers->at(c_i).demand); 
        currentObjective += distance(customers->at(c_i), facilities->at(f_i));
        assignedCustomers.push_back(c_i);
    }
    
    template<class ForwardIterator>
    ForwardIterator findNearestCompatibleFacility(
                                                  ForwardIterator first, 
                                                  ForwardIterator last, 
                                                  CustomerIndex c_i) {
        auto &c = customers->at(c_i);
        auto &fs = *facilities;
        auto m = std::min_element(first, last, 
                                  [&](FacilityIndex i_0, FacilityIndex i_1) {
                                      return isCompatible(fs[i_0], (uint)facilityFilling[i_0], c) && 
                                      (!isCompatible(fs[i_1], (uint)facilityFilling[i_1], c) ||
                                       distance(fs[i_0], c) < 
                                       distance(fs[i_1], c));
                                  });
        if (!isCompatible(fs[*m], (uint)facilityFilling[*m], c)) {
            m = last;
        }
        return m;
    }
    
    std::vector<FacilityIndex> improve(const std::vector<Customer>& customers, 
                                       const std::vector<Facility>& facilities,
                                       const std::vector<FacilityIndex>& customerFacilities) override {
        facilityFilling.clear();
        for (CustomerIndex i = 0; i < customers.size(); i++) {
            facilityFilling.increase(customerFacilities[i], customers[i].demand); 
        }
        currentObjective = MFL_Solver::objective(customers, facilities, customerFacilities);
        currentCustomerFacilities = customerFacilities;
        this->customers = &customers;
        this->facilities = &facilities;
        FacilityToClose best_f_to_close;
        Count closedCount = 0;
        double bestObjective = currentObjective;
        while(true) {
            bool f_to_close_found = false;
            // find out what's the best facility to close
            for (auto f_i : facilityFilling.keys()) {
                /*
                 std::cout << "opened facilities: ";
                 for (auto f_i : facilityFilling.keys()) {
                 std::cout << f_i << " ";
                 }
                 std::cout << "\n";
                 std::cout << "customer facilities: ";
                 for (auto c_i : ant::irange(customers.size())) {
                 std::cout << c_i << " : " << customerFacilities[c_i] << ", ";
                 }
                 std::cout << "\n\n";
                 std::cout.flush();
                 */
                dropFacility(f_i);          
                bool can_close = true;  
                auto ff = facilityFilling.keys();
                for (auto c_i : droppedCustomers) {
                    auto c_f = findNearestCompatibleFacility(ff.begin(), ff.end(), c_i);
                    if (c_f == ff.end()) {
                        can_close = false;
                        break;
                    }
                    else {
                        assignCustomerToFacility(c_i, *c_f);
                        if (currentObjective >= bestObjective) {
                            can_close = false;
                            break;
                        }
                    }
                }
                if (can_close) {
                    f_to_close_found = true;
                    bestObjective = currentObjective;
                    best_f_to_close.index = f_i;
                    best_f_to_close.redistribution.clear();
                    //std::cout << assignedCustomers.size() << std::endl;
                    for (auto c_i : assignedCustomers) {
                        best_f_to_close.redistribution[c_i] = currentCustomerFacilities[c_i];
                    }
                    backupFacility(f_i);
                    break;
                }
                backupFacility(f_i);
                assert(f_to_close_found || std::abs(currentObjective - bestObjective) < 1.e-3);
            }
            if (f_to_close_found) {
                //std::cout << "+1" << std::endl;
                dropFacility(best_f_to_close.index);
                //std::cout << best_f_to_close.redistribution.size() << std::endl; 
                for (auto p : best_f_to_close.redistribution) {
                    assignCustomerToFacility(p.first, p.second);
                }
                droppedCustomers.clear();
                assignedCustomers.clear();
                closedCount++;
                if (closedCount%100 == 0) std::cout << currentObjective << std::endl;
                assert(std::abs(currentObjective - bestObjective) < 1.e-3);
            }
            else break;
        } 
        
        return currentCustomerFacilities;
    }
    
};


struct MFL_Nearest {
    
    std::vector<FacilityIndex> improve(const std::vector<Customer>& customers, 
                                       const std::vector<Facility>& facilities,
                                       const std::vector<FacilityIndex>& customerFacilities) {
        vector<double> facility_filling = facilityFilling(customers, customerFacilities, facilities.size());
        vector<FacilityIndex> customer_facilities = customerFacilities;
        start_again:
        for (auto c_i = 0; c_i < customers.size(); ++c_i) {
            auto f_i = customer_facilities[c_i];
            for (auto f_i_new = 0; f_i_new < facilities.size(); ++f_i_new) {
                if (facility_filling[f_i_new] > 0 && 
                    distance(customers[c_i], facilities[f_i]) > distance(customers[c_i], facilities[f_i_new]) &&
                    facility_filling[f_i_new] + customers[c_i].demand <= facilities[f_i_new].capacity) {
                        
                    facility_filling[f_i_new] += customers[c_i].demand;
                    facility_filling[f_i] -= customers[c_i].demand;
                    customer_facilities[c_i] = f_i_new;
                    goto start_again;
                }
            }
        }
        return customer_facilities;
    }

};



#endif /* defined(__FacilityLocation__MFL__) */
