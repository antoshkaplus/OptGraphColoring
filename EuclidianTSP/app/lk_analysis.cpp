
#include <tbb/parallel_do.h>
#include <tbb/parallel_for_each.h>

#include "tsp_util.hpp"
#include "lk/lk_analysis.hpp"

int main(int argc, const char * argv[]) {
    LK_Analyze(argv[1]);
}
