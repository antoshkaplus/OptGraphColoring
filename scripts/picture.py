
from matplotlib import pyplot as plt
import sys

points_filename = sys.argv[1]
path_filename = sys.argv[2]

points_file = open(points_filename)
N = int(points_file.readline())
# nothing after last line break
lines = points_file.read().split("\n")
points = map(lambda x: map(float, x.split()), lines[:-1])
path_file = open(path_filename)
# first number is perimeter
P = float(path_file.readline())
# last space is extra
cities = map(int, path_file.readline().split()[:-1])

print len(cities)
print len(points)

ordered_points = []
for c in cities:
    ordered_points.append(points[c])
    plt.text(points[c][0], points[c][1], c)
ordered_points.append(points[cities[0]])

x, y = zip(*ordered_points)
plt.plot(x, y, "ro-")
plt.show()