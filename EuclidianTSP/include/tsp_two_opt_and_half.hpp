#pragma once

#include "tsp.hpp"


struct TSP_TwoOptAndHalf : TSP_Improver {
    typedef size_t Index;

    double swapEpsilon;

    TSP_TwoOptAndHalf() : swapEpsilon(1e-7) {}

    vector<City> improve(const vector<Point>& ps,
                         const vector<City>& in_tour) override {
        vector<City> tour = in_tour;
        function<Index(Index)> next = [&](Index i) {
            return (i+1)%tour.size();
        };
        function<double(Index, Index)> distance = [&](Index i0, Index i1) {
            return ps[tour[i0]].Distance(ps[tour[i1]]);
        };

        Index reverseCount = 0, printedReverseCount = 0;
        bool start_again;
        do {
            start_again = false;
            Index a0, a1 = 0, a2, b1, b2;
            while ((a2 = next(a1)) != tour.size()-1) {
                b1 = next(a2);
                b2 = next(b1);
                while (b1 != 0) {
                    if (distance(a1, a2) + distance(b1, b2) > distance(a1, b1) + distance(a2, b2) + swapEpsilon) {
                        reverse(tour.begin()+a2, tour.begin()+b1+1);
                        reverseCount++;
                        start_again = true;
                    }
                    else {
                        a0 = (a1 + tour.size() - 1)%tour.size();
                        if (distance(a0, a2) + distance(b1, a1) + distance(a1, b2) <
                            distance(a0, a1) + distance(a1, a2) + distance(b1, b2) - swapEpsilon) {
                            //cout << "before: " << perimeter(ps, tour, true);
                            City c = tour[a1];
                            tour.erase(tour.begin() + a1);
                            // cuz of erase shift
                            tour.insert(tour.begin() + b2-1, c);
                            //cout << " after: " << perimeter(ps, tour, true);
                        }
                    }
                    b1 = b2;
                    b2 = next(b1);
                }
                a1 = a2;
            }
            if (reverseCount > printedReverseCount+1000) {
                cout << "reverce count: " << reverseCount << " obj: " << Perimeter(ps, tour, true)  << endl;
                printedReverseCount = reverseCount;
            }
        } while (start_again);
        return tour;
    }

};