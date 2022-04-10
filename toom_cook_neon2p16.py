#!/usr/bin/python3

from math import log,ceil,floor,sqrt
from toom_matrices import Toom_Matrices
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

try : NAME = sys.argv[4]
except : NAME = "polymul_tc"

try : BOT = sys.argv[5]
except : BOT = "none"

R = 2**16

lgN = 0
while (2**lgN < N) : lgN += 1
assert (N == 2**lgN)



def toom_cook_neon_c (B, K) :
    M0,M1,ll = Toom_Matrices[(K, N)]

    print('''
#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>''')

    if (BOT == "none") :
        print('''
#if (defined(KARATSUBA))
#define polymul polymul_ka
#else
#define polymul polymul_sb
#endif''')
    else :
        print("#define polymul %s" % (BOT))
         
    print('''
#define NS %d

static int16x8_t qm = {%d,%d,%d,%d,%d,%d,%d,%d}; 

static int16_t eval[] = {''' % (ll,N-1,N-1,N-1,N-1,N-1,N-1,N-1,N-1))
    for i in range(2,2*K-1) :
        for j in range(K) :
            print(M0[i][j],end=",")
        for j in range(K,(K+3)//4*4) :
            print("0",end=",")
        print("")
    print('''};
static int16_t interp[] ={''')
    for i in range(2*K-1) :
        for j in range(2*K-1) :
            print(M1[i][j],end=",")
        for j in range(2*K-1,(2*K+2)//4*4) :
            print("0",end=",")
        print("")
    print('''};
static int16x8_t q0 = {0,0,0,0,0,0,0,0};

extern void polymul(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n);

void %s(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n){

  int i, j, k, l, _K=%d, ll=%d;
  int16_t *ptr;  

  assert(n == _K * ll); 
  int L = (2*_K-3) * ll;  // number of extra buffer for multiplicands
  int LL = L + 2*ll; // number for extra buffer for products
  int16_t ff[L], gg[L], hh[2*LL];''' % (NAME, K, B))
    for i in list(range(0,2*K-1,4)) :
        print("  int16x4_t d%d, d%d0" % (i,i), end="") 
        for j in range(1,2*K-1) :
            print(", d%d%d" % (i,j), end="")
        print(";")
    for i in [1,2] :
        print("  int16x8_t q%d, q%d0" % (i,i), end="") 
        for j in range(1,2*K-1) :
            print(", q%d%d" % (i,j), end="")
        print(";")
    print('''
  ptr = eval;''')
    for l in range(2*K-3) :
        for i in range(0,K-1,4) :
            print("  d%d%d = vld1_s16(ptr); ptr+=4;" % (i,l))
    print('''
  for (j=0; j<ll; j+=8) {''')
    for i in range(K) :    
        print("    q1%d = vld1q_s16(&f[%d*ll+j]);" % (i,i));
        print("    q2%d = vld1q_s16(&g[%d*ll+j]);" % (i,i));
    print("")
    for l in range(2*K-3) :
        for i in range(K) :
            if (i==0) :
                print("    q1 = vmulq_lane_s16(q10, d0%d, 0);" % l)
                print("    q2 = vmulq_lane_s16(q20, d0%d, 0);" % l)
            else :
                print("    q1 = vmlaq_lane_s16(q1, q1%d, d%d%d, %d);"%(i,i//4*4,l,i%4))
                print("    q2 = vmlaq_lane_s16(q2, q2%d, d%d%d, %d);"%(i,i//4*4,l,i%4))
        print('''    vst1q_s16(&ff[%d*ll+j], q1);
    vst1q_s16(&gg[%d*ll+j], q2);
''' % (l,l))
    print('''  }
  polymul(hh,f,g,ll);
  polymul(hh+2*ll,f+(_K-1)*ll,g+(_K-1)*ll,ll);  
  for (l=0; l<2*_K-3; l++){
    polymul(hh+(2+l)*2*ll,ff+l*ll,gg+l*ll,ll);
  }
  memcpy(h,hh,2*ll);  
  memcpy(h+(2*_K-1)*ll,hh+3*ll,2*ll);
  ptr = interp+ %d;''' % ((2*K+2)//4*4))
    for l in range(2*K-3) :
        for i in range(0,2*K-1,4) :
            print("  d%d%d = vld1_s16(ptr); ptr+=4;" % (i,l))
    print('''
  for (j=0; j<ll; j+=8) {''')
    for i in range(2*K-1) :    
        print("    q1%d = vld1q_s16(&hh[(%d*2)*ll+j]);" % (i,i));
    for i in range(2*K-1) :
        for l in range(2*K-3) :
            if (i==0) : print("    q2%d = vmulq_lane_s16(q10, d0%d, 0);" %(l,l))
            else : print("    q2%d = vmlaq_lane_s16(q2%d, q1%d, d%d%d, %d);" % (l,l,i,i//4*4,l,i%4))
    print('''
    q2 = vld1q_s16(&hh[ll+j]);
    q2 = vsraq_n_s16(q2,q20,NS);''')
    if (2**ll*N<2**16) : print("    q2 = vandq_s16(q2,qm);")
    print("    vst1q_s16(&h[ll+j],q2);")
    for l in range(1,2*K-3) :
        print("    vst1q_s16(&h[(1+%d)*ll+j],q2%d);" % (l,l))
    print('''  }''')
    print('''
  for (j=0; j<ll; j+=8) {''')
    for i in range(2*K-1) :    
        print("    q1%d = vld1q_s16(&hh[(%d*2+1)*ll+j]);" % (i,i));
    for l in range(2*K-3) :
        if (l<2*K-4) :
            print("    q2%d = vld1q_s16(&h[(2+%d)*ll+j]);" %(l,l))
        else : print("    q2%d = q0;" %(l))
    for i in range(2*K-1) :
        for l in range(2*K-3) :
            print("    q2%d = vmlaq_lane_s16(q2%d, q1%d, d%d%d, %d);" % (l,l,i,i//4*4,l,i%4))
    for l in range(2*K-4) :
        print('''
    q2%d = vshrq_n_s16(q2%d,NS);''' % (l,l))
        if (2**ll*N<2**16) : print("    q2%d = vandq_s16(q2%d,qm);" % (l,l))
        print("    vst1q_s16(&h[(%d+2)*ll+j],q2%d);" % (l,l))
    print('''
    q2 = vld1q_s16(&hh[2*ll+j]);
    q2 = vsraq_n_s16(q2,q2%d,NS);'''% (2*K-4))
    if (2**ll*N<2**16) : print("    q2 = vandq_s16(q2,qm);")
    print('''    vst1q_s16(&h[(2+%d)*ll+j], q2);
  }
}''' % (2*K-4))



    




toom_cook_neon_c(B, K) 
