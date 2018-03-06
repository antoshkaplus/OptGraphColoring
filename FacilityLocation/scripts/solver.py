#!/usr/bin/python
# -*- coding: utf-8 -*-

import os

def solve_it(input_data):
    print input_data.split("\n", 1)[0]
    file_in = open("input.txt", "w")
    file_in.write(input_data)
    file_in.close()
    
    executable = ("/Users/antoshkaplus/Documents"
                  "/Coursera/DiscreteOptimization"
                  "/include/DerivedData"
                  "/include/Build/Products/Release/include")
    os.system(executable)
    
    file_out = open("output.txt")
    output_data = file_out.read()
    file_out.close()
    
    return output_data


import sys

if __name__ == '__main__':
    if len(sys.argv) > 1:
        file_location = sys.argv[1].strip()
        input_data_file = open(file_location, 'r')
        input_data = ''.join(input_data_file.readlines())
        input_data_file.close()
        print solve_it(input_data)
    else:
        print 'This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/fl_16_2)'

