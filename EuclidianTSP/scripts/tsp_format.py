

import os, sys


if __name__ == '__main__':
    if len(sys.argv) > 1:
        fileLocation = sys.argv[1].strip()
        inputDataFile = open(fileLocation, 'r')
        inputData = ''.join(inputDataFile.readlines())
        
        points = open(fileLocation).read().split("\n")[1:-1]
        
        out_file = open(fileLocation + ".tsp", "w")
        """
        out_file.write("NAME: problem\n"
                       "TYPE : TSP\n"
                       "DIMENSION : " + str(len(points)) + "\n" +
                       "EDGE_WEIGHT_TYPE : EUC_2D\n"
                       "NODE_COORD_SECTION\n")
        for i in range(len(points)):
            out_file.write(str(i+1) + " " + points[i] + "\n")
        out_file.write("EOF")
        """
        out_file.write(str(len(points)) + "\n")
        for i in range(len(points)):
            out_file.write(points[i] + " " + str(i) + "\n")
        