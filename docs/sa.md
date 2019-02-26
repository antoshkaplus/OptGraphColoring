
f = e^(cost/temp)

if temp -> inf => cost/temp -> 0   => f -> 1
if temp -> 0   => cost/temp -> inf => negative cost is important to move it all to 0

inf and 0 are not needed, just close enough values: big and small

Goals:
* find optimum temp function
* find optimum iterations / trials

It can be seen that increasing number of iterations can be done at any moment
and it doesn't change anything done previously, but just continues convergence
to one solution.

It's hard to tell what trial count / iteration strategy / cooling schedule to use.

In case of taking trial count that can try all possible changes in solution neighbourhood
(very big number) user should pay attention more to exponential cooling schedule and 100 iterations
can be more than enough.

Oftentimes trial count is equal to problem size and in that case linear or polynomial
schedule is used unless user rough solution is enough.

