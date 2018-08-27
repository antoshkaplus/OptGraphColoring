
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

        ifstream file;
        if (argc == 2) {
            file.open(argv[1]);
        }
        istream& cin = argc == 2 ? file : std::cin;

        auto problem = ReadProblem(cin);

#ifdef SIMPLEX_INS

        TSP_SimplexInsertion solver;

#endif
#ifdef SHORTEST_EDGE

        TSP_ShortestEdge solver;

#endif
#ifdef RANDOM_INS

        TSP_RandomInsertion solver;

#endif
#ifdef NEAREST_NEIGHBOR

        TSP_NearestNeighbor solver;

#endif
#ifdef NEAREST_INS

        TSP_NearestInsertion solver;

#endif
#ifdef FARTHEST_INS

        TSP_FarthestInsertion solver;

#endif
#ifdef ANT_COLONY

        TSP_AntColony solver;

#endif
#ifdef SA

        TSP_SA solver(TSP_SA_Iterations(problem.size()), std::chrono::hours(1));

#endif

        auto solution = solver.solve(problem);
        if (!isFeasibleSolution(problem, solution)) throw runtime_error("result is not feasible");

        cout << TSP_Distance(problem, solution) << endl;
    } catch (const std::exception& e) {
        std::cerr << e.what();
        throw;
    }
}
