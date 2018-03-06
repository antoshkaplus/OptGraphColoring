//
//  TSP_LocalSearch.h
//  TravelingSalesman
//
//  Created by Anton Logunov on 4/5/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __TravelingSalesman__TSP_LocalSearch__
#define __TravelingSalesman__TSP_LocalSearch__

#include <iostream>

#include "TSP.h"

//struct TSP_TwoOpt2 : TSP_Improver {
//    typedef size_t Index;
//    
//    double swapEpsilon;
//    
//    TSP_TwoOpt2() : swapEpsilon(1e-7) {}
//    
//    vector<City> improve(const vector<Point>& ps,
//                         const vector<City>& in_tour) override {
//        vector<City> tour = in_tour;
//        
//        function<Index(Index)> next = [&](Index i) { 
//            return (i+1)%tour.size(); 
//        };
//        function<double(Index, Index)> distance = [&](Index i0, Index i1) {
//            return ps[tour[i0]].distance(ps[tour[i1]]);
//        };
//        
//        
//        bool start_again;
//        do {
//            start_again = false;
//            Index a0 = 0, a1, b0, b1;
//            do {
//                a1 = prev(a0);
//                for (Index i = 0; i < tour.size(); i++) {
//                    double d_a = distance(a0, a1), d_ab_0;
//                    if (i != a0 && i != a1 && (d_ab_0 = distance(a0, i) < d_a)) {
//                        b1 = next(b0);
//                        if (d_a + distance(b0, b1) > d_ab_0 + distance(a1, b1) + swapEpsilon) {
//                            // need to know which side should i pick
//                            reverse(<#_BidirectionalIterator __first#>, <#_BidirectionalIterator __last#>)
//                        }
//                    }
//                }
//                // same thing with a1 = next(a0)
//                
//                
//            } while (true);
//            
//            
//            Index a1 = 0, a2, b1, b2;
//            while ((a2 = next(a1)) != tour.size()-1) { 
//                b1 = next(a2);
//                b2 = next(b1);
//                while (b1 != 0) {
//                    if (distance(a1, a2)+distance(b1, b2) > distance(a1, b1) + distance(a2, b2) + swapEpsilon) {
//                        reverse(tour.begin()+a2, tour.begin()+b1+1);
//                        reverseCount++;
//                        start_again = true;
//                    }
//                    b1 = b2;
//                    b2 = next(b1);
//                }
//                a1 = a2;
//            }
//        } while (start_again);
//        return tour;
//    }
//}; 


#endif /* defined(__TravelingSalesman__TSP_LocalSearch__) */
