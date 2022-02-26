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

def schoolbook_neon_block (B) :
    for i in range(B) : print("      u%d = vld1_s16(&g[i-j+%d]);" % (i,4*i))
    for i in range(B) :
        print("      v%d = vld1_s16(&f[j+%d]);" % (i, 4*i))
        print("")
        for j in range(B-1,-1,-1) :
            print("      a%d = vmlal_lane_s16(a%d,u%d,v%d,0);"%(i+j,i+j,j,i))
        print("      t  = vextq_s32(a%d,  z, 1);" % (B+i))
        for j in range(B,0,-1) :
            print("      a%d = vextq_s32(a%d, a%d, 1);" % (i+j,i+j-1,i+j))
        print("      a%d = vextq_s32(z,  a%d, 1);" % (i,i))		    
        print("")
        for k in range(1,4) :
            for j in range(B) :
                print("      a%d = vmlal_lane_s16(a%d,u%d,v%d,%d);"%(i+j+1,i+j+1,j,i,k))
            for j in range(B):
                print("      a%d = vextq_s32(a%d, a%d, 1);" % (i+j,i+j,i+j+1))
            print("      a%d = vextq_s32(a%d, t, 1);" % (i+B,i+B)) 
            if (k<3) : print("      t  = vextq_s32(t,  z,  1);")
            print("")
                
    print("    if (j & %d == %d) {" % (MC-1,MC-4*B))
    for i in range(2*B) :
        print("        a%d=reduce32x4(a%d);" % (i,i))
    print('''      }
    }''')
    for i in range(2*B) :
        print("    a%d=reduce32x4(a%d);" % (i,i))
    for i in range(2*B) :
        print("    a%d=reduce32x4(a%d);" % (i,i))
    for i in range(B) :
        print("    tt%d = vmovn_s32(a%d); vst1_s16(&h[i+%d],tt%d);" % (i,i,4*i,i))
    print("    ", end="")
    for i in range(B) : print("a%d=a%d; a%d^=a%d;" % (i,i+B,i+B,i+B), end=" ")
    print('''
  }''')

              
    

def schoolbook_neon_c (B) :
    print('''
#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>
#include <stdio.h>

static int32x4_t reduce32x4 (int32x4_t a) {
  int32x2_t N = {%d,%d};
  int32x4_t a1 = vqrdmulhq_lane_s32(a, N, 1);
  a = vmlsq_lane_s32(a, a1, N, 0);
  return(a);
}

void polymul_sb(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n){

  int i, j;
''' % (N, N2))

    print("  int16x4_t ", end="")
    for i in range(B) :
        print("u%d, v%d, tt%d" % (i,i,i), end="")
        if (i<B-1) : print(", ", end="")
    print(";")

    print("  int32x4_t ", end="")
    for i in range(B) :
        print("a%d, a%d, " %(i*2,i*2+1), end="")
    print("t%d, z, t, b;" % (B))

    print("  // assert(n %% %d == 0)" % (4*B))

    print("  ", end="")
    for i in range(2*B) :
        print("a%d ^= a%d; " % (i,i), end="")
    print("z ^= z;")

    print('''
  for (i = 0; i < n; i+=%d) {
    for (j = 0; j <= i; j+=%d) {
''' % (4*B, 4*B))

    schoolbook_neon_block(B)

    print('''
  for (; i < n * 2 - %d; i+=%d) {
    for (j = i - n + %d; j < n; j+=%d) {
''' % (4*B, 4*B, 4*B, 4*B))

    schoolbook_neon_block(B)
    
    for i in range(B) :
        print("  tt%d = vmovn_s32(a%d); vst1_s16(&h[i+%d],tt%d);" % (i,i,4*i,i))
    print("}")


try : B = int(sys.argv[1])
except : B = 2    

    
schoolbook_neon_c(B) 
