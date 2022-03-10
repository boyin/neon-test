#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "cpucycles.h"
#include "mock_std_mult.h"

#if (defined(TOOM))
#define polymul polymul_tc
#elif (defined(KARATSUBA))
#define polymul polymul_ka
#else
#define polymul polymul_sb
#endif

#define REPS 10

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

#if (defined(SIZE))
#else
#define SIZE 768
#endif

extern void polymul(int16_t *h, const int16_t *f, const int16_t *g, const int16_t n);
//extern unsigned int hal_get_time();

int main(){
  int c1,c2,cc[REPS],c=0;
  int i,j;
  int16_t f[768], g[768], h[1536], hh[1536];
  int n = SIZE;

  
#ifndef __aarch64__
  hal_init_perfcounters(1,1);
#endif
  
  for (i=0; i<REPS; i++) {
    for (j=0; j<n; j++) {
      f[j] = rand();
      g[j] = rand();
    }
    //for (j=0; j<N; j++) {
    //  printf("%d, %d, %d\n",j,f[j],g[j]);
    //}
    
    c1 = hal_get_time();
    polymul(h,f,g,n);
    c2 = hal_get_time();
    c += (cc[i] = c2-c1);
    mock_std_mult_2p16(hh,f,g,n);

    for (j=0; j<2*n; j++) {
      if ((h[j]-hh[j]) & 8191 !=0) {
	printf("%d %d %d\n",j,h[j],hh[j]);
	break;
      }
    }
  }
  qsort(cc, REPS, sizeof(int), cmpfunc);
  printf("everything okay, avg time = %d, median = %d\n",c/REPS,cc[REPS>>1]);
}

