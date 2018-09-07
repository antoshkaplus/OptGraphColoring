#pragma once

#include "tsp.hpp


template<class Tour>
struct LinKernighanOld {

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

    LinKernighanOld(const vector <Point>& ps, const vector<int>& cities,
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


    vector<Index> ts;

    bool CanClose() {
        ts.push_back(ts[0]);

        // for each ts index another index of the broken edge
        vector<Index> broken(ts.size() / 2);
        for (auto i = 0; i < ts.size(); i += 2) {
            broken[i] = i+1;
            broken[i+1] = i; // pretty straight forward
        }

        // for index i find broken index : (i % 2 == 0) ? i+1 : i-1

        // for new edges : (i % 2 == 0) ? i-1 : i+1

        // for y care for i-1 as it can slip out of bounds
        // for x care for i+1 as it can slip out of bounds

        // prevent by using additional first and last elements???

        // only indices should be sorted
        vector<Index> ts_order;

        sort(ts_order.begin(), ts_order.end(), bind(&Tour::LineOrdered, tour)); // if not line ordered it will swap the items.
        // 0 - prev, 1 - next
        vector<Index> ts_left(ts.size());
        // just have to check first element if broken
        for (auto i = 0; i < ts.size(); ) {
            if (!broken(ts_order[i], ts_order[i+1])) {
                ts_left[ts_order[i]] = ts_order[i+1];
                ts_left[ts_order[i+1]] = ts_order[i];
                i += 2;
            }
            // if broken just go to the next one
        }

        // so now I have both added and left. is path exists

        vector<bool> ts_visited(ts.size(), false);

        auto t_index = 0;
        for (auto i = 0; i < ts.size() / 2; ++i) {
            if (ts_visited[t_index]) return false;

            ts_visited[t_index] = true;
            t_index = ts_left[t_index];
            ts_visited[t_index] = true;
            t_index = added(t_index);
        }
        return true;
    }

    // again we have ts vector + tour. need to close it.
    void Close() {

    }

};

