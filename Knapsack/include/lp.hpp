#pragma once

#include <coin/ClpSimplex.hpp>
#include <coin/CoinModel.hpp>
#include <coin/OsiClpSolverInterface.hpp>
#include <coin/CbcModel.hpp>
#include <coin/CbcSimpleInteger.hpp>
#include <coin/CglKnapsackCover.hpp>
#include <coin/CbcCompareDepth.hpp>


using namespace std;

struct KS_Solver : OsiClpSolverInterface {

};


class KS_LP {

public:
    pair<int, vector<int>> solve(const vector<Item>& items, int capacity) {

        CoinModel coinModel;
        for (auto item : items) {
            const int row = 0;
            double w = item.w;
            coinModel.addCol(1, &row, &w, 0, 1, item.v, NULL, true);
        }
        coinModel.setRowUpper(0, capacity);

        cout << "cap:" << capacity << "\n";

        OsiClpSolverInterface solver;
        solver.loadFromCoinModel(coinModel);
        solver.setObjSense(-1);

        CbcModel model(solver);
        //model.setLogLevel(0);
        CglKnapsackCover generator;
        //model.addCutGenerator(&generator, 0, "knapsack");


        //model.initialSolve();

        setPriority(model, items);

        CbcCompareDepth depthCompare;
//        model.setNodeComparison(depthCompare);

        const double *initSolution = model.solver()->getColSolution();
        vector<double> initRes(initSolution, initSolution+items.size());

        ant::Println(cout, initRes, std::string("relax"));

        model.branchAndBound();
        const double *solution = model.bestSolution();

        vector<int> res(solution, solution+items.size());
        return {abs(model.getSolverObjValue()), res};
    }

private:
    void setPriority(CbcModel& model, const vector<Item>& items) {
        std::vector<CbcSimpleInteger*> objs;

        ant::Println(cout, effectiveness(items), std::string("effect"));
        auto k = 0;

        for (auto i : items) {
            cout << k++ <<","<< i.w << " ";
        }
        cout << "\n";

        int priority = items.size();
        for (auto c : effectiveness(items)) {

            auto obj = new CbcSimpleInteger(&model, c);
            obj->setPriority(100*priority);
            obj->setPreferredWay(1);
            obj->setColumnNumber(c);

            --priority;

            // go up first
            objs.push_back(obj);
        }
        model.addObjects(objs.size(), (CbcObject**)objs.data());

        for (auto obj : objs) {
            delete obj;
        }
    }

};