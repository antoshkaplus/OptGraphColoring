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

using namespace ant;
using namespace std;

// colors are numbers. if vertex is uncolored then it has -1?
using Node          = int;
using Edge          = pair<int, int>;
using AdjacencyList = vector<vector<int>>;
using Degree        = size_t;
using Color         = int;
using Graph         = graph::Graph16<false>;
using GraphBuilder  = graph::GraphBuilder<Graph::directed, Graph::NodeType>;

constexpr int COLOR_NONE = -1;


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