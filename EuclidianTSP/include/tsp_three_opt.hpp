#pragma once

#include "tsp.hpp"


struct TSP_ThreeOpt : TSP_Improver {

    double swapEpsilon;

    TSP_ThreeOpt() : swapEpsilon(1e-7) {}

    TSP_ThreeOpt(double swap_epsilon) {
        swapEpsilon = swap_epsilon;
    }

    vector<City> improve(const vector<Point>& ps,
                         const vector<City>& in_tour) override {
        vector<City> tour = in_tour;
        function<Index(Index)> next = [&](Index i) {
            return (i+1)%tour.size();
        };
        function<double(Index, Index)> distance = [&](Index i0, Index i1) {
            return ps[tour[i0]].Distance(ps[tour[i1]]);
        };
        bool start_again;
        do {
            start_again = false;
            Index a1 = 0, a2, b1, b2, c1, c2;
            while ((a2 = a1+1) != tour.size()-1-1) {
                b1 = a2;
                while ((b2 = b1+1) != tour.size()-1) {
                    c1 = b2;
                    // want to capture first-last edge
                    while ((c2 = (c1+1)%tour.size()) != 1) {
                        bool did_transform = true;
                        // 4 possible permutations
                        double d_a = distance(a1, a2),
                            d_b = distance(b1, b2),
                            d_c = distance(c1, c2),
                            total = d_a + d_b + d_c;
                        if (total > distance(a1, c1) + distance(a2, b2) + distance(b1, c2) + swapEpsilon) {
                            reverse(tour.begin()+a2, tour.begin()+b1+1);
                            reverse(tour.begin()+a2, tour.begin()+c1+1);
                            //cout << 0 << endl;
                        } else
                        if (total > distance(a1, b2) + distance(a2, c2) + distance(b1, c1) + swapEpsilon) {
                            reverse(tour.begin()+b2, tour.begin()+c1+1);
                            reverse(tour.begin()+a2, tour.begin()+c1+1);
                            //cout << 1 << endl;
                        } else
                        if (total > distance(a1, b2) + distance(a2, c1) + distance(b1, c2) + swapEpsilon) {
                            reverse(tour.begin()+a2, tour.begin()+b1+1);
                            // now we have variant 2 just copy paste
                            reverse(tour.begin()+b2, tour.begin()+c1+1);
                            reverse(tour.begin()+a2, tour.begin()+c1+1);
                            //cout << 2 << endl;
                        } else
                        if (total > distance(a1, b1) + distance(a2, c1) + distance(b2, c2) + swapEpsilon) {
                            reverse(tour.begin()+a2, tour.begin()+b1+1);
                            reverse(tour.begin()+b2, tour.begin()+c1+1);
                            //cout << 3 << endl;

                        } else did_transform = false;

                        if (did_transform) {
                            start_again = true;
                            //    cout << "obj: " << perimeter(ps, tour, true)  << endl;
                        }
                        c1 = c2;
                    }
                    b1 = b2;
                }
                a1 = a2;
            }
        } while(start_again);

        return tour;
    }
};
