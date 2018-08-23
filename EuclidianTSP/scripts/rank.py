# this script reads all the scores
# and prints sorted entries with corresponding
# total scores

import glob
import os
import numpy


class Solution:
    def __init__(self, name):
        self.name = name
        self.local_scores = []

    def add_local_score(self, s):
        self.local_scores.append(s)

    def init_global(self, best_scores):
        self.global_score = int(10**6 * numpy.mean(numpy.divide(best_scores, self.local_scores)))


solutions = []
filenames = glob.glob("../scores/*.txt")
for f_name in filenames:
    with open(f_name) as f:
        base = os.path.basename(f_name)
        base = base.split(".")[0]
        sol = Solution(base)
        f.readline()
        for line in f:
            try:
                score = float(line.split(",")[1])
                sol.add_local_score(score)
            except ValueError:
                print(f_name, line)
                raise

        solutions.append(sol)

best_scores = list(map(min, zip(*[s.local_scores for s in solutions])))
for x in solutions: x.init_global(best_scores)
solutions.sort(key=lambda x: x.global_score, reverse=True)

for s in solutions:
    print(s.name, ":", s.global_score)