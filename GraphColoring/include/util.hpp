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
#include <experimental/memory_resource>

#include <ant/graph/graph.hpp>
#include <ant/graph/graph_util.hpp>
#include <ant/core/count_index.hpp>


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


struct Problem {
    NodeCount nodeCount;
    int completeness;
};

//vector<Problem> ProblemSet() {
//    vector<Problem> ps;
//
//}

vector<NodeCount> nodeCounts = {50, 70, 100, 250, 500};
vector<int> completeness = {1, 3, 5, 7, 9};

vector<pair<NodeCount, int>> problems;


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

Graph ReadProblem(NodeCount nodeCount, int completeness) {
    ifstream in(Format("data/gc_%d_%d", nodeCount, completeness));
    return ReadProblem(in);
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

bool ResetColor(Node node, const Graph& graph, vector<Color>& coloring, ColorCount colorCount, default_random_engine& rng) {
    std::vector<bool> colors;
    colors.resize(colorCount);
    std::fill(colors.begin(),
              colors.end(),
              false);

    for (auto k : graph.nextNodes(node)) {
        colors[coloring[k]] = true;
    }

    vector<Color> valid;
    for (int c = 0; c < colors.size(); ++c) {
        if (!colors[c]) {
            valid.push_back(c);
        }
    }
    if (valid.empty()) return false;

    uniform_int_distribution<int> distr{0, static_cast<int>(valid.size())-1};
    coloring[node] = valid[distr(rng)];
    return true;
}

Count CountViolations(const Graph& gr, const Coloring& colors) {
    int violation_count = 0;
    graph::ForEachEdge(gr, [&](auto i, auto j) {
        if (colors[i] == colors[j]) {
            ++violation_count;
        }
    });
    return violation_count;
}