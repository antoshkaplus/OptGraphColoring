// 
// GA applyed to the Graph Coloring Problem
// authors: Musa M. Hindi and Roman V. Yampolskiy
//

#ifndef __GC_GA_H__
#define __GC_GA_H__

#include <chrono>
#include <random>


#include "GC.h"

struct GC_GA : GC {
    
    using Population = vector<vector<Color>>;
    using Coloring = vector<Color>;
    
    ColoredGraph solve(const Graph& gr) override {
        // always init edges first
        edges_ = gr.edges();
        graph_ = &gr;
        GC_Naive_2 naive;
        ColoredGraph c_gr = naive.solve(gr); 
        int color_count = c_gr.colorCount();
        int node_count = gr.nodeCount;
        Coloring coloring = c_gr.coloring();
        
        // has time
        int population_size = 50;
        while (true) {
            --color_count;
            auto p_0 = randomPopulation(population_size, node_count, color_count);
            // lowest violation count
            auto v_0 = countViolations(p_0);
            auto p_1 = p_0;
            auto v_1 = v_0;
            
            auto* population = &p_0;
            auto* violations = &v_0;
            auto* next_population = population;
            auto* next_violations = violations;
            
            int best_fitness;
            for (int i = 0; i < 20000; ++i) {
                best_fitness = *min_element(violations->begin(), violations->end());
                if (best_fitness == 0) {
                    break;
                }
                if (best_fitness > 4) {
                    nextGeneration(*population, *violations, next_population, next_violations, color_count);
                } else {
                    nextGeneration_2(*population, *violations, next_population, next_violations, color_count);
                }
                swap(population, next_population);
                swap(violations, next_violations);
            }
            
            if (best_fitness != 0) {
                break;
            }
            
            int i = min_element(violations->begin(), violations->end()) - violations->begin();
            coloring = (*population)[i];
        }
        return ColoredGraph(gr.adjacencyList, coloring);
    }
    
    
    
    
private:

    int populationMinViolationCount(const Population& population) {
        int min = numeric_limits<int>::max();
        for (auto& p : population) {
            int v = countViolations(p);
            if (v < min) {
                min = v;
            }
        }
        return min;
    }


    int countViolations(const Coloring& coloring) {
        int violation_count = 0;
        for (auto& e : edges_) {
            if (coloring[e.first] == coloring[e.second]) {
                ++violation_count;
            }
        }
        return violation_count;
    }

    vector<int> countViolations(const Population& populaiton) {
        vector<int> vs(populaiton.size());
        GC_GA& gc_ga = *this;
        transform(populaiton.begin(), populaiton.end(), vs.begin(), 
                  [&] (const Coloring& cs) { return gc_ga.countViolations(cs); });
        return vs;
    }
    
    vector<bool> adjacent_colors_;
    void nextGeneration(const Population& population, const vector<int>& violations,   
                        Population* next_population, vector<int>* next_violations, int color_count) {
        auto fittest = [&] (int i_0, int i_1) {
            return violations[i_0] < violations[i_1] ? i_0 : i_1;
        };
        uniform_int_distribution<int> indices(0, population.size()-1);
        uniform_int_distribution<int> node(0, population.front().size()-1);
        for (int i = 0; i < population.size(); ++i) {
            // parent selection
            int p_1 = fittest(indices(rng_), indices(rng_));
            int p_2 = fittest(indices(rng_), indices(rng_));
            // crossover
            int crosspoint = node(rng_);
            Coloring& child = (*next_population)[i];
            copy(population[p_1].begin(), population[p_1].begin() + crosspoint, child.begin());
            copy(population[p_2].begin() + crosspoint, population[p_2].end(), child.begin() + crosspoint);
            // mutation
            int viol = 0;
            for (auto e : edges_) {
                if (child[e.first] != child[e.second]) continue;
                adjacent_colors_.resize(color_count);
                for (auto i : {e.first, e.second}) {
                    std::fill(adjacent_colors_.begin(), adjacent_colors_.end(), false);
                    for (auto k : graph_->adjacencyList[i]) {
                        adjacent_colors_[child[k]] = true;
                    }
                    for (int c = 0; c < color_count; ++c) {
                        if (!adjacent_colors_[c]) {
                            child[i] = c;
                            goto next;
                        }
                    }
                }
                // can't find any color (((
                // if could reach this
                // can actually count violations right here
                ++viol; 
                next:;
            }
            (*next_violations)[i] = viol;
        }
    }

    // used when best fitness 4 or less violations 
    vector<int> top_individuals_;
    void nextGeneration_2(const Population& population, const vector<int>& violations,   
                          Population* next_population, vector<int>* next_violations, int color_count) {
        auto fittest = [&] (int i_0, int i_1) {
            return violations[i_0] < violations[i_1] ? i_0 : i_1;
        };
        int node_count = population.front().size();
        top_individuals_.resize(population.size());
        iota(top_individuals_.begin(), top_individuals_.end(), 0);
        int top_count = population.size()/3;
        partial_sort(top_individuals_.begin(), 
                     top_individuals_.begin() + top_count, 
                     top_individuals_.end(), fittest);
        top_individuals_.erase(top_individuals_.begin() + top_count, 
                              top_individuals_.end());
        
        uniform_int_distribution<int> indices(0, top_count-1);
        uniform_int_distribution<int> node(0, population.front().size()-1);
        uniform_int_distribution<int> color(0, color_count-1);
        for (int i = 0; i < population.size(); ++i) {
            // parent selection
            int p_1 = fittest(top_individuals_[indices(rng_)], top_individuals_[indices(rng_)]);
            int p_2 = fittest(top_individuals_[indices(rng_)], top_individuals_[indices(rng_)]);
            // crossover
            int crosspoint = node(rng_);
            Coloring& child = (*next_population)[i];
            copy(population[p_1].begin(), population[p_1].begin() + crosspoint, child.begin());
            copy(population[p_2].begin() + crosspoint, population[p_2].end(), child.begin() + crosspoint);
            // mutation
            for (auto e : edges_) {
                if (child[e.first] != child[e.second]) continue;
                child[e.first] = color(rng_);
                child[e.second] = color(rng_);
            }
            (*next_violations)[i] = countViolations(child); 
        }
    }

    vector<Color> randomColoring(int node_count, int color_count) {
        vector<Color> cs(node_count);
        uniform_int_distribution<int> color_distr(0, color_count-1);
        for (auto& c : cs) {
            c = color_distr(rng_);
        }
        return cs;
    }
    
    vector<vector<Color>> randomPopulation(int population_size, int node_count, int color_count) {
        vector<vector<Color>> population(population_size);
        for (auto& p : population) {
            p = randomColoring(node_count, color_count);
        }
        return population;
    }
    
    default_random_engine rng_{static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count())};
    
    vector<Edge> edges_;
    const Graph* graph_;
}; 


#endif
