#!/usr/bin/python
# -*- coding: utf-8 -*-

import os

def solve_it(inputData):
    f = open("input.txt", "w")
    f.write(inputData)
    f.close()

    os.system("/Users/antoshkaplus/Documents/Coursera/DiscreteOptimization/src/DerivedData/src/Build/Products/Release/src")
        
    f = open("output.txt")
    outputData = f.read()
    f.close()
    return outputData


import sys

if __name__ == '__main__':
    if len(sys.argv) > 1:
        fileLocation = sys.argv[1].strip()
        inputDataFile = open(fileLocation, 'r')
        inputData = ''.join(inputDataFile.readlines())
        inputDataFile.close()
        print solve_it(inputData)
    else:
        print 'This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/ks_4_0)'

