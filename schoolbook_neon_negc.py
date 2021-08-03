#!/usr/bin/python3 
from math import log,ceil,floor,sqrt
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
N1 = round(R**2/N)
N2 = round(R**2/N/2)
MC = 2**(int(floor(log(R**2/(N/2.0)**2/2)/log(2.0))))



              
    

def schoolbook_neon_c (B) :
    print('''
#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>
#include <stdio.h>

int32x4_t reduce32x4 (int32x4_t a) {
  int32x2_t N = {%d,%d};
  int32x4_t a1 = vqrdmulhq_lane_s32(a, N, 1);
  a = vmlsq_lane_s32(a, a1, N, 0);
  return(a);
}

void polymul_negc_sb(int16_t *h,const int16_t *f,const int16_t *g){

  int i, j;
''' % (N, N2))

    print("  int16x4_t ", end="")
    for i in range(B) :
        print("u%d, v%d, tt%d" % (i,i,i), end="")
        if (i<B-1) : print(", ", end="")
    print(";")

    print("  int32x4_t ", end="")
    for i in range(B) :
        print("a%d," % (i), end="")
    print("t;")
    
    print("  // assert(n %% %d == 0)" % (4*B))

    print("  ", end="")
    for i in range(B) :
        print("a%d ^= a%d; " % (i,i), end="")
    print("")

    for i in range(B-1,-1,-1) : print("  u%d = vld1_s16(&g[%d]);" % (i,4*i))
    print("")
    for i in range(B-1,-1,-1) :
        print("  v%d = vld1_s16(&f[%d]);" % (i, 4*i))
        print("")
        for j in range(3,-1,-1) :
            for k in range(B-1,-1,-1) :
                if (i==B-1 and j==3) :
                    print("  a%d = vmull_lane_s16(u%d,v%d,3);"%(k,k,i))
                else:
                    print("  a%d = vmlal_lane_s16(a%d,u%d,v%d,%d);"%(k,k,k,i,j))
            if (i>0 or j>0) :
                print("  t  = vnegq_s32(a%d);" % (B-1))
                for k in range(B-1,0,-1) :
                    print("  a%d = vextq_s32(a%d, a%d, 3);" % (k,k-1,k))
                print("  a0 = vextq_s32(t, a0, 3);")		    
        print("")
                
    for i in range(B-1,-1,-1) :
        print("  a%d=reduce32x4(a%d);" % (i,i))
    for i in range(B-1,-1,-1) :
        print("  a%d=reduce32x4(a%d);" % (i,i))
    for i in range(B-1,-1,-1) :
        print("  tt%d = vmovn_s32(a%d); vst1_s16(&h[%d],tt%d);" % (i,i,4*i,i))
    print("}")


try : B = int(sys.argv[1])
except : B = 2    

    
schoolbook_neon_c(B) 
