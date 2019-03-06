
#include <tbb/parallel_do.h>
#include <tbb/parallel_for_each.h>

#include "tsp_util.hpp"
#include "tsp_lin_kernighan_base.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"

int main(int argc, const char * argv[]) {
    auto problem = ReadProblem(std::string("./data/tsp_575_1"));

    auto neighbours = NearestNeighbours(problem, 5);

    TwoLevelTreeTour tour(problem.size());
    LinKernighanBase solver(tour, problem, neighbours, 1e-3);

    solver.Solve();

    auto solution = tour.Order();

    Println(cout, TSP_Distance(problem, solution));
    Println(cout, solver.iter_count());
    Println(cout, solver.close_count());
}
