#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "cpucycles.h"
#include "mock_std_mult.h"

#define tmvp tmvp_sb

#define REPS 20

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

#if (defined(SIZE))
#if (defined(SIZE2))
#else
#define SIZE2 ((SIZE+15)/16*16)
#endif
#else
#define SIZE 701
#define SIZE2 720
#endif


extern void polymul(int16_t *h, const int16_t *f, const int16_t *g, const int16_t n);
//extern unsigned int hal_get_time();

int main(){
  int c1,c2,cc[REPS],c=0;
  int i,j;
  int16_t f[SIZE], g[SIZE2*2], h[SIZE2], hh[SIZE];
  int n = SIZE;
  int nn = SIZE2;
  
#ifndef __aarch64__
  hal_init_perfcounters(1,1);
#endif
  
  for (i=0; i<REPS; i++) {
    for (j=0; j<n; j++) {
      f[j] = rand();
      g[nn-n+j] = g[nn+j] = rand();
    }
    for (j=n; j<nn; j++) {
      f[j] = 0;
      g[nn+j] = 0;
      g[nn-j] = 0;
    }
    g[0] = 0;
    //for (j=0; j<2*nn; j++) printf("%d ",g[j]); printf("\n");
    //for (j=0; j<nn; j++) printf("%d ",f[j]); printf("\n");
    
    c1 = hal_get_time();
    tmvp(h,f,g,nn);
    c2 = hal_get_time();
    c += (cc[i] = c2-c1);
    mock_std_conv_2p16(hh,f,g+nn,n);

    for (j=0; j<n; j++) {
      if (h[j]!=hh[j]) {
	printf("%d %d %d %d\n",i,j,h[j],hh[j]);
	break;
      }
    }
  }
  qsort(cc, REPS, sizeof(int), cmpfunc);
  printf("everything okay, avg time = %d, median = %d\n",c/REPS,cc[REPS>>1]);
}

