#include<stdio.h> 
int cost=0; 

int main() 
{ 
  int n, a[10][10], visited[10]; 
  scanf("%d",&n); 
  get(a, n, visited); 
  mincost(1, n, a, visited); 
  put(); 
} 

// getting cost matrix
void get(int a[10][10], int n, int visited[10]) 
{ 
  int i, j, max, src, dest, wt; 
  for(i=1;i<=n;i++) { 
    visited[i]=0; 
    for(j=1;j<=n;j++) 
    { 
      a[i][j]=999;
    } 
  } 
  max=n*(n-1)/2; 
  for(i=1;i<=max;i++) { 
    printf("\nEnter source and destination:"); 
    scanf("d",&src,&dest); 
    if(src==0&&dest==0) break; 
    else { 
      printf("\nEnter the distance:"); 
      scanf("%d",&wt); 
      a[src][dest]=wt; 
      a[dest][src]=wt; 
    }  
  } 
  printf("\nCost Matrix"); 
  printf("\n~~~~~~~~~~~\n"); 
  for(i=1;i<=n;i++) {  
    for(j=1;j<=n;j++) { 
      if(a[i][j]!=999) printf("%d\t",a[i][j]); 
      else printf("0\t"); 
    } 
    printf("\n");
  } 
} 


void mincost(int city,int n,int a[10][10],int visited[]) 
{ 
  int i, ncity; 
  visited[city]=1; 
  printf("%d->",city); 
  ncity=least(city,n,a,visited); 
  if(ncity==999) { 
    ncity=1; 
    printf("%d",ncity); 
    cost+=a[city][ncity]; 
    return; 
  } 
  mincost(ncity,n,a,visited); 
} 

int least(int c,int n,int a[10][10],int visited[10]) 
{ 
  int i, nc=999, min=999, kmin; 
  for(i=1;i<=n;i++) { 
    if((a[c][i] !=0 ) && (visited[i] == 0)) { 
      if(a[c][i] < min) { 
        min=a[i][1]+a[c][i]; 
        kmin=a[c][i]; 
        nc=i; 
      } 
    } 
  } 
  if(min != 999) cost+=kmin; 
  return nc; 
} 

void put() { 
  printf("\nMinimun cost for visiting all cities:%d\n",cost); 
} 


/*
OUTPUT: 
~~~~~~ 

Travelling Salesman Problem Using Branch and Bound 
************************************************** 
Enter number of Cities:5 

Enter source and destination:1 2 

Enter the distance:3 

Enter source and destination:1 3 

Enter the distance:1 

Enter source and destination:1 4 

Enter the distance:5 

Enter source and destination:1 5 

Enter the distance:8 

Enter source and destination:2 3 

Enter the distance:6 

Enter source and destination:2 4 

Enter the distance:7 

Enter source and destination:2 5 

Enter the distance:9 

Enter source and destination:3 4 

Enter the distance:4 

Enter source and destination:3 5 

Enter the distance:2 

Enter source and destination:4 5 

Enter the distance:3 

Cost Matrix 
~~~~~~~~~~~ 
0 3 1 5 8 
3 0 6 7 9 
1 6 0 4 2 
5 7 4 0 3 
8 9 2 3 0 
1->3->5->4->2->1 
Minimum cost for visiting all cities:16 
*/





#include <iostream.h>
#include <conio.h>

int main() {
  int i, j, k, n, min, g[20][20], c[20][20], s, s1[20][1], s2, lb;
  cin >> n;
  for(i=1;i<=n;i++) {
    for(j=1;j<=n;j++) {
      c[i][j]=0;
    }
  }

  for(i=1;i<=n;i++) {
    for(j=1;j<=n;j++) {
      if(i==j) continue;
      else{
        cout << "input"<<i<<"to"<<j<<"cost:";
        cin >> c[i][j];
      }
    }
  }
    
  for(i=2;i<=n;i++) {
    g[i][0]=c[i][1];
  }
  
  for(i=2;i<=n;i++) {
    for(j=2;j<=n;j++) {
      if(i!=j) g[i][j]=c[i][j]+g[j][0];
    }
  }
  
  for(i=2;i<=n;i++) {
    for(j=2;j<=n;j++) {
      if(i!=j) break;
    }
  }

  for(k=2;k<=n;k++){
    if(i!=k && j!=k){ 
      if((c[i][j]+g[i][k])<(c[i][k]+g[k][j])) {
        g[i][j]=c[i][j]+g[j][k];
        s1[i][j]=j;
      }
      else {
        g[i][1]=c[i][k]+g[k][j];
        s1[i][1]=k;
      }
    }
  }
  
  min=c[1][2]+g[2][1];
  s=2;
  for(i=3;i<n;i++) {
    if((c[i][i]+g[i][i])<min) {
      min=c[1][i]+g[i][1];
      s=i;
    }
  }
  
  int y=g[i][1]+g[i][j]+g[i][i];
  
  lb=(y/2);
  cout<<"Edge Matrix";
  for(i=1;i<=n;i++) {
    cout<<"\n";
    for(j=1;j<=n;j++) {
      cout<<"\t"<<c[i][j];
    }
  }
  
  cout<<"\n min"<< min;
  cout<<"\n\b"<< lb;
  for(i=2;i<=n;i++) {
    if(s!=i && s1[s][1]!=i) {
      s2=i;
    }
  }
  
  cout<<"\n"<<1<<"-->"<<s<<"-->"<<s1[s][1]<<"-->"<<s2<<"-->"<<1<<"\n";
  
  return (0);
}

