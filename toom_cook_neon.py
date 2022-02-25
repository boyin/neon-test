#!/usr/bin/python3

from math import log,ceil,floor,sqrt
from toom_matrix import Toom_Matrix
import sys
import re

def round (x) :
    return(floor(x+0.5))

def cmod (a,b) :
    assert (b>0)
    r = (a + b//2) % b - (b//2)
    return r

def extended_gcd(a, b) :
    if a == 0:
        return (b, 0, 1)
    else:
        g, y, x = extended_gcd(b % a, a)
        return (g, x - (b // a) * y, y)

def mod_inverse(a, m) :
    g, x, y = extended_gcd(a, m)
    if (g ** 2 != 1):
        raise Exception('modular inverse does not exist')
    elif (g == 1):
        return x % m
    else : # (g == -1)
        return (-x) % m
    
def cmod_inverse(a, m) :
    return cmod(mod_inverse(a, m), m)

N = 4591
R = 2**16
NN = cmod_inverse(N, R)
NS = int(floor(log(N)/log(2.0))) - 1
N3 = round((R<<NS)/N)
N2 = round(R**2/N/2)
MC = 2**(int(floor(log(R**2/(N/2.0)**2/2)/log(2.0))))
K = 4

def toom_cook_neon_c (B, K) :
    M0,M1 = Toom_Matrix (K, N)
    print('''
#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if (defined(KARATSUBA))
#define polymul polymul_ka
#else
#define polymul polymul_sb
#endif


    
int16_t eval[] = {''')
    for i in range(2,2*K-1) :
        for j in range(K) :
            print(M0[i][j],end=",")
        print("")
    print('''};
int16_t interp[] ={''')
    for i in range(2*K-1) :
        for j in range(2*K-1) :
            print(M1[i][j],end=",")
        print("")
    print('''};
int32x4_t q00 = {0,0,0,0};

/*
int16x8_t reduce16x8 (int16x8_t a) {
  const int16x4_t N = {%d,%d,0,0};
  int16x8_t a1 = vqdmulhq_lane_s16(a, N, 1);
  a1 = vrshrq_n_s16(a1, %d);  
  a = vmlsq_lane_s16(a, a1, N, 0);
  return(a);
}
*/

static int32x4_t reduce32x4 (int32x4_t a) {
  int32x2_t N = {%d,%d};
  int32x4_t a1 = vqrdmulhq_lane_s32(a, N, 1);
  a = vmlsq_lane_s32(a, a1, N, 0);
  return(a);
}


extern void polymul(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n);

void polymul_tc(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n){

  int i, j, k, l, _K=%d, ll=%d;
  int16_t *ptr; 
  int16x4_t d0, d1, d2;
  int32x4_t q0, q1, q2;
  int16x8_t q10, q20;
''' % (N, N3, NS+1, N, N2, K, B))

    print('''  assert(n == _K * ll); 
  int L = (2*_K-3) * ll;  // number of extra buffer for multiplicands
  int LL = L + 2*ll; // number for extra buffer for products
  int16_t ff[L], gg[L], hh[2*LL];
    
  ptr = eval;
  for (l=0; l<_K-2; l++, ptr+=_K){
    for (j=0; j<ll; j+=4) {
      q1 = q2 = q00;
      for (i=0; i<_K; i++) {
        d0 = vld1_lane_s16(ptr++, d0, 0);
        d1 = vld1_s16(&f[i*ll+j]); 
        d2 = vld1_s16(&g[i*ll+j]);
	q1 = vmlal_lane_s16(q1, d1, d0, 0);
        q2 = vmlal_lane_s16(q2, d2, d0, 0);
      }
      ptr -= _K;
      reduce32x4(q1); reduce32x4(q2);
      d1 = vmovn_s32(q1); vst1_s16(&ff[l*ll+j], d1);
      d2 = vmovn_s32(q2); vst1_s16(&gg[l*ll+j], d2);
    }
  }
  polymul(hh,f,g,ll);
  polymul(hh+2*ll,f+(_K-1)*ll,g+(_K-1)*ll,ll);  
  for (l=0; l<_K-2; l++){
    polymul(hh+(1+l)*2*ll,ff+l*ll,gg+l*ll,ll);
  }
  memset(h+2*ll,0,2*(2*_K-2)*ll);  
  memcpy(h,hh,4*ll);
  ptr = interp + (2*_K-1);  
  for(l=1; l<2*_K-3; l++) {
    for (j=0; j<2*ll; j+=4) {
      d1 = vld1_s16(&h[i*ll+j]);
      q1 = vmovl_s16(d1);
      for (i=1; i<_K-1; i++) {
        d0 = vld1_lane_s16(ptr++, d0, 0);
        d1 = vld1_s16(&hh[i*2*ll+j]);
        q1 = vmlal_lane_s16(q1, d1, d0, 0);
      }
      ptr -=  (2*_K-1);
      reduce32x4(q1);
      d1 = vmovn_s32(q1); 
      vst1_s16(&h[l*ll+j], d1);
    }
  }
  for (j=0; j<2*ll; j+=8) {   
    q10 = vld1q_s16(&h[(2*_K-2)*ll+j]);
    q20 = vld1q_s16(&hh[2*ll+j]);
    q10 = vaddq_s16(q10, q20);
    vst1q_s16(&h[(2*_K-2)*ll+j], q10); 
  }

}''')



    



    
try : B = int(sys.argv[1])
except : B = 32    

try : K = int(sys.argv[2])
except : K = 3

toom_cook_neon_c(B, K) 
