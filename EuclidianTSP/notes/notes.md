
## Conclusions

- Array Tour during flip reversing backward in the direction of fewer elements
doesn't help due to next index computation overhead
- Two Level Tree Tour start to make huge difference for 1 million cities. 5 times
faster than Array Tour in case of 10 thousands cities.
- Caching distances helps only when the matrix stays in the cache. Otherwise
it's faster to compute it on the fly other than address slower memory


### TODO

Failed By Time. Need timer stop:
ant_colony

Need error check. Look at memory consumption.
shortest_edge
simplex_ins