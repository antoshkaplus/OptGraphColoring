#!/usr/bin/python
# -*- coding: utf-8 -*-

import os


def solve_it(input_data):
    f = open("input.txt", "w")
    f.write(input_data)
    f.close()
    
    print input_data.split("\n", 1)[0]
    
    os.system("/Users/antoshkaplus/Documents/Coursera/DiscreteOptimization/"
              "VehicleRouting/DerivedData/VehicleRouting/Build/Products/Release/VehicleRouting")
    
    f = open("output.txt")
    outputData = f.read()
    f.close()
    return outputData


import sys

if __name__ == '__main__':
    if len(sys.argv) > 1:
        file_location = sys.argv[1].strip()
        input_data_file = open(file_location, 'r')
        input_data = ''.join(input_data_file.readlines())
        input_data_file.close()
        print 'Solving:', file_location
        print solve_it(input_data)
    else:

        print 'This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/vrp_5_4_1)'

