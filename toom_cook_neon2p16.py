#!/usr/bin/python3

from math import log,ceil,floor,sqrt
from toom_matrix_sage import Toom_Matrix3
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

try : B = int(sys.argv[1])
except : B = 16    

try : K = int(sys.argv[2])
except : K = 4

try : N = int(sys.argv[3])
except : N = 8192

R = 2**16

lgN = 0
while (2**lgN < N) : lgN += 1
assert (N == 2**lgN)



def toom_cook_neon_c (B, K) :
    M0,M1,l = Toom_Matrix3 (K, N)
    M0 = eval(M0)
    M1 = eval(M1)
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

#define NS %d

int16_t eval[] = {''' % (l))
    for i in range(2,2*K-1) :
        for j in range(K) :
            print(M0[i][j],end=",")
        for j in range(K,(K+3)//4*4) :
            print("0",end=",")
        print("")
    print('''};
int16_t interp[] ={''')
    for i in range(2*K-1) :
        for j in range(2*K-1) :
            print(M1[i][j],end=",")
        for j in range(2*K-1,(2*K+2)//4*4) :
            print("0",end=",")
        print("")
    print('''};
int16x8_t q0 = {0,0,0,0,0,0,0,0};

extern void polymul(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n);

void polymul_tc(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n){

  int i, j, k, l, _K=%d, ll=%d;
  int16_t *ptr;  
  int16x8_t q1, q2, q10, q20;

  assert(n == _K * ll); 
  int L = (2*_K-3) * ll;  // number of extra buffer for multiplicands
  int LL = L + 2*ll; // number for extra buffer for products
  int16_t ff[L], gg[L], hh[2*LL];''' % (K, B))
    for i in range(0,2*K-1,4) :
        print("  int16x4_t d%d;" % (i))
    print('''
  ptr = eval;
  for (l=0; l<2*_K-3; l++){''')
    for i in range(0,K-1,4) :
        print("    d%d = vld1_s16(ptr); ptr+=4;" % (i))
    print('''
    for (j=0; j<ll; j+=8) {''')
    for i in range(K) :
        print("      q10 = vld1q_s16(&f[%d*ll+j]);" % (i));
        print("      q20 = vld1q_s16(&g[%d*ll+j]);" % (i));
        if (i==0) :
            print("      q1 = vmulq_lane_s16(q10, d0, 0);")
            print("      q2 = vmulq_lane_s16(q20, d0, 0);")
        else :
            print("      q1 = vmlaq_lane_s16(q1, q10, d%d, %d);" % (i//4*4,i%4))
            print("      q2 = vmlaq_lane_s16(q2, q20, d%d, %d);" % (i//4*4,i%4))
        
    print('''
      vst1q_s16(&ff[l*ll+j], q1);
      vst1q_s16(&gg[l*ll+j], q2);
    }
  }
  polymul(hh,f,g,ll);
  polymul(hh+2*ll,f+(_K-1)*ll,g+(_K-1)*ll,ll);  
  for (l=0; l<2*_K-3; l++){
    polymul(hh+(2+l)*2*ll,ff+l*ll,gg+l*ll,ll);
  }
  memset(h+2*ll,0,2*(2*_K-2)*ll);  
  memcpy(h,hh,2*ll);
  memcpy(h+(2*_K-1)*2*ll,hh+6*ll,2*ll);  
  ptr = interp + %d;
  for(l=1; l<2*_K-2; l++) {''' % ((2*K+2)//4*4))

    for i in range(0,2*K-1,4) :
        print("    d%d = vld1_s16(ptr); ptr+=4;" % (i))
    print('''
    for (j=0; j<2*ll; j+=8) {
      q1 = vld1q_s16(&h[l*ll+j]);''')
        
    for i in range(2*K-1) :
        print("      q10 = vld1q_s16(&hh[%d*ll+j]);" % (2*i));
        print("      q1 = vmlaq_lane_s16(q1, q10, d%d, %d);" % (i//4*4,i%4))

    print('''
      vst1q_s16(&h[l*ll+j], q1);
    }
  }
  for (j=0; j<ll; j+=8) {   
    q1 = vld1q_s16(&h[(2*_K-2)*ll+j]);
    q2 = vld1q_s16(&h[ll+j]);
    q10 = vld1q_s16(&hh[2*ll+j]);
    q20 = vld1q_s16(&hh[ll+j]);
    q10 = vsraq_n_s16(q10, q1, NS);
    q20 = vsraq_n_s16(q20, q2, NS);
    vst1q_s16(&h[(2*_K-2)*ll+j], q10); 
    vst1q_s16(&h[ll+j],q20);
  }

}''')



    




toom_cook_neon_c(B, K) 
