#pragma once

#include "tsp.hpp


struct LinKernighan {

    Tour tour;
    const vector <Point>& ps;
    vector <vector<int>> nearestCities;
    int kNearest;
    int nCities;

    bool didTourChanged;
    vector <Edge> brokenEdges;
    vector <Edge> addedEdges;
    vector <Edge> segsLeft;
    time_t time;
    int maxOpt;
    int curOpt;
    double minGain;

    LinKernighan(const vector <Point>& ps, const vector<int>& cities,
                 int kNearest, int maxOpt, double minGain, time_t time)
        : tour(cities), ps(ps), nearestCities(cities.size()), kNearest(kNearest), nCities((int) cities.size()),
          time(time), maxOpt(maxOpt), minGain(minGain) {
        for (int i = 0; i < nCities; i++) {
            kNearestVertices(ps, ps[i], nearestCities[i], kNearest);
        }
    }

    bool canClose() {
        addedEdges.push_back(Edge(brokenEdges.front().first, brokenEdges.back().second));

        double gain = 0.;
        for (Edge& e : brokenEdges) {
            gain += ps[e.first].distance(ps[e.second]);
        }
        for (Edge& e : addedEdges) {
            gain -= ps[e.first].distance(ps[e.second]);
        }

        if (gain <= minGain) {
            addedEdges.pop_back();
            return false;
        }

        tour.segmentsAfterRemovingEdges(brokenEdges, segsLeft);
        if (!tour.canMutate(segsLeft, addedEdges)) {
            addedEdges.pop_back();
            return false;
        }
        //cout << "gain: " << gain << endl;
        addedEdges.pop_back();
        return true;
    }

    void close() {
        addedEdges.push_back(Edge(brokenEdges.front().first, brokenEdges.back().second));
        tour.mutate(segsLeft, addedEdges);
        // maybe can clear somewhere else
        brokenEdges.clear();
        addedEdges.clear();
        didTourChanged = true;
    }

    // need function for comparing edges!!!
    void go(int city1, int city2) {
        curOpt++;
        if (curOpt == maxOpt + 1) {
            curOpt--;
            return;
        }
        double d12 = ps[city1].distance(ps[city2]);
        for (int city3 : nearestCities[city2]) {
            /* 1) edge (city2, city3) shouldn't be in tour
             * 2) edge (city2, city3) shouldn't be added before
             * 3) edge (city2, city3) should be better than (city1, city3)
             */
            double d23 = ps[city2].distance(ps[city3]);
            Edge e23(city2, city3), e32(city3, city2);
            if (d23 < d12 && tour.prev(city2) != city3 && tour.next(city2) != city3 &&
                find(addedEdges.begin(), addedEdges.end(), e23) == addedEdges.end() &&
                find(addedEdges.begin(), addedEdges.end(), e32) == addedEdges.end() &&
                find_if(brokenEdges.begin(), brokenEdges.end(),
                        [city3](const Edge& e) { return e.first == city3 || e.second == city3; })
                == brokenEdges.end()) {
                addedEdges.push_back(e23);
                for (int city4 : {tour.prev(city3), tour.next(city3)}) {
                    Edge e34(city3, city4), e43(city4, city3);
                    if (find_if(brokenEdges.begin(), brokenEdges.end(),
                                [city4](const Edge& e) { return e.first == city4 || e.second == city4; })
                        == brokenEdges.end()) {
                        brokenEdges.push_back(e34);
                        if (canClose()) {
                            close();
                            curOpt--;
                            return;
                        }
                        go(city3, city4);
                        if (didTourChanged) {
                            curOpt--;
                            return;
                        }
                        brokenEdges.pop_back();
                    }
                }
                addedEdges.pop_back();
            } else if (d23 >= d12) {
                // following edges will be just equal or longer
                break;
            }
        }
        curOpt--;
    }

    vector<int> improve() {
        curOpt = 0;
        int i = 0;

        for (;;) {
            didTourChanged = false;
            int start = random() % nCities;
            int a1 = start, a2 = tour.next(a1);
            do {
                brokenEdges.push_back(Edge(a1, a2));
                go(a1, a2);
                if (didTourChanged) break;
                brokenEdges.pop_back();

                brokenEdges.push_back(Edge(a2, a1));
                go(a2, a1);
                if (didTourChanged) break;
                brokenEdges.pop_back();

                a1 = a2;
                a2 = tour.next(a1);
            } while (a1 != start);
            if ((i++ % 10) == 0) cout << totalTravellingDistance(tour.cities, ps) << "\n";

            if (!didTourChanged) break;
        }

        return tour.cities;
    }
};

