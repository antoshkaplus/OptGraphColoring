
#pragma once

#include <iostream>
#include <armadillo>

#include "tsp.hpp"
#include "tsp_nearest_neighbor.hpp"

/* algorithm described in : 
 * "Ant colonies for the traveling salesman problem" 
 * Marco Dorigo, Luca Maria Gambardella
 **/
struct TSP_AntColony : TSP_Solver {

    struct Ant {

        const TSP_AntColony *solver;

        vector<City>    tour;
        vector<City>    unvisitedCities;
        vector<double>  unvisitedCityValues;

        Count cityCount;

        void setSolver(const TSP_AntColony *solver) {
            this->solver = solver;
            cityCount = solver->cityCount;
            // allocate memory for the problem
            unvisitedCities.reserve(cityCount);
            tour.reserve(cityCount);
            unvisitedCityValues.reserve(cityCount);
        }

        void prepareForTourConstruction() {
            tour.resize(0);
            unvisitedCities.resize(cityCount);
            iota(unvisitedCities.begin(), unvisitedCities.end(), 0);
        }
    private:
        // index in unvisitedCities vector
        // where is local pheromone update?
        void visit(Index i) {
            tour.push_back(unvisitedCities[i]);
            swap(unvisitedCities[i], unvisitedCities.back());
            unvisitedCities.pop_back();
        }
    public:
        void visitNextCity() {
            if (unvisitedCities.size() == cityCount) {
                visit(random(cityCount));
                return;
            }
            // localizing some vars
            auto &pheromone         = solver->pheromone;
            auto &distance          = solver->distance;
            auto parPheromone       = solver->parPheromone;
            auto parDistance        = solver->parDistance;
            auto parTakingBestEdge  = solver->parTakingBestEdge;

            unvisitedCityValues.resize(unvisitedCities.size());
            for (Index i = 0; i < unvisitedCities.size(); i++) {
                unvisitedCityValues[i] = pow(pheromone.at((uint)tour.back(),
                                              (uint)unvisitedCities[i]), parPheromone)
                                         *
                                         pow( distance.at((uint)tour.back(),
                                              (uint)unvisitedCities[i])/1000.,-parDistance );
            }

            double r = random();
            // can we just take best edge???
            if (r < parTakingBestEdge) {
                Index i, i_max = 0;
                for (i = 1; i < unvisitedCities.size(); i++) {
                    if (unvisitedCityValues[i] > unvisitedCityValues[i_max]) {
                        i_max = i;
                    }
                }
                visit(i_max);
            } else { // no // will choose edge by probability
                double valueSum = accumulate(unvisitedCityValues.begin(),
                                             unvisitedCityValues.end(), 0.);
                for (auto& v : unvisitedCityValues) {
                    v /= valueSum;
                }

                Index i = 0;
                double pr = random();
                double cumulative = unvisitedCityValues[i];
                assert(pr != 0. && pr != 1.);
                while (pr > cumulative) cumulative += unvisitedCityValues[++i];
                assert(i < unvisitedCities.size());
                visit(i);
            }
        }
    };

    typedef vector<vector<City>> AntTours;

    Count cityCount;
    
    arma::Mat<double> pheromone;
    arma::Mat<double> distance;
    
    vector<City> shortestTour;
    double shortestTourLength;
    
    double parPheromone;        // alpha    1
    double parDistance;         // beta     2
    double parEvaporation;      // rho      0.1
    double parTakingBestEdge;   // q_0      0.9 
    double parLocalTrail;       // n*Lnn - cityCount/(Length of nearest neighbor algorithm
    
    Count antCount;             // m        10
    vector<Ant> ants;
    
    Index currentIteration;
    size_t iterationCount;
    const vector<Point> *points;
    
    
    // ants should be already ready
    void constructAntsTours() {
        for (auto& a : ants) {
            a.prepareForTourConstruction();
        }
        
        // don't forget first-last city connection 
        for (Index i = 0; i <= cityCount; i++) {
            for (Index i_ant = 0; i_ant < ants.size(); i_ant++) {
                if (i != cityCount) ants[i_ant].visitNextCity();
                if (i != 0) {
                    City c_0, c_1;
                    c_0 = ants[i_ant].tour[i-1]; // prev city
                    c_1 = ants[i_ant].tour[i == cityCount ? 0 : i];
                    // symmetric matrix
                    double &ph_0 = pheromone.at((uint)c_0, (uint)c_1),
                           &ph_1 = pheromone.at((uint)c_1, (uint)c_0); 
                    ph_0 = ph_1 = (1.-parEvaporation)*ph_0 + 
                                      parEvaporation *parLocalTrail;
                }
            }
        }
        
        // for each ant you make local search to know best ant
        TSP_TwoOpt imp;
        TSP_ThreeOpt improver;
//        for (auto& a : ants) {
//            //a.tour = improver.improve(*points, a.tour);
//        }
    }
    
    
    void handleAntTours() {
        const vector<Point> &ps = *points; 
        TSP_TwoOpt opt;
        for (auto& a : ants) {
            a.tour = opt.improve(ps, a.tour);
        }
        Index best_ant = 0;
        double best_length = Perimeter(ps, ants[best_ant].tour, true);
        double length;
        for (Index i_ant = 1; i_ant < ants.size(); i_ant++) {
            length = Perimeter(ps, ants[i_ant].tour, true);
            if (length < best_length) {
                best_ant = i_ant;
                best_length = length;
            }
        }
        
        if (best_length < shortestTourLength) {
            shortestTourLength = best_length;
            shortestTour = ants[best_ant].tour;
            cout << "it: "         << currentIteration 
                 << ",  new obj: " << shortestTourLength << endl;
        }
        
        // global trail update
        const auto &tour = ants[best_ant].tour; 
        auto &ph   = pheromone;
        for (Index i = 0; i < cityCount; i++) {
            City c_0 = tour[i],
                 c_1 = tour[(i+1)%cityCount];
            ph.at((uint)c_0, (uint)c_1) = ph.at((uint)c_1, (uint)c_0) = 
                (1-parEvaporation)*ph.at((uint)c_0, (uint)c_1) +
                   parEvaporation *(1./best_length);
        }
    }
    
    
    vector<City> solve(const vector<Point>& points) override {
        cityCount       = (Count)points.size();
        this->points    = &points; 
        TSP_NearestNeighbor solver;
        shortestTour       = solver.solve(points);
        shortestTourLength = Perimeter(points, shortestTour, true);

        cout << "start solution: " << shortestTourLength << endl;

        parPheromone        = 1.;        
        parDistance         = 2.;
        parEvaporation      = 0.1;
        parTakingBestEdge   = 0.9; 
        parLocalTrail       = cityCount/shortestTourLength;
        
        // if there are not many cities (edges) too many ants can evaporate edges too quickly
        antCount = 10;
        
        iterationCount = 100; 
    
        pheromone.resize(cityCount, cityCount);
        distance.resize(cityCount, cityCount);
        pheromone.fill(parLocalTrail);
        for (uint i_0 = 0; i_0 < cityCount; i_0++) {
            for (uint i_1 = i_0; i_1 < cityCount; i_1++) {
                distance(i_0, i_1) = distance(i_1, i_0) = points[i_0].Distance(points[i_1]);
            }
        }
        ants.resize(antCount);
        for (auto &a : ants) a.setSolver(this);
        
        currentIteration = 0;
        while (currentIteration++ < iterationCount) {
            constructAntsTours();
            handleAntTours();
        }
        
        return shortestTour;
    }
    
};
