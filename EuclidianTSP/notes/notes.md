
## Conclusions

- Array Tour during flip reversing backward in the direction of fewer elements
doesn't help due to next index computation overhead
- Two Level Tree Tour start to make huge difference for 1 million cities. ~1.7 times
faster than Array Tour in case of 10 thousands cities.
- Caching distances helps only when the matrix stays in the cache. Otherwise
it's faster to compute it on the fly other than address slower memory


### TODO

LK algorithm should be reconsidered with implemenations of more ideas from Lin Kernighan
initial paper + research alpha-nearness + 1-trees in getting lower bound


Failed By Time. Need timer stop:
ant_colony

Need error check. Look at memory consumption.
shortest_edge
simplex_ins

Consider methods described in https://www2.seas.gwu.edu/~simhaweb/champalg/tsp/tsp.html
