#pragma once

#include "KempeChain.hpp"


struct GC_HillClimbing : GC {

    ColoredGraph solve(const Graph& gr) {
        GC_Naive_2 naive;
        auto result = naive.solve(gr);

        for (;;) {
            auto c_gr = result;

            auto colorCount = result.colorCount()-1;
            for (auto i = 0; i < c_gr.nodeCount(); ++i) {
                if (c_gr.color(i) == colorCount) c_gr.unsetColor(i);
            }

            for (;;) {
                auto uncolored = c_gr.uncoloredNodes();
                for (auto i : uncolored) {

                    bool success = c_gr.setColor(i);
                    if (success) continue;

                    SetColorUsingKempeChain(c_gr, i);
                }

                if (uncolored.size() == c_gr.uncoloredNodeCount()) break;
            }

            if (c_gr.uncoloredNodeCount() > 0) break;
            else {
                cout << "new sol: " << c_gr.colorCount() << endl;
                assert(isFeasibleColoring(c_gr));
                result = c_gr;
            }
        }
        return result;
    }

    string name() {
        return "HillClimbing";
    }
};
