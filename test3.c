#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "cpucycles.h"
#include "mock_std_mult.h"

#if (defined(KARATSUBA))
#define polymul3 polymul3_ka
#else
#define polymul3 polymul3_sb
#endif

#define REPS 10

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}


extern void polymul(uint8_t *h, const uint8_t *f, const uint8_t *g, int n);
//extern unsigned int hal_get_time();

int main(){
  int c1,c2,cc[REPS],c=0;
  int i,j;
  int8_t f[768], g[768], h[1536], hh[1536];
  int n = 768; 
  //int n = 16;
  
#ifndef __aarch64__
  hal_init_perfcounters(1,1);
#endif
  
  for (i=0; i<REPS; i++) {
    for (j=0; j<n; j++) {
      f[j] = rand() % 3;
      g[j] = rand() % 3;
    }
    
    c1 = hal_get_time();
    polymul3(h,f,g,n);
    c2 = hal_get_time();
    c += (cc[i] = c2-c1);
    mock_std_mult_mod3(hh,f,g,n);

    for (j=0; j<2*n; j++) {
      if (h[j]!=hh[j]) {
	printf("%d %d %d\n",j,h[j],hh[j]);
	break;
      }
    }
  }
  qsort(cc, REPS, sizeof(int), cmpfunc);
  printf("everything okay, avg time = %d, median = %d\n",c/REPS,cc[REPS>>1]);
}

