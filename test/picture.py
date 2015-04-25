
from matplotlib import pyplot as plt

cities = map(int, open("output.txt").read().split()[2:])
points = map(lambda x: map(float, x.split()) ,open("input.txt").read().split("\n")[1:-1])

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