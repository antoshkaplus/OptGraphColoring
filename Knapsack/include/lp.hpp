#pragma once

#include <coin/ClpSimplex.hpp>
#include <coin/CoinModel.hpp>
#include <coin/OsiClpSolverInterface.hpp>
#include <coin/CbcModel.hpp>

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

        OsiClpSolverInterface solver;
        solver.loadFromCoinModel(coinModel);
        solver.setObjSense(-1);

        CbcModel model(solver);
//        model.setLogLevel(0);
        model.branchAndBound();
        const double *solution = model.bestSolution();

        vector<int> res(solution, solution+items.size());
        return {abs(model.getSolverObjValue()), res};
    }
};