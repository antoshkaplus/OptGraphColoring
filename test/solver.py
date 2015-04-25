#!/usr/bin/python
# -*- coding: utf-8 -*-


import os
from math import sqrt
from matplotlib import pyplot as plt

def parsePoints(inputData):
  ps = map(float, inputData.split()[1:])
  ps = zip(ps[0::2], ps[1::2])
  return ps

def distance(p1, p2):
  return sqrt((p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]))

def graph_result(inputData, outputData):
  ps = parsePoints(inputData)
  arr = map(int, outputData.split()[2:])
  d = []
  b = arr[0]
  for a in arr[1:]:
    d.append(distance(ps[b], ps[a]))
    b = a
  plt.plot(range(len(d)), d)
  plt.show()  
    

def solve_it(inputData):
  f = open("input.txt", "w")
  f.write(inputData)
  f.close()
  
  os.system("./../DerivedData/TravelingSalesman/Build/Products/Release/TravelingSalesman")
  """
  os.system("python ./tsp_format.py " + "input.txt")
  os.system("./acotsp -i input.txt.tsp -z --acs -r 1")
  """
  f = open("output.txt")
  outputData = f.read()
  f.close()
  
  #graph_result(inputData, outputData)
  
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
        print 'This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/tsp_51_1)'

