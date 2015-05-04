

from matplotlib import pyplot as plt
from solver import parsePoints
import sys
import os

def readPoints():
  f = open("./input.txt")
  ps = parsePoints(f.read())
  return ps

def drawEdges():
  ps = readPoints()
  f = open("./test.txt")
  order =  map(lambda s: map(int,s.split()), f.read().split("\n")[:-1])
  for i1, i2 in order:
    plt.plot((ps[i1][0], ps[i2][0]),(ps[i1][1], ps[i2][1]), "ro-")
  plt.show()
  
def drawPolygon():
  ps = readPoints()
  inds = map(int, open("./test.txt").read().split("\n")[:-1])
  i_prev = inds[-1]
  for i in inds:
    plt.text(ps[i][0], ps[i][1], str(i))
    #plt.text((ps[i_prev][0]+ps[i][0])/2, (ps[i_prev][1]+ps[i][1])/2, 
    #         "%.2f" % ((ps[i_prev][0]-ps[i][0])**2 + (ps[i_prev][1]-ps[i][1])**2)**0.5)
    plt.plot((ps[i_prev][0], ps[i][0]), (ps[i_prev][1], ps[i][1]), "ro-")
    i_prev = i
  plt.show()


def solveIt(inputData):
  f = open("input.txt", "w")
  f.write(inputData)
  f.close()
  
  os.system("./../DerivedData/TravelingSalesman/Build/Products/Release/TravelingSalesman")

  #drawPolygon()
  #drawEdges()

if __name__ == '__main__':
  if len(sys.argv) > 1:
    fileLocation = sys.argv[1].strip()
    inputDataFile = open(fileLocation, 'r')
    inputData = ''.join(inputDataFile.readlines())
    inputDataFile.close()
    print solveIt(inputData)
  else:
    print 'This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/tsp_51_1)'
