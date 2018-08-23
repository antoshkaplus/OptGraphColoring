
import sys
import os
from io import StringIO
import subprocess as sp
from multiprocessing import cpu_count
from multiprocessing import Pool
from optparse import OptionParser

TEST = None
VERSION = None

CPU_COUNT = cpu_count()
if CPU_COUNT > 2: CPU_COUNT -= 2

parser = OptionParser(usage="usage: %prog --version=%executable [--test=%index] [--cpu=%cpu_count]")
parser.add_option("--test", dest="test")
parser.add_option("--version", dest="version")
parser.add_option("--cpu", dest="cpu_count")

(options, args) = parser.parse_args()

if options.test:
    TEST = int(options.test)

if options.version:
    VERSION = options.version
else:
    parser.error("version not specified")

if options.cpu_count:
    CPU_COUNT = int(options.cpu_count)

def worker(name):
    print("start problem:", name)

    out = StringIO()
    p = sp.Popen("../bin/" + VERSION, stdin=open("../data/" + name), stdout=sp.PIPE, stderr=sp.PIPE)
    line = p.stdout.read()
    if len(line) > 0 and line[-1] == '\n': line = line[:-1]
    return line

# skip new line chars
names = list(map(lambda x: x[:-1], open("problem_names.txt")))
if TEST != None: names = [names[TEST]]

pool = Pool(CPU_COUNT)
result = pool.map(worker, (n for n in names))
with open("../scores/" + VERSION + ".txt", "w") as out:
    out.write(str(len(result)) + "\n")
    for name, score in zip(names, result):
        out.write(str(name) + "," + str(float(score)) + "\n")