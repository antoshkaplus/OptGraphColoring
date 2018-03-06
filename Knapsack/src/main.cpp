
#include <fstream>
#include <string>

#include "Knapsack.h"
#include "KnapsackOpt.h"

using namespace std;

const bool debug = 0;

const string root = "/Users/antoshkaplus/Documents/Coursera/DiscreteOptimization/Knapsack/scripts/";
ifstream in(root + "input.txt");
ofstream out(root + "output.txt");

int main(int argc, const char * argv[])
{
  int n, K;
  in >> n >> K;
  vector<Item> items(n);
  for (Item &i : items) {
    in >> i.v >> i.w;
  }
  KnapsackOpt kp;
  pair<int, vector<int>> res = kp.relaxation(items, K);
  if (debug) {
    pair<int, vector<int>> res2 = usual(items, K);
    if (res.first != res2.first) {
      cout << "we have a problem: " << res.first << " != " << res2.first << endl;
    }
    else {
      cout << "all right" << endl;
      if (res.second == res2.second) {
        cout << "solutions are the same" << endl;
      }
    }
  }
  out << res.first << " " << 1 << endl;
  for (int i : res.second) {
    out << i << " ";
  }
  
  return 0;
}

