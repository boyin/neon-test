#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "cpucycles.h"
#include "mock_std_mult.h"

#if (defined(KARATSUBA))
#define polymul polymul_negc_ka
#else
#define polymul polymul_negc_sb
#endif

#define REPS 10

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}


extern void polymul(int16_t *h, const int16_t *f, const int16_t *g);
//extern unsigned int hal_get_time();

int main(){
  int c1,c2,cc[REPS],c=0;
  int i,j;
  int16_t f[768], g[768], h[1536], hh[1536];
  int n = 16; 
  //int n = 64;
  
#ifndef __aarch64__
  hal_init_perfcounters(1,1);
#endif
  
  for (i=0; i<REPS; i++) {
    for (j=0; j<n; j++) {
      f[j] = rand() % 4591 - 2295;
      g[j] = rand() % 4591 - 2295;
    }
    //for (j=0; j<N; j++) {
    //  printf("%d, %d, %d\n",j,f[j],g[j]);
    //}
    
    c1 = hal_get_time();
    polymul(h,f,g);
    c2 = hal_get_time();
    c += (cc[i] = c2-c1);
    mock_std_mult(hh,f,g,n);

    for (j=0; j<n; j++) {
      hh[j] -= hh[j+n];
      if ((h[j]-hh[j]) % 4591 != 0) {
	printf("%d %d %d\n",j,h[j],hh[j]);
	break;
      }
    }
  }
  qsort(cc, REPS, sizeof(int), cmpfunc);
  printf("everything okay, avg time = %d, median = %d\n",c/REPS,cc[REPS>>1]);
}

