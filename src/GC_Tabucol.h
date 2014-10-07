//
//  GC_Tabucol.h
//  GraphColoring
//
//  Created by Anton Logunov on 10/6/14.
//
// 


// 
// algorithm by A.Hertz and D. de Werra
// reference: Using tabu search techniques for graph coloring
//  


#ifndef GraphColoring_GC_Tabucol_h
#define GraphColoring_GC_Tabucol_h

#include "ant/optimization.h"

#include "GC.h"

struct GC_Tabucol : GC {
    
    ColoredGraph solve(const Graph& gr) {
        graph_ = &gr;
        Index current_iteration = 0;
        Index nonimproving_iteration = 0;
        
        
        GC_Naive_2 naive;
        ColoredGraph c_gr = naive.solve(gr);
        ColoredGraph result;
        
        conflicting_nodes_.clear();
        
       // Count min_conflict_count;
        
        while (nonimproving_iteration < 150000) {
            if (conflicting_nodes_.empty()) {
                nonimproving_iteration = 0;
                result = c_gr;
                Color minColor = -1;
                Count minCount = -1;
                Color color;
                Count count;
                for (auto& p : c_gr.nodeCountOfColor()) {
                    tie(color, count) = p;
                    if (minCount == -1 || count < minCount) {
                        minCount = count;
                        minColor = color;
                    }
                }
                for (auto n = 0; n < c_gr.nodeCount; ++n) {
                    if (c_gr.color(n) == minColor) {
                        if (changeColorToNonAdjacent(n, &c_gr)) continue;
                        changeColorToAdjacent(n, &c_gr);
                    }
                }
                for (auto n = 0; n < c_gr.nodeCount; ++n) {
                    if (c_gr.adjacentNodesWithSameColorCount(n) > 0) {
                        conflicting_nodes_.insert(n);
                    }
                }
                //min_conflict_count = conflicting_nodes_.size();
            } 
            
            vector<Node> remove;
            while (true) {
                for (Node n : conflicting_nodes_) {
                    if (c_gr.adjacentNodesOfColorCount(n, c_gr.color(n)) == 0) {
                        remove.push_back(n);
                        continue;
                    }
                    Color old_color = c_gr.color(n);
                    // check if can change and conflict count will go to minimum
                    // so then I should check everyone
                    if (changeColorToNonAdjacent(n, &c_gr)) {
                        tabu_list_.insert(key(n, old_color), tenure(), current_iteration);
                        remove.push_back(n);
                        continue;
                    }
                }
                if (remove.empty()) break;
                for (Node n : remove) {
                    conflicting_nodes_.erase(n);
                }
                remove.clear();
            }
            
            if (conflicting_nodes_.empty()) continue;
            
            
            uniform_int_distribution<> node_distr(0, conflicting_nodes_.size()-1); 
            set<Node>::const_iterator it = conflicting_nodes_.begin();
            advance(it, node_distr(rng_));
            Node n = *it;
            
            current_iteration_ = current_iteration;
            recolorNode(n, &c_gr);
            
            ++nonimproving_iteration;
            ++current_iteration;
            
        }
        return result;
    }
    
    
private:
    
    void recolorNode(Node n, ColoredGraph* c_gr) {
        // take color with minimum conflits and that's not tabu
        Color minColor = -1;
        Count minCount;
        Color color;
        Count count;
        for (auto& p : c_gr->adjacentNodesOfColorCount(n)) {
            tie(color, count) = p;
            if (color == c_gr->color(n) || tabu_list_.hasInside(key(n, color), current_iteration_)) continue;
            if (minColor == -1 || count < minCount) {
                minColor = color;
                minCount = count;
            }
        }
        assert(minColor != -1); 
        tabu_list_.insert(key(n, c_gr->color(n)), tenure(), current_iteration_);
        c_gr->setColor_2(n, minColor);
        for (auto nn : c_gr->adjacencyList[n]) {
            if (c_gr->color(nn) == minColor) conflicting_nodes_.insert(nn);
        }

    }
    
    
    void recolorNode_3(Node n, ColoredGraph* c_gr) {
        uniform_int_distribution<> distr(0, c_gr->colorCount()-1);
        while (true) {
            Index i = distr(rng_);
            for (auto& p : c_gr->nodeCountOfColor()) {
                if (i-- != 0) continue;
                if (p.first == c_gr->color(n) || tabu_list_.hasInside(key(n, p.first), current_iteration_)) {
                    break;
                }
                tabu_list_.insert(key(n, c_gr->color(n)), tenure(), current_iteration_);
                Color c = p.first;
                c_gr->setColor_2(n, c);
                for (auto nn : c_gr->adjacencyList[n]) {
                    if (c_gr->color(nn) == c) conflicting_nodes_.insert(nn);
                }
                return;
            }
        }
    }
    
    
    void recolorNode_2(Node n, ColoredGraph* c_gr) {
        CountMap<Color> colors;
        for (auto nn : c_gr->adjacencyList[n]) {
            if (c_gr->color(n) == c_gr->color(nn) || tabu_list_.hasInside(key(n, c_gr->color(nn)), current_iteration_)) {
                continue;
            }
            if (!canChangeColorToNonAdjacent(nn, c_gr)) {
                colors.increase(c_gr->color(nn));
            } else {
                colors.increase(c_gr->color(nn), 0);
            }
        }
        assert(!colors.empty());
        Color c = colors.minCount();
        tabu_list_.insert(key(n, c_gr->color(n)), tenure(), current_iteration_);
        c_gr->setColor_2(n, c);
        for (auto nn : c_gr->adjacencyList[n]) {
            if (c_gr->color(nn) == c) conflicting_nodes_.insert(nn);
        }
    }
    
    
    struct ColorCount {
        Color color;
        Count count;
    };
    
    bool canChangeColorToNonAdjacent(Node n, ColoredGraph* c_gr) {
        Count c = c_gr->adjacentNodesWithSameColorCount(n) > 0 ? 0 : 1;
        if (c_gr->colorCount() == c + c_gr->adjacentColorCount(n)) return false;
        return true;
    }
    
    bool changeColorToNonAdjacent(Node n, ColoredGraph* c_gr) {
        Count c = c_gr->adjacentNodesWithSameColorCount(n) > 0 ? 0 : 1;
        if (c_gr->colorCount() == c + c_gr->adjacentColorCount(n)) return false;
        for (Color color : c_gr->colors()) {
            if (c_gr->adjacentNodesOfColorCount(n, color) == 0 && c_gr->color(n) != color) {
                c_gr->setColor_2(n, color);
                return true;
            }
        }
        throw logic_error("can't happen");
    }

    bool changeColorToNonAdjacent_2(Node n, ColoredGraph* c_gr) {
        Count c = c_gr->adjacentNodesWithSameColorCount(n) > 0 ? 0 : 1;
        if (c_gr->colorCount() == c + c_gr->adjacentColorCount(n)) return false;
        auto adj_set = c_gr->adjacentColors(n);
        auto set = c_gr->colors();
        vector<Color> extra(set.size() - adj_set.size());
        set_difference(set.begin(), set.end(), adj_set.begin(), adj_set.end(), extra.begin());
        uniform_int_distribution<> distr(0, extra.size()-1);
        Index i = distr(rng_);
        if (extra[i] == c_gr->color(n)) {
            if (i == 0) ++i;
            else --i;
        }
        c_gr->setColor_2(n, extra.at(i));
//        throw logic_error("can't happen");
        return true;
    }


    void changeColorToAdjacent(Node n, ColoredGraph* c_gr) {
        ColorCount min{-1, 1};
        for (auto p : c_gr->adjacentNodesOfColorCount(n)) {
            if (p.first == c_gr->color(n)) {
                continue;
            }
            if (min.color == -1 || min.count > p.second) {
                min = {p.first, p.second};
            }
        }
        assert(min.color != -1);
        c_gr->setColor_2(n, min.color);
    }
    
    
    size_t key(Node node, Color color) const {
        return graph_->nodeCount * color + node;
    }
    
    
    Count tenure() {
        return 10;//distr_(rng_) + 0.6 * conflict_nodes_count_;
    }
    
    default_random_engine rng_;
    // random number for tenure 
    uniform_int_distribution<int> distr_{0, 9};

    Index current_iteration_;

    int conflict_nodes_count_;
    
    set<Node> conflicting_nodes_;
    
    
    const Graph* graph_;
    ant::opt::TabuList<size_t> tabu_list_;
};



#endif
