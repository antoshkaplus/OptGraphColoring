#pragma once

#include <chrono>
#include <random>
#include <algorithm>

#include "GC.hpp"


// Method can be found
// Genetic Algorithm Applied to the Graph Coloring Problem
// Musa M. Hindi and Roman V. Yampolskiy

// Chromosome - coloring of the graph
struct GC_GA : GC {
private:

    //using Population = vector<vector<Color>>;
    using Coloring = vector<Color>;

    struct Sample {
        vector<Color> coloring;
        int violations = 0;
        int iteration = 0;

        bool operator<(const Sample& s) const {
            return violations < s.violations || (violations == s.violations && iteration > s.iteration);
        }

        bool operator==(const Sample& s) const {
            return violations == s.violations && coloring == s.coloring;
        }
    };

    class Population {
        using It = vector<Sample>::iterator;

        vector<Sample> samples;
        int capacity_;
        int children_start = 0;

    public:

        Population() = default;

        Population(int node_count, int capacity) : samples(capacity), capacity_(capacity) {
            for (auto& s : samples) {
                s.coloring.resize(node_count);
            }
        }

        int capacity() const {
            return capacity_;
        }

        int size() const {
            return children_start;
        }

        std::vector<Sample>::iterator begin() {
            return samples.begin();
        }
        std::vector<Sample>::const_iterator begin() const {
            return samples.begin();
        }

        std::vector<Sample>::iterator end() {
            return begin() + children_start;
        }
        std::vector<Sample>::const_iterator end() const {
            return begin() + children_start;
        }

        void Reset() {
            children_start = 0;
        }

        template<class Func>
        void MakeChildren(Func&& func) {
            for (auto i = children_start; i < capacity_; ++i) {
                func(samples[i]);
            }
            children_start = capacity_;
        }

        void Merge(const Population& population) {
            samples.resize(children_start + population.size());
            copy(population.begin(), population.end(), samples.begin()+children_start);
            sort(samples.begin(), samples.end());
            samples.resize(capacity());

            children_start = capacity();
        }

        void Sort() {
            sort(samples.begin(), samples.end());
        }

        template<class Func>
        void ForEach(Func&& func) {
            for (auto i = 0; i < children_start; ++i) {
                func(samples[i]);
            }
        }

        int RemoveBottomHalf() {
            if (children_start != capacity_) return std::numeric_limits<int>::max();

            sort(samples.begin(), samples.end());
//
//            sort(samples.begin(), samples.end(), [](auto& s_1, auto& s_2) {
//                return s_1.violations < s_2.violations;
//            });

            children_start = capacity_ / 2;
            return samples[children_start].violations;
        }

        const Sample& operator[](int i) const {
            assert(i < children_start);
            return samples[i];
        }

        const Sample& MinViolations() const {
            return *min_element(begin(), end());
        }
    };

    struct Params {
        std::experimental::optional<double> mutation_frequency;
        std::experimental::optional<double> population_size;
        std::experimental::optional<bool> one_child_per_couple;
        std::experimental::optional<bool> continious_mutation;
    };

    default_random_engine rng_{static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count())};
    
    const Graph* graph_;
    
    uniform_int_distribution<int> node_distr_;
    uniform_real_distribution<> real_distr_;
    
    Count population_size_ = 100;
    Count max_iteration_count_ = 20000;
    double top_portion_ = 0.5;
    double mutation_frequency = 0.5;
    // try to increase diversity in the new generation
    bool one_child_per_couple = false;
    bool continious_mutation = false;

    Population generation;
    Population next_generation;
    vector<Edge> edges;

    int iteration = 0;

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
        graph_ = &gr;
        
        GC_Naive_2 naive;
        ColoredGraph c_gr = naive.solve(gr); 
        Count color_count = c_gr.colorCount();
        int node_count = gr.nodeCount();
        Coloring coloring(node_count);
        for (auto i = 0; i < node_count; ++i) {
            coloring[i] = c_gr.color(i);
        }

        graph::ForEachEdge(gr, [&](auto i, auto j) {
            edges.emplace_back(i, j);
        });

        next_generation = generation = Population(node_count, population_size_);

        // has time
        while (true) {
            --color_count;
            generation.Reset();
            generation.MakeChildren(std::bind(&GC_GA::randomColoring, this, std::placeholders::_1, color_count));

            bool found = false;

            int best_fitness = 10000;
            for (iteration = 0; iteration  < max_iteration_count_; ++iteration ) {



                if (best_fitness == 0) {
                    cout << "iterations "  << iteration  << endl;
                    break;
                }
                if (best_fitness > 4) {
                    nextGeneration(std::bind(&GC_GA::selectParents_1, this),
                                   std::bind(&GC_GA::crossover_1, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                                   std::bind(&GC_GA::mutation_1, this, std::placeholders::_1, color_count));
                } else {
//                    generation.RemoveBottomHalf();
                    nextGeneration(std::bind(&GC_GA::selectParents_2, this),
                                   std::bind(&GC_GA::crossover_2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                                   std::bind(&GC_GA::mutation_2, this, std::placeholders::_1, color_count));
                }

                // TODO full replacement is good
                generation = next_generation;
                generation.Sort();
//                generation.Merge(next_generation);
//                swap(generation, next_generation);

                if (!found) {
                    bool next_gen = false;
                    for (auto& s : generation) {
                        if (s.iteration == iteration) {
                            next_gen = true;
                            break;
                        }
                    }
                    if (!next_gen) Println(cout, "next gen not included ", iteration);
//                    found = true;
                }

                best_fitness = min_element(next_generation.begin(), next_generation.end())->violations;
                next_generation.Reset();
            }

            if (best_fitness != 0) {
                break;
            }

            if (!isFeasibleColoring(ColoredGraph(gr, generation[0].coloring))) {
                Println(cout, generation[0].coloring);
                throw std::runtime_error("found solution is not feasible");
            }

            std::cout << "sol found " << color_count << endl;
            coloring = generation[0].coloring;
        }
        return ColoredGraph(gr, coloring);
    }


private:
    
    int countViolations(const Coloring& coloring) {
        int violation_count = 0;
        graph::ForEachEdge(*graph_, [&](auto i, auto j) {
            if (coloring[i] == coloring[j]) {
                ++violation_count;
            }
        });
        return violation_count;
    }

    void crossover_1(Coloring& child, const Sample& p_0, const Sample& p_1) {
        // TODO maybe always should include genes from both parents or not
        uniform_int_distribution<int> crosspoint_distr(0, graph_->nodeCount());
        int crosspoint = crosspoint_distr(rng_);

        copy(p_0.coloring.begin(), p_0.coloring.begin() + crosspoint, child.begin());
        copy(p_1.coloring.begin() + crosspoint, p_1.coloring.end(), child.begin() + crosspoint);
    }

    void crossover_2(Coloring& child, const Sample& p_0, const Sample& p_1) {
        child = min(p_0, p_1).coloring;
    }

    std::pair<int, int> selectParents_1() {
        uniform_int_distribution<int> indices(0, generation.size()-1);

        auto fittest = [&](auto i, auto j) {
            return generation[i] < generation[j];
        };

        auto s_1 = min(indices(rng_), indices(rng_), fittest);
        auto s_2 = min(indices(rng_), indices(rng_), fittest);

        return {s_1, s_2};
    };

    std::pair<int, int> selectParents_2() {
        // TODO not reducing capacity is a bad thing
        uniform_int_distribution<int> indices(0, (generation.capacity()/2)-1);
        return {indices(rng_), indices(rng_)};
    }

    // passing as arguments to avoid allocating memory
    template<class FuncSelectParents, class FuncCrossover, class FuncMutation>
    void nextGeneration(FuncSelectParents&& selectParents, FuncCrossover&& crossover, FuncMutation&& mutation) {

        unordered_set<int> used_parents;
        next_generation.MakeChildren([&](auto& sample) {
            int ss_1, ss_2;
            for (;;) {
                auto [s_1, s_2] = selectParents();
                if (s_1 != s_2 && used_parents.count(s_1*graph_->nodeCount() + s_2) == 0) {
                    ss_1 = s_1;
                    ss_2 = s_2;
                    if (one_child_per_couple) {
                        used_parents.insert(ss_1*graph_->nodeCount() + ss_2);
                        used_parents.insert(ss_2*graph_->nodeCount() + ss_1);
                    }
                    break;
                }
            }
            crossover(sample.coloring, generation[ss_1], generation[ss_2]);
//            if (real_distr_(rng_) < mutation_frequency) {
                mutation(sample.coloring);
//            }
            sample.violations = countViolations(sample.coloring);
            sample.iteration = iteration;
        });
    }

    void mutation_1(Coloring& child, ColorCount colorCount) {
        if (real_distr_(rng_) > mutation_frequency) return;
        bool progress = false;

        vector<Edge> unresolved;
        ForEachEdgeAtRandom([&](auto i, auto j) {
            if (child[i] != child[j]) return;
            if (real_distr_(rng_) < 0.5) swap(i, j);
            if (!ResetColor(i, *graph_, child, colorCount, rng_)) {
                if (!ResetColor(j, *graph_, child, colorCount, rng_)) unresolved.emplace_back(i, j);
                else progress = true;
            } else {
                progress = true;
            }
        });
        vector<Edge> unresolved_new;

        if (!continious_mutation) progress = false;

        while (progress) {
            progress = false;
            for (auto e : unresolved) {
                auto [i, j] = e;
                if (child[i] != child[j]) return;
                if (real_distr_(rng_) < 0.5) swap(i, j);
                if (!ResetColor(i, *graph_, child, colorCount, rng_)) {
                    if (!ResetColor(j, *graph_, child, colorCount, rng_)) unresolved_new.emplace_back(i, j);
                    else progress = true;
                } else {
                    progress = true;
                }
            }
            if (progress) Println(cout, "helping");
            swap(unresolved, unresolved_new);
            unresolved_new.clear();
        }

    }

    void mutation_2(Coloring& child, ColorCount colorCount) {
        uniform_int_distribution<int> color(0, colorCount-1);
        ForEachEdgeAtRandom([&](auto i, auto j) {
            if (child[i] != child[j]) return;
            if (real_distr_(rng_) < 0.5) swap(i, j);
            while ((child[i] = color(rng_)) == child[j]);
        });
    }

    Index randomNode() {
        return node_distr_(rng_);
    }

    void randomColoring(Sample& sample, int color_count) {
        uniform_int_distribution<int> color_distr(0, color_count-1);
        for (auto& c : sample.coloring) {
            c = color_distr(rng_);
        }
        sample.violations = countViolations(sample.coloring);
        sample.iteration = iteration;
    }

    template<class Func>
    void ForEachEdgeAtRandom(Func&& func) {
        shuffle(edges.begin(), edges.end(), rng_);
        for (auto e : edges) {
            func(e.first, e.second);
        }
    }

    void countDuplicates(const Population& population) {

        for (auto i = 0; i < population.size();) {

            int j = i + 1;
            for (; j < population.size() && population[i] == population[j]; ++j);
            if (j-i-1 > 0) Println(cout, "index: ", i, " duplicates: ", j-i-1);
            i = j;
        }
    }
};