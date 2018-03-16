
#include "support.h"
#include "lp.hpp"

using namespace ant;

int main(int argc, const char *argv[]) {
    auto path = "data/ks_19_0";
    std::ifstream in(path);
    auto pr = ReadProblem(in);

    KS_LP solver;
    auto sol = solver.solve(pr.items, pr.capacity);

//    CoinModel coinModel;
//    for (auto item : pr.items) {
//        const int row = 0;
//        double w = item.w;
//        coinModel.addCol(1, &row, &w, 0, 1, item.v, NULL, true);
//    }
//    coinModel.setRowUpper(0, pr.capacity);
//
//    OsiClpSolverInterface solver;
//    solver.loadFromCoinModel(coinModel);
//    solver.setObjSense(-1);
//
//    solver.initialSolve();
//
//    ant::Println(cout, solver.getObjValue());
//    ant::Println(cout, relaxation(pr.items, pr.capacity));
}