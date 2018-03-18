#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <numeric>
#include <cassert>
#include <iterator>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <fstream>
#include <string>

#include "ant/graph/graph.hpp"
#include "ant/core/count_index.hpp"

using namespace ant;
using namespace std;

// colors are numbers. if vertex is uncolored then it has -1?
using Graph         = graph::Graph16<false>;
using GraphBuilder  = graph::GraphBuilder<Graph::directed, Graph::NodeType>;
using Node          = Graph::NodeType;
using Edge          = pair<Node, Node>;
using AdjacencyList = vector<vector<Node>>;
using Degree        = Node;
using Color         = Node;
using NodeCount     = Node;
using ColorCount    = Node;


constexpr Color COLOR_NONE = -1;


Graph ReadProblem(std::istream& in) {
    Count nodeCount, edgeCount;
    in >> nodeCount >> edgeCount;
    GraphBuilder builder(nodeCount);
    for (auto i = 0; i < edgeCount; ++i) {
        Count e_1, e_2;
        in >> e_1 >> e_2;
        builder.Add(e_1, e_2);
    }
    return builder.Build();
}

class Coloring {

    struct Item {
        Color color = COLOR_NONE;
        Node uncoloredIndex = -1;
    };

    // node => color
    vector<Item> items;
    // nodes that are uncolored
    vector<Node> uncoloredNodes_;

public:

    Coloring() {}
    Coloring(Node nodeCount) : items(nodeCount), uncoloredNodes_(nodeCount) {
        iota(uncoloredNodes_.begin(), uncoloredNodes_.end(), 0);
        for (auto i : uncoloredNodes_) {
            items[i].uncoloredIndex = i;
        }
    }

    Coloring(const vector<Color>& coloring) : Coloring(coloring.size()) {
        for (auto i = 0; i < coloring.size(); ++i) {
            set(i, coloring[i]);
        }
    }

    Color color(Node n) const {
        return items[n].color;
    }

    Color operator[](Node n) const {
        return items[n].color;
    }

    void reset(Node n) {
        auto& t = items[n];
        if (t.color == COLOR_NONE) return;

        t.color = COLOR_NONE;
        t.uncoloredIndex = uncoloredNodes_.size();
        uncoloredNodes_.push_back(n);
    }

    void set(Node n, Color c) {
        auto& t = items[n];
        if (t.color == COLOR_NONE) {
            // remove from uncolored set
            SwapBackPop(uncoloredNodes_, t.uncoloredIndex);
            if (uncoloredNodes_.empty() == false) {
                items[uncoloredNodes_[t.uncoloredIndex]].uncoloredIndex = t.uncoloredIndex;
            }
        }
        t.color = c;
    }

    const vector<Node>& uncoloredNodes() const {
        return uncoloredNodes_;
    }
};