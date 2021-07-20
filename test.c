#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "cpucycles.h"
#include "mock_std_mult.h"

extern void polymul(int16_t *h, const int16_t *f, const int16_t *g, const int16_t n);
//extern unsigned int hal_get_time();

int main(){
  int c1,c2,c=0;
  int i,j;
  int16_t f[768], g[768], h[1536], hh[1536];

  int N = 768;
  
  //init_perfcounters(1,1);
  
  for (i=0; i<10; i++) {
    for (j=0; j<N; j++) {
      f[j] = rand() % 4591 - 2295;
      g[j] = rand() % 4591 - 2295;
    }
    //for (j=0; j<N; j++) {
    //  printf("%d, %d, %d\n",j,f[j],g[j]);
    //}
    
    c1 = hal_get_time();
    polymul(h,f,g,N);
    c2 = hal_get_time();
    c += (c2-c1);
    mock_std_mult(hh,f,g,N);

    for (j=0; j<2*N; j++) {
      if (h[j]!=hh[j]) {
	printf("%d %d %d\n",j,h[j],hh[j]);
	break;
      }
    }
  }
  printf("everything is correct, time = %d\n",c);
}

