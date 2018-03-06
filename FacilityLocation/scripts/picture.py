

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Facility:
    def __init__(self, cost, capacity, x, y):
        self.cost = cost
        self.capacity = capacity
        self.location = Point(x, y)
    
class Customer:
    def __init__(self, demand, x, y):
        self.demand = demand
        self.location = Point(x, y)
    

data = open("input.txt").read().split("\n")

facility_count, customer_count = map(int, data[0].split())
next_data_index = 1
facilities = []
for index in range(next_data_index, next_data_index + facility_count):
    facilities.append(Facility(*(map(float, data[index].split()))))
next_data_index += facility_count
customers = []
for index in range(next_data_index, next_data_index + customer_count):
    customers.append(Customer(*(map(float, data[index].split()))))
    
# first string is objective
result_data = open("output.txt").read().split("\n")

# possible wrong last space
customer_to_facility = map(int, result_data[1].split())

x = []
y = []
for i in range(customer_count):
    c_l = customers[i].location
    f_l = facilities[customer_to_facility[i]].location
    x.extend([c_l.x, f_l.x, None])
    y.extend([c_l.y, f_l.y, None])

plt.plot(x, y)
plt.show()





