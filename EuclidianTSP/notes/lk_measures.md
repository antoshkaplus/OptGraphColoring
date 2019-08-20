\#1 : 51_1 / 70_1 / 100_4  
\#2 : 575_1 / 783_1 / 1173_1  
\#3 : 1889_1 / 3038_1 / 5934_1
\#4 : 14051_1 / 33810_1 / 85900_1  

\#2

5 nighbours, 60 deep
575_1: 6911.73, P_V1: 1s60ms156us156ns, P: 269ms404us70ns
783_1: 9022.68, P_V1: 5s216ms150us935ns, P: 1s388ms48us42ns
1173_1: 58395.7, P_V1: 16s419ms722us467ns, P: 4s560ms991us547ns

using 7 neighbours, 70 deep
575_1: 6844.63, 6s685ms74us92ns
783_1: 8991.88, 5m2s217ms321us350ns
1173_1: 58451.1, 3h41m48s424ms526us788ns

while result can become a little better with increase of neighbours and depth
it doesn't improve the result very much

\#3
5 neighbours, 20 deep
1889_1: 568630, 80ms270us549ns
3038_1: 143305, 252ms789us748ns
5934_1: 896804, 574ms263us218ns

\#4
5 neighbours, 20 deep
14051_1: 487460, it: 1938672, 4s218ms243us376ns
33810_1: 7.30552e+07, it: 24553347, 5m6s950ms609us377ns
85900_1: 1.55155e+08, it: 74564426, 40m1s550ms682us850ns

the number of iterations needed grows linearly with problem size