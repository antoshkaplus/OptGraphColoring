

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection


data = open("input.txt").read().split("\n")

points = map(lambda x: map(float, x.split()[1:]), data[1:-1])

# first string is objective
result_data = open("output.txt").read().split("\n")
routes =  map(lambda x: map(int, x.split()), result_data[1:-1])

xs = []
ys = []
for r in routes:
    for i in r:
        xs.append(points[i][0])
        ys.append(points[i][1])
    
print xs
print ys
    
    
plt.plot(xs, ys)
plt.show()





