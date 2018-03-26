#pragma once

#include <thread>
#include <cstdio>

#include <coin/ClpSimplex.hpp>
#include <coin/CoinModel.hpp>
#include <coin/OsiClpSolverInterface.hpp>
#include <coin/CbcModel.hpp>
#include <coin/CbcBranchAllDifferent.hpp>
#include <coin/CbcSOS.hpp>
#include <coin/CglKnapsackCover.hpp>
#include <coin/CbcCompareDepth.hpp>
#include <coin/CbcCutGenerator.hpp>
#include <coin/CbcHeuristic.hpp>

#include "GC.hpp"


class GC_LP_Heuristic : public CbcHeuristic {

    Count prevColorCount = -1;

public:
    GC_LP_Heuristic() = default;
    GC_LP_Heuristic(const GC_LP_Heuristic& heuristic) : CbcHeuristic(heuristic) {}

    CbcHeuristic* clone() const {
        return new GC_LP_Heuristic();
    }

    int solution(double &objectiveValue, double *newSolution) {
        const double * bestSolution = model_->bestSolution();
        if (!bestSolution)
            return 0;

        auto numCols = model_->solver()->getNumCols();
        auto colorCount = 1 + *max_element(bestSolution, bestSolution+numCols);

        if (colorCount == prevColorCount) return 0;
        prevColorCount = colorCount;

        struct Item {
            Color color;
            Node count = 0;

            bool operator<(const Item& item) const {
                return count > item.count;
            }
        };

        vector<Item> order(colorCount);

        for (auto i = 0; i < colorCount; ++i) order[i].color = i;

        for (auto i = 0; i < numCols; ++i) {
            ++order[bestSolution[i]].count;
        }

        sort(order.begin(), order.end());

        for (auto i = 0; i < order.size(); ++i) {
            if (order[i].count == -1 || order[i].color == i) continue;

            Color newColor = i;
            Color color = order[i].color;
            for (;;) {
                auto index = color;
                auto& item = order[index];
                // save for the next iteration
                color = item.color;
                item.color = newColor;
                item.count = -1;
                newColor = index;
                if (i == newColor) break;
            }
        }

        objectiveValue = 0;
        for (auto i = 0; i < numCols; ++i) {
            objectiveValue += (newSolution[i] = order[bestSolution[i]].color);
        }

        return 1;
    }

    void resetModel(CbcModel * /*model*/) {
        throw runtime_error("resetModel not supportes");
    }
};

class GC_LP_EventHandler : public CbcEventHandler {

    string recoveryPath;

public:

    GC_LP_EventHandler(string recoveryPath) : recoveryPath(recoveryPath) {}
    GC_LP_EventHandler(const GC_LP_EventHandler& handler)
        : CbcEventHandler(handler), recoveryPath(handler.recoveryPath) {}

    CbcAction event(CbcEvent whichEvent) {
        if (whichEvent == CbcEvent::solution) {
            ofstream out(recoveryPath, std::ios::app);
            auto solution = model_->bestSolution();
            if (solution == nullptr) {
                Println(out, "NULL");

                return CbcEventHandler::event(whichEvent);
            }
            auto numCols = model_->solver()->getNumCols();
            auto colorCount = 1 + *max_element(solution, solution+numCols);

            Println(out, colorCount);
            PrintlnSequence(out, solution, solution+numCols);
        }

        return CbcEventHandler::event(whichEvent);
    }

    CbcEventHandler* clone() const {
        return new GC_LP_EventHandler(*this);
    }
};



enum class GC_LP_Rules {
    PerNode,
    PerEdge
};

class GC_LP : public GC {

    GC_LP_Rules rules;
    string recoveryPath;
    double max_seconds {0};

public:
    GC_LP(GC_LP_Rules rules)
        : rules(rules)  {}

    // nothing to inherit form GC really
    ColoredGraph solve(const Graph& gr) override {

        CoinModel coinModel;
        for (auto i = 0; i < gr.nodeCount(); ++i) {
            coinModel.addCol(0, nullptr, nullptr, 0, gr.nodeCount()-1, 1, nullptr, true);
        }
        OsiClpSolverInterface solver;
        solver.loadFromCoinModel(coinModel);

        CbcModel model(solver);
        model.setLogLevel(0);

        GC_LP_Heuristic heuristic;
        model.addHeuristic(&heuristic);

        if (rules == GC_LP_Rules::PerEdge) {
            AddPerEdgeRules(model, gr);
        } else {
            AddPerNodeRules(model, gr);
        }

        model.setNumberThreads(std::thread::hardware_concurrency());

        if (!recoveryPath.empty()) {
            model.passInEventHandler(make_unique<GC_LP_EventHandler>(recoveryPath).get());
        }
        if (max_seconds != 0) model.setMaximumSeconds(max_seconds);

        model.initialSolve();
        model.branchAndBound();

        if (model.maximumSecondsReached()) Println(cout, "max seconds reached");
        if (model.isSecondsLimitReached()) Println(cout, "seconds limit reached");

        const double *solution = model.bestSolution();

        if (solution == nullptr) {
            vector<Color> colors(gr.nodeCount());
            iota(colors.begin(), colors.end(), 0);
            return {gr, colors};
        }
        return ColoredGraph(gr, {solution, solution+gr.nodeCount()});
    }

    string name() override {
        return "LinearProgramming";
    }

    void set_max_seconds(double seconds) {
        max_seconds = seconds;
    }

    void set_recovery_path(const string& str) {
        recoveryPath = str;
    }

private:

    void AddPerNodeRules(CbcModel& model, const Graph& gr) {
        vector<CbcBranchAllDifferent*> objects;

        for (auto i = 0; i < gr.nodeCount(); ++i) {
            auto& next = gr.nextNodes(i);
            vector<int> arr(next.begin(), next.end());
            arr.push_back(i);

            objects.push_back(new CbcBranchAllDifferent(&model, arr.size(), arr.data()));
        }

        model.addObjects(objects.size(), (CbcObject**)objects.data());

        for (auto obj : objects) {
            delete obj;
        }
        objects.clear();
    }

    void AddPerEdgeRules(CbcModel& model, const Graph& gr) {
        vector<CbcBranchAllDifferent*> objects;

        ForEachEdge(gr, [&](Node n_1, Node n_2) {
            int arr[2] = {n_1, n_2};
            objects.push_back(new CbcBranchAllDifferent(&model, 2, arr));
        });
        model.addObjects(objects.size(), (CbcObject**)objects.data());

        for (auto obj : objects) {
            delete obj;
        }
        objects.clear();
    }

    void SaveModel(CbcModel& model) {
        char tmpName[64];
        strcpy(tmpName, "/tmp/XXXXXX");

        auto fd = mkstemp(tmpName);
        FILE* fp = fdopen(fd, "w");
        model.generateCpp(fp, 0);

        fclose(fp);
        close(fd);

        if (rename(tmpName, recoveryPath.c_str())) {
            throw runtime_error("unable to save model");
        }
    }
};