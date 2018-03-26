#pragma once

#include <ant/grid/grid.hpp>

#include "KempeChain.hpp"


using namespace ant::grid;

class GC_KempeChain : GC {

    struct Item {
        Node i;
        Color c;
        double objDiff = 0;

        bool operator<(const Item& item) const {
            return objDiff < item.objDiff;
        }
    };


public:
    ColoredGraph solve(const Graph& gr) {
        GC_Naive_2 naive;
        auto c_gr = naive.solve(gr);

        for (;;) {
            Grid<char> visited(c_gr.nodeCount(), c_gr.colorCount(), false);

            KempeChain kempeChain(c_gr);

            Item best;

            bool improved = false;
            for (Node i = 0; i < c_gr.nodeCount(); ++i) {
                Item best;
                for (auto c : c_gr.nodeCountOfColor().keys()) {
                    if (c_gr.adjacentNodesOfColorCount(i, c) == 0 && c_gr.color(i) != c && objectiveChange(c_gr, i, c) > 0) {
                        Item item{i, c, objectiveChange(c_gr, i, c)};
                        if (best < item) best = item;
                    }
                }

                if (best.objDiff != 0) {
                    c_gr.resetColor(i, best.c);
                    improved = true;
                }
            }

            if (improved) continue;

            // may decide to use first improvement strategy and pick the best kempeChain
            for (Node i = 0; i < c_gr.nodeCount(); ++i) {
                for (auto c : c_gr.adjacentColors(i)) {
                    if (!visited(i, c)) {
                        kempeChain.resetSwapSets();
                        kempeChain.Make(i, c);

                        assert(kempeChain.nodeSwapSet().size() > 0);
                        assert(kempeChain.colorSwapSet().size() > 0);

                        for (auto k : kempeChain.nodeSwapSet()) visited(k, c) = true;
                        for (auto k : kempeChain.colorSwapSet()) visited(k, c_gr.color(i)) = true;

                        Item item{i, c, objectiveChange(c_gr, i, c, kempeChain)};

                        if (best < item) best = item;
                    }
                }
            }

            if (best.objDiff == 0) break;

            kempeChain.resetSwapSets();
            kempeChain.Make(best.i, best.c);

            ResetColor(c_gr, kempeChain.colorSwapSet(), c_gr.color(best.i));
            ResetColor(c_gr, kempeChain.nodeSwapSet(), best.c);
        }
        return c_gr;
    }

    string name() {
        return "KempeChain_ReduceColorNodes";
    }

private:

    double objectiveChange(const ColoredGraph& c_gr, Node i, Color c) {
        auto c_1 = c_gr.color(i);
        auto c_2 = c;

        auto n_1 = c_gr.nodeCountOfColor(c_1);
        auto n_2 = c_gr.nodeCountOfColor(c_2);

        return -2.*n_1 + 2.*n_2 + 2;
    }

    double objectiveChange(const ColoredGraph& c_gr, Node i, Color c, const KempeChain& kempeChain) {
        auto c_1 = c_gr.color(i);
        auto c_2 = c;

        auto n_1 = c_gr.nodeCountOfColor(c_1);
        auto n_2 = c_gr.nodeCountOfColor(c_2);

        auto d_1 = kempeChain.nodeSwapSet().size();
        auto d_2 = kempeChain.colorSwapSet().size();

        auto a_1 = n_1-d_1+d_2;
        auto a_2 = n_2-d_2+d_1;

        return -1.*n_1*n_1 - 1.*n_2*n_2 + 1.*a_1*a_1 + 1.*a_2*a_2;
    }

    double objective(const ColoredGraph& c_gr) const {
        double obj = 0;
        auto& nodeCountOfColor = c_gr.nodeCountOfColor();
        for (auto color : nodeCountOfColor.keys()) {
            obj += nodeCountOfColor[color]*nodeCountOfColor[color];
        }

        return obj;
    }
};
