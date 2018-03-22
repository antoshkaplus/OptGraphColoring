#pragma once

#include <chrono>
#include <random>


#include "GC.hpp"


// Method can be found
// Genetic Algorithm Applied to the Graph Coloring Problem
// Musa M. Hindi and Roman V. Yampolskiy
struct GC_GA : GC {
private:    
    using Population = vector<vector<Color>>;
    using Coloring = vector<Color>;
    
    default_random_engine rng_{static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count())};
    
    vector<Edge> edges_;
    const Graph* graph_;
    
    uniform_int_distribution<int> node_distr_;
    uniform_real_distribution<> real_distr_;
    
    // buffer methods used in certain methods to avoid
    // allocating arrays every time method is used
    vector<bool> adjacent_colors_buffer_;
    vector<int> top_individuals_buffer_;
    vector<int> valid_colors_buffer_;
    
    Count population_size_ = 50;
    Count max_iteration_count_ = 20000;
    double top_portion_ = 0.5;
    double mutation_frequency = 0.7;
    
public:
    string name() override {
        return "ga";
    }

    void set_population_size(Count size) {
        population_size_ = size;
    }
    
    Count population_size() const {
        return population_size_;
    }
    
    void set_max_iteration_count(Count count) {
        max_iteration_count_ = count;
    }
    
    Count max_iteration_count() const {
        return max_iteration_count_;
    }
    
    
    ColoredGraph solve(const Graph& gr) override {
        node_distr_ = uniform_int_distribution<>(0, gr.nodeCount()-1);
        // always init edges first
        edges_.clear();
        graph::ForEachEdge(gr, [&](auto i, auto j) {
            edges_.emplace_back(i, j);
        });
        graph_ = &gr;
        
        GC_Naive_2 naive;
        ColoredGraph c_gr = naive.solve(gr); 
        Count color_count = c_gr.colorCount();
        int node_count = gr.nodeCount();
        Coloring coloring(node_count);
        for (auto i = 0; i < node_count; ++i) {
            coloring[i] = c_gr.color(i);
        }
        
        // has time
        while (true) {
            --color_count;
            adjacent_colors_buffer_.resize(color_count);
            auto p_0 = randomPopulation(population_size_, node_count, color_count);
            // lowest violation count
            auto v_0 = countViolations(p_0);
            auto p_1 = p_0;
            auto v_1 = v_0;
            
            int best_fitness;
            for (int i = 0; i < max_iteration_count_; ++i) {
                best_fitness = *min_element(v_0.begin(), v_0.end());
                if (best_fitness == 0) {
                    break;
                }
                if (best_fitness > 4) {
                    nextGeneration_1(p_0, v_0, p_1);
                } else {
                    nextGeneration_2(p_0, v_0, p_1);
                }
                v_1 = countViolations(p_1);
                swap(p_0, p_1);
                swap(v_0, v_1);
            }

            if (best_fitness != 0) {
                break;
            }
            
            int i = min_element(v_0.begin(), v_0.end()) - v_0.begin();
            coloring = p_0[i];
        }
        return ColoredGraph(gr, coloring);
    }


private:

    vector<int> countViolations(const Population& populaiton) {
        vector<int> vs(populaiton.size());
        GC_GA& gc_ga = *this;
        transform(populaiton.begin(), populaiton.end(), vs.begin(), 
                  [&] (const Coloring& cs) { return gc_ga.countViolations(cs); });
        return vs;
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

    void crossover(Coloring& child, const Coloring& p_0, const Coloring& p_1) {
        int crosspoint = randomNode();
        copy(p_0.begin(), p_0.begin() + crosspoint, child.begin());
        copy(p_1.begin() + crosspoint, p_1.end(), child.begin() + crosspoint);
    }
    
    
    // passing as arguments to avoid allocating memory
    void nextGeneration_1(const Population& population, const vector<int>& violations,   
                          Population& next_population) {
        auto fittest = [&] (int i_0, int i_1) {
            return violations[i_0] < violations[i_1] ? i_0 : i_1;
        };
        uniform_int_distribution<int> indices(0, population.size()-1);
        for (int i = 0; i < population.size(); ++i) {
            
            int p_1 = fittest(indices(rng_), indices(rng_));
            int p_2 = fittest(indices(rng_), indices(rng_));
            
            auto& child = next_population[i];
            crossover(child, population[p_1], population[p_2]);
            if (real_distr_(rng_) < mutation_frequency) {
                mutation_1(child);
            } 
        }
    }

    // returns number of violations
    void mutation_1(Coloring& child) {
        auto& colors = adjacent_colors_buffer_;
        vector<Index> inds(edges_.size());
        iota(inds.begin(), inds.end(), 0);

        shuffle(inds.begin(), inds.end(), rng_);
        
        //for (auto e : edges_) {
        bool changed = false;
        
        for (int i : inds) {
            auto e = edges_[i];
            if (child[e.first] != child[e.second]) continue;
            
            for (auto i : (real_distr_(rng_) < 0.5 ? vector<int>{e.first, e.second} : vector<int>{e.second, e.first})) {
                std::fill(colors.begin(), 
                          colors.end(), 
                          false);
                
                for (auto k : graph_->nextNodes(i)) {
                    colors[child[k]] = true;
                }
                auto& valid = valid_colors_buffer_;
                valid.clear();
                for (int c = 0; c < colors.size(); ++c) {
                    if (!colors[c]) {
                        valid.push_back(c);
                    }
                }
                if (!valid.empty()) {
                    changed = true;
                    uniform_int_distribution<> d(0, valid.size()-1);
                    child[i] = valid[d(rng_)];
                    break;
                }
            }
        }
        if (!changed && real_distr_(rng_) < 0.7) {
            mutation_2(child);
        }
    }
    

    // used when best fitness 4 or less violations 
    
    void nextGeneration_2(const Population& population, const vector<int>& violations,   
                          Population& next_population) {
        next_population = population;
        int i = min_element(violations.begin(), violations.end()) - violations.begin();
        mutation_2(next_population[i]);

//        auto fittest = [&] (int i_0, int i_1) {
//            return violations[i_0] < violations[i_1] ? i_0 : i_1;
//        };
//        auto& top = top_individuals_buffer_;
//        top.resize(population.size());
//        iota(top.begin(), top.end(), 0);
//        int top_count = top_portion_ * population.size();
//        partial_sort(top.begin(), 
//                     top.begin() + top_count, 
//                     top.end(), fittest);
//        top.erase(top.begin() + top_count, top.end());
//        
////        top_count = 1;
////        while (violations[top_count-1] == violations[top_count]) top_count++;
//        
//        uniform_int_distribution<int> indices(0, top_count-1);
//        for (int i = 0; i < population.size(); ++i) {
//            // parent selection
//            int p = top[indices(rng_)];
//            next_population[i] = population[p];
//            if (real_distr_(rng_) < mutation_frequency) {
//                mutation_2(next_population[i]);
//            }
//        }
    }
    
    void mutation_2(Coloring& child) {
        // can think about putting color_count as member variable
        uniform_int_distribution<int> color(0, adjacent_colors_buffer_.size()-1);
        for (auto e : edges_) {
            if (child[e.first] != child[e.second]) continue;
            child[e.first] = color(rng_);
            child[e.second] = color(rng_);
        }
    }

    Index randomNode() {
        return node_distr_(rng_);
    }

    // don't make them static to avoid creating static member variables
    
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
   
};