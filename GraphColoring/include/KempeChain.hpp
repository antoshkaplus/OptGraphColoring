#pragma once

#include "ColoredGraph.hpp"


class KempeChain {

    ColoredGraph& c_gr;
    vector<Node> nodeSwapSet_;
    vector<Node> colorSwapSet_;

    vector<bool> visited;
    vector<Node> toHandle;

public:
    KempeChain(ColoredGraph& c_gr) : c_gr(c_gr), visited(c_gr.nodeCount()) {}

    void resetSwapSets() {
        nodeSwapSet_.clear();
        colorSwapSet_.clear();
    }

    bool Make(Node i_1, Color c_2) {
        static vector<bool> prohibited;
        prohibited.resize(c_gr.nodeCount());
        fill(prohibited.begin(), prohibited.end(), false);

        return Make(i_1, c_2, prohibited);
    }

    bool Make(Node i_1, Color c_2, const vector<bool>& prohibited) {
        assert(i_1 != COLOR_NONE);

        if (prohibited[i_1]) return false;

        auto c_1 = c_gr.color(i_1);

        fill(visited.begin(), visited.end(), false);
        toHandle.clear();

        nodeSwapSet_.push_back(i_1);
        toHandle.push_back(i_1);
        visited[i_1] = true;
        while (!toHandle.empty()) {
            Node i = toHandle.back();
            toHandle.pop_back();
            for (Node j : c_gr.nextNodes(i)) {
                if (prohibited[j]) return false;
                if (visited[j]) continue;

                visited[j] = true;
                if (c_gr.color(j) == c_1) {
                    nodeSwapSet_.push_back(j);
                    toHandle.push_back(j);
                } else if (c_gr.color(j) == c_2) {
                    colorSwapSet_.push_back(j);
                    toHandle.push_back(j);
                }
            }
        }
        return true;
    }

    const vector<Node>& nodeSwapSet() const {
        return nodeSwapSet_;
    }

    const vector<Node>& colorSwapSet() const {
        return colorSwapSet_;
    }
};

bool SetColorUsingKempeChain(ColoredGraph& c_gr, Node i) {
    unordered_map<Color, vector<Node>> colorNodes;
    for (auto j : c_gr.nextNodes(i)) {
        if (c_gr.isColored(j)) colorNodes[c_gr.color(j)].push_back(j);
    }

    KempeChain kempeChain(c_gr);
    vector<bool> prohibited(c_gr.nodeCount());

    for (auto it_1 = colorNodes.begin(); it_1 != colorNodes.end(); ++it_1) {
        auto c_1 = it_1->first;
        fill(prohibited.begin(), prohibited.end(), false);
        for (auto j : it_1->second) {
            prohibited[j] = true;
        }

        for (auto it_2 = next(it_1); it_2 != colorNodes.end(); ++it_2) {

            bool success = false;

            kempeChain.resetSwapSets();

            auto c_2 = it_2->first;
            for (auto j : it_2->second) {
                success = kempeChain.Make(j, c_1, prohibited);
                if (!success) break;
            }

            if (success) {
                ResetColor(c_gr, kempeChain.nodeSwapSet(), c_1);
                ResetColor(c_gr, kempeChain.colorSwapSet(), c_2);

                for (auto i = 0; i < c_gr.nodeCount(); ++i)
                    if (c_gr.isColored(i))
                        assert(c_gr.adjacentNodesOfColorCount(i, c_gr.color(i)) == 0);

                c_gr.setColor(i, c_2);
                return true;
            }
        }
    }
    return false;
}

