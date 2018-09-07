/*
 * Simulated annealing and the Symetric
 * Euclidian Traveling Salesman Problem.
 *
 * Solution based on local search heuristics for
 * non-crossing paths and nearest neighbors 
 *
 * Storage Requirements: n^2+4n ints
 *
 * Problem: given the coordinates of n cities in the plane, find a
 * permutation pi_1, pi_2, ..., pi_n of 1, 2, ..., n that minimizes
 * sum for 1<=i<n D(pi_i,pi_i+1), where D(i,j) is the euclidian
 * distance between cities i and j
 *
 * Note: with n cities, there is (n-1)!/2 possible tours.
 * factorial(10)=3628800  factorial(50)=3E+64  factorial(150)=5.7E+262
 * If we could check one tour per clock cycle on a 100 MHZ computer, we
 * would still need to wait approximately 10^236 times the age of the
 * universe to explore all tours for 150 cities. 
 *
 * gcc -O4 -o tsp tsp.c -lm ; tsp | ghostview -
 *
 * Usage: tsp [-v] [n=dd] [s=dd] [filename]
 *     -v       : verbose
 *     n=       : nb of cities (cities generated randomly on E^2
 *     s=       : seed nb of random generator
 *     filename : tsp input file. If none, stdin assumed. 
 *
 * Input file format:
 * n
 * x1 y1 name1
 * y2 y2 name2
 * ...
 * 
 * Last Edited: Wed Jul 26 16:13:25 1995 by Maugis Lionel (Sofreavia) on sid1
 */

#include <stdio.h>               /* printf(), fopen() prototype */
#include <math.h>                /* sqrt()                      */
#include <string.h>              /* strdup()                    */

#define T_INIT                        100
#define FINAL_T                       0.1
#define COOLING                       0.9 /* to lower down T (< 1) */
#define TRIES_PER_T                   500*n  
#define IMPROVED_PATH_PER_T           60*n   

/*
 * Portable Uniform Integer Random Number in [0-2^31] range
 * Performs better than ansi-C rand() 
 * D.E Knuth, 1994 - The Stanford GraphBase
 */
#define RANDOM()        (*rand_fptr >= 0 ? *rand_fptr-- : flipCycle ()) 
#define two_to_the_31   ((unsigned long)0x80000000) 
#define RREAL           ((double)RANDOM()/(double)two_to_the_31)

static long A[56]= {-1};
long *rand_fptr = A;

#define mod_diff(x,y)   (((x)-(y))&0x7fffffff) 
long flipCycle()
{
	register long *ii,*jj;
	for (ii = &A[1], jj = &A[32]; jj <= &A[55]; ii++, jj++)
	*ii= mod_diff (*ii, *jj);

	for (jj = &A[1]; ii <= &A[55]; ii++, jj++)
	*ii= mod_diff (*ii, *jj);
	rand_fptr = &A[54];
	return A[55];
}

void initRand (long seed)
{
	register long i;
	register long prev = seed, next = 1;
	seed = prev = mod_diff (prev,0);
	A[55] = prev;
	for (i = 21; i; i = (i+21)%55)
	{
		A[i] = next;
		next = mod_diff (prev, next);
		if (seed&1) seed = 0x40000000 + (seed >> 1);
		else seed >>= 1;
		next = mod_diff (next,seed);
		prev = A[i];
	}
	
	for (i = 0; i < 7; i++) flipCycle(); 
}

long unifRand (long m)
{
	register unsigned long t = two_to_the_31 - (two_to_the_31%m);
	register long r;
	do {
		r = RANDOM();
	} while (t <= (unsigned long)r);
	return r%m;
}

/*
 * Defs
 */
#define MOD(i,n)    ((i) % (n) >= 0 ? (i) % (n) : (i) % (n) + (n))

typedef int Path[3];      /* specify how to change path */
typedef struct {
	float x, y;
	char *name;
} Point;       

/*
 * State vars
 */
int     n, verbose = 0;
Point   *cities;
int     *dist;
int     *iorder, *jorder;
float   b[4];

#define D(x,y) dist[(x)*n+y]		
#define SCALEX(x) (50+500.0/(b[1]-b[0])*(x - b[0]))
#define SCALEY(y) (50+700.0/(b[3]-b[2])*(y - b[2]))

float  stodeg(char *deg)
{
	int i,j,k,l,m,n,o;
	float x = 0;
	i = deg[0]=='N'||deg[0]=='E';
	j = deg[0]=='S'||deg[0]=='W';
	if (i||j) {
		++deg;
		o = sscanf(deg, "%2d%2d%2d%2d", &k, &l, &m, &n);
		x = k * 100.0 + l * 1.0 + m /100.0 + n / 10000.0 ;
	        if (j) x=-x; 
	} else x = atof(deg); 
        return x;
}

readCities(FILE *f)
{
	int i, j;
	int dx, dy;
	char sbuf[512];
	char sx[512], sy[512];

	if (f && (fscanf(f,"%d", &n) != 1)) {
		fprintf(stderr, "input syntax error\n");
		exit (-1);
	}
	if (verbose) fprintf (stderr, "Cities:\n%d\n", n);

	if (!(cities = (Point*) malloc (n * sizeof(Point))) ||
		    !(dist   = (int*) malloc (n * n * sizeof(int))) ||
		    !(iorder = (int*) malloc (n * sizeof(int)))   ||
		    !(jorder = (int*) malloc (n * sizeof(int))))
		{
			fprintf (stderr, "Memory allocation pb...\n");
			exit(-1);
		}
	for (i = 0; i < n; i++)
	{
		if (f) {			
			if (fscanf(f,"%s %s %[^\n]*", sx, sy, sbuf) != 3) {
				fprintf(stderr, "input syntax error\n");
				exit (-1);
			}
			cities[i].name = strdup(sbuf);
			cities[i].x  = stodeg(sx);
			cities[i].y  = stodeg(sy);
		} else {
			cities[i].x = unifRand (10*n);
			cities[i].y = unifRand (10*n);
			sprintf(sbuf, "%d",i);
			cities[i].name = strdup(sbuf);
		}
#define min(a,b) (a)<(b)?(a):(b)
#define max(a,b) (a)>(b)?(a):(b)
#define sqr(x)   ((x)*(x))
		if (i==0)
		{
			b[0]= cities[i].x; b[1]= b[0]; 
			b[2]= cities[i].y; b[3]= b[2];
		} else {
			b[0] = min(b[0],cities[i].x); b[1] = max(b[1],cities[i].x);
			b[2] = min(b[2],cities[i].y); b[3] = max(b[3],cities[i].y);
		}
		if (verbose)
		{			
			fprintf(stderr, "%d %d ", (int)cities[i].x, (int)cities[i].y);
			if ((i+1)%10 == 0) fprintf(stderr,"\n");
		}
	}
	if (verbose)
	{			
		fprintf(stderr,"[%d %d %d %d]\n",(int)b[0],
			(int)b[1], (int)b[2], (int)b[3]);
	}
	/* compute inter cities distance matrix */
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			dx = (int)SCALEX(cities[i].x)-(int)SCALEX(cities[j].x);
			dy = (int)SCALEY(cities[i].y)-(int)SCALEY(cities[j].y);

			/* D(i,j) satisfies triangle inequality */
			D(i,j) = sqrt ((int)(dx*dx + dy*dy));
		}
	}

}

printSol()
{
	double x, y;
	int i;
	printf ("%%!\n%%%%Path Length %d\n", pathLength(iorder));
	printf (".01 setlinewidth/l{lineto currentpoint 1.5 0 360 arc}def/m{moveto}def\n");
	printf ("/n{currentpoint 3 -1 roll dup stringwidth pop 2 div neg 5 rmoveto show moveto}def\n");
        printf ("/s{currentpoint stroke moveto}def/Helvetica findfont\n");
        printf ("8 scalefont setfont/st{gsave show grestore}def\n");		
	for (i = 0; i <= n; i++)
	{
		x = SCALEX(cities[iorder[i%n]].x); 
		y = SCALEY(cities[iorder[i%n]].y);
		if (!i) printf ("%f %f m",x,y);
		printf ("%s (%s) %f %f l %s\n", i%20==0&&i?" s\n":"",
			cities[iorder[i%n]].name,x,y,cities[iorder[i%n]].name?"n":"");
	}
	printf ("s showpage\n");
	fflush (stdout);
}

/*
 * Prim's approximated TSP tour
 * See also [Cristophides'92]
 */
findEulerianPath()
{
	int *mst, *arc;	
	int d, i, j, k, l, a;
	float maxd;
        if (!(mst = (int*) malloc(n * sizeof(int))) ||
	    !(arc = (int*) malloc(n * sizeof(int))))
	{
		fprintf (stderr, "Malloc failed.\n");
		exit(-3);
	}
/* re-use vars */
#define dis jorder
	
	maxd = sqr(b[1]-b[0])+sqr(b[3]-b[2]);
	d    = maxd;
	dis[0] = -1;
	for (i = 1; i < n; i++)
	{
		dis[i] = D(i,0); arc[i] = 0;
	        if (d > dis[i])
		{
			d = dis[i];
			j = i;
		}
	}

	/*
	 * O(n^2) Minimum Spanning Trees by Prim and Jarnick 
	 * for graphs with adjacency matrix. 
	 */
	for (a = 0; a < n - 1; a++)
	{
		mst[a] = j * n + arc[j]; /* join fragment j with MST */
		dis[j] = -1; 
		d = maxd;
		for (i = 0; i < n; i++)
		{
			if (dis[i] >= 0) /* not connected yet */
			{
				if (dis[i] > D(i,j))
				{
					dis[i] = D(i,j);
					arc[i] = j;
				}
				if (d > dis[i])
				{
					d = dis[i];
					k = i;
				}
			}
		}
		j = k;
	}

	/*
	 * Preorder Tour of MST
	 */
#define VISITED(x) jorder[x]
#define NQ(x) arc[l++] = x
#define DQ()  arc[--l]
#define EMPTY (l==0)
		
	for (i = 0; i < n; i++) VISITED(i) = 0;
	k = 0; l = 0; d = 0; NQ(0);
	while (!EMPTY)
	{
		i = DQ();
		if (!VISITED(i))
		{
			iorder[k++] = i;
			VISITED(i)  = 1;			
			for (j = 0; j < n - 1; j++) /* push all kids of i */
			{
				if (i == mst[j]%n) NQ(mst[j]/n); 
			}	
		}
	}
	printf ("%%Page 2\n.01 setlinewidth/l{lineto currentpoint 1.5 0 360 arc}def/m{moveto}def/s{stroke}def\n");		
	for (i = 0; i < n - 1; i++)
	{
		printf ("(%s) %f %f m n\n",
			cities[mst[i]/n].name,
			SCALEX(cities[mst[i]/n].x),
			SCALEY(cities[mst[i]/n].y));
		printf ("%f %f l %s\n",
			SCALEX(cities[mst[i]%n].x), 
			SCALEY(cities[mst[i]%n].y),
			i%20==0&&!i?"s":"");
	}
	printf ("s showpage\n");
	fflush (stdout);
	free (mst); free (arc);
}

int pathLength (int *iorder)
{
	int i, j, k;
	int len = 0;
	for (i = 0; i < n-1; i++)
	{
		len += D(iorder[i], iorder[i+1]);
	}
	len += D(iorder[n-1], iorder[0]); /* close path */
	return (len);
}

/*
 * Local Search Heuristics
 *  b-------a        b       a
 *  .       .   =>   .\     /.
 *  . d...e .        . e...d .  
 *  ./     \.        .       .
 *  c       f        c-------f
 */
int getThreeWayCost (Path p)
{
	int a, b, c, d, e, f;
	a = iorder[MOD(p[0]-1,n)]; b = iorder[p[0]];
	c = iorder[p[1]];   d = iorder[MOD(p[1]+1,n)];
	e = iorder[p[2]];   f = iorder[MOD(p[2]+1,n)];
	return (D(a,d) + D(e,b) + D(c,f) - D(a,b) - D(c,d) - D(e,f)); 
        /* add cost between d and e if non symetric TSP */ 
}

doThreeWay (Path p)
{
	int i, count, m1, m2, m3, a, b, c, d, e, f;
	a = MOD(p[0]-1,n); b = p[0];
	c = p[1];   d = MOD(p[1]+1,n);
	e = p[2];   f = MOD(p[2]+1,n);	
	
	m1 = MOD(n+c-b,n)+1;  /* num cities from b to c */
	m2 = MOD(n+a-f,n)+1;  /* num cities from f to a */
	m3 = MOD(n+e-d,n)+1;  /* num cities from d to e */

	count = 0;
	/* [b..c] */
	for (i = 0; i < m1; i++) jorder[count++] = iorder[MOD(i+b,n)];

	/* [f..a] */
	for (i = 0; i < m2; i++) jorder[count++] = iorder[MOD(i+f,n)];

	/* [d..e] */
	for (i = 0; i < m3; i++) jorder[count++] = iorder[MOD(i+d,n)];

	/* copy segment back into iorder */
	for (i = 0; i < n; i++) iorder[i] = jorder[i];
}

/*
 *   c..b       c..b
 *    \/    =>  |  |
 *    /\        |  |
 *   a  d       a  d
 */
int getReverseCost (Path p)
{
	int a, b, c, d;
	a = iorder[MOD(p[0]-1,n)]; b = iorder[p[0]];
	c = iorder[p[1]];   d = iorder[MOD(p[1]+1,n)];
	
	return (D(d,b) + D(c,a) - D(a,b) - D(c,d));
        /* add cost between c and b if non symetric TSP */ 
}

doReverse(Path p)
{
	int i, j, nswaps, first, last, tmp;

        /* reverse path b...c */
	nswaps = (MOD(p[1]-p[0],n)+1)/2;
	for (i = 0; i < nswaps; i++)
        {
		first = MOD(p[0]+i, n);
		last  = MOD(p[1]-i, n);
		tmp   = iorder[first];
		iorder[first] = iorder[last];
		iorder[last]  = tmp;
        }
}

annealing()
{
	Path p;
	int    i=1, j, pathchg;
	int    numOnPath, numNotOnPath;
	int    pathlen, bestlen;
	double energyChange, T;

	pathlen = pathLength (iorder); 
	bestlen = pathlen;

	for (T = T_INIT; T > FINAL_T; T *= COOLING)  /* annealing schedule */
        {
		pathchg = 0;
		for (j = 0; j < TRIES_PER_T; j++)
		{
			do {
				p[0] = unifRand (n);
				p[1] = unifRand (n);
				if (p[0] == p[1]) p[1] = MOD(p[0]+1,n); /* non-empty path */
				numOnPath = MOD(p[1]-p[0],n) + 1;
				numNotOnPath = n - numOnPath;
			} while (numOnPath < 2 || numNotOnPath < 2); /* non-empty path */
			
			if (RANDOM() % 2) /*  threeWay */
			{
				do {
					p[2] = MOD(unifRand (numNotOnPath)+p[1]+1,n);
				} while (p[0] == MOD(p[2]+1,n)); /* avoids a non-change */

				energyChange = getThreeWayCost (p);
				if (energyChange < 0 || RREAL < exp(-energyChange/T) )
				{
					pathchg++;
					pathlen += energyChange;
					doThreeWay (p);
				}
			}
			else            /* path Reverse */
			{
				energyChange = getReverseCost (p);
				if (energyChange < 0 || RREAL < exp(-energyChange/T))
				{
					pathchg++;
					pathlen += energyChange;
					doReverse(p); 
				}
			}
			if (pathlen < bestlen) bestlen = pathlen;
			if (pathchg > IMPROVED_PATH_PER_T) break; /* finish early */
		}   
		if (verbose) fprintf (stderr,  "T:%f L:%d B:%d C:%d\n", T, pathlen, bestlen, pathchg);
		if (pathchg == 0) break;   /* if no change then quit */
        }
}
	
main (int argc, char **argv)
{
	int   i;
	float r;
	FILE *f = stdin;
	long  seed = -314159L;
	
	while(--argc)
	{
		if (sscanf(argv[argc],"n=%d",&n)==1) f = NULL;
		else if (sscanf(argv[argc],"s=%ld",&seed)==1);
		else if (strcmp(argv[argc], "-v")==0) verbose = 1;
		else if ((f=fopen(argv[argc],"r"))==NULL)
		{
			fprintf(stderr,
				"Usage: %s [-v] [n=dd] [s=dd] [filename]\n",argv[0]);
			return-2;
		}
	}
	initRand (seed);
	readCities (f);

	/* identity permutation */
	for (i = 0; i < n; i++) iorder[i] = i; 
	printSol ();
	if (verbose)
	fprintf (stderr, "Initial Path Length: %d\n", pathLength(iorder));

	/*
	 * Set up first eulerian path iorder to be improved by
	 * simulated annealing. 
	 */
	findEulerianPath(); 
	printSol ();
	if (verbose)
	fprintf (stderr, "Approximated Path Length: %d\n", pathLength(iorder));
	
	annealing();
	printSol ();
	if (verbose)
	fprintf (stderr, "Best Path Length: %d\n", pathLength(iorder));
	fflush (stderr);
	
	for (i=1,r=0.0; i<n; i++)
	{ 
		r+=sqrt(sqr(cities[iorder[i]].x-cities[iorder[i-1]].x)+
			sqr(cities[iorder[i]].y-cities[iorder[i-1]].y));
	}
    FILE *file = fopen("output.txt", "w");
    fprintf(file, "%f 0\n", r);
    for (i = 0; i < n; i++) {
        fprintf(file, "%d ", iorder[i]);
    }
	if (verbose)
	fprintf (stderr, "Best Path Length in orig coord: %f\n", r);	
}
	
/* EOF */
