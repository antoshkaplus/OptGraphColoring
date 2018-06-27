
#include "tsp_util.hpp"
#include "tsp_simplex_insertion.hpp"
#include "tsp_shortest_edge.hpp"
#include "tsp_random_insertion.hpp"
#include "tsp_nearest_neighbor.hpp"
#include "tsp_nearest_insertion.hpp"
#include "tsp_farthest_insertion.hpp"
#include "tsp_ant_colony.hpp"
#include "tsp_sa.hpp"


int main(int argc, const char * argv[]) {

    try {

        auto problem = ReadProblem(cin);

#ifdef SIMPLEX_INS

        TSP_SimplexInsertion solver;
        solver.solve(problem);

#endif
#ifdef SHORTEST_EDGE

        TSP_ShortestEdge solver;
        solver.solve(problem);

#endif
#ifdef RANDOM_INS

        TSP_RandomInsertion solver;
        solver.solve(problem);

#endif
#ifdef NEAREST_NEIGHBOR

        TSP_NearestNeighbor solver;
        solver.solve(problem);

#endif
#ifdef NEAREST_INS

        TSP_NearestInsertion solver;
        solver.solve(problem);

#endif
#ifdef FARTHEST_INS

        TSP_FarthestInsertion solver;
        solver.solve(problem);

#endif
#ifdef ANT_COLONY

        TSP_AntColony solver;
        solver.solve(problem);

#endif
#ifdef SA

        TSP_SA solver(10000);
        solver.solve(problem);

#endif

        auto solution = solver.solve(problem);
        if (!isFeasibleSolution(problem, solution)) throw runtime_error("result is not feasible");

        cout << TSP_Distance(problem, solution) << endl;
    } catch (const std::exception& e) {
        std::cerr << e.what();
        throw;
    }
}
