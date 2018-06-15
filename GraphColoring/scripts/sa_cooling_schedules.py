
import math
import matplotlib.pyplot as plt
import pandas as pd

def ExpMulCooling(T_0, alpha):
    return lambda k: T_0* alpha**k

def LogMulCooling(T_0, alpha):
    return lambda k: T_0 / (1. + alpha*math.log(1+k))

def LinearMulCooling(T_0, alpha):
    return lambda k: T_0 / (1. + alpha * k)

def QuadMulCooling(T_0, alpha):
    return lambda k: T_0 / (1. + alpha * k**2)

max_iteration = 1000000
step = max_iteration / 1000

indices = range(0, max_iteration, step)

series = {}
for name, func in (("exp", ExpMulCooling(0.99, 0.9995)),
                   ("log", LogMulCooling(0.9, 100000)), # have no real usage
                   ("lin", LinearMulCooling(0.95, 1.5)),
                   ("quad", QuadMulCooling(0.98, 1000))): # nothing good with this one too
    series[name] = pd.Series(map(func, indices), index=indices)

print pd.DataFrame(series)


