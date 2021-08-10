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
    for i in range(B) : print("      u%d = vld1q_u32(&g[i-j+%d]);" % (i,16*i))
    for i in range(B) :
        print("      v%d_0 = vld1_u32(&f[j+%d]);" % (i, 16*i))
        print("      v%d_1 = vld1_u32(&f[j+%d]);" % (i, 16*i+8))
        print("")
        for j in range(B-1,-1,-1) :
            print("      a%d = vmlaq_lane_u32(a%d,u%d,v%d_0,0);"%(i+j,i+j,j,i))
        print("      t  = vextq_u32(a%d,  z, 1);" % (B+i))
        for j in range(B,0,-1) :
            print("      a%d = vextq_u32(a%d, a%d, 1);" % (i+j,i+j-1,i+j))
        print("      a%d = vextq_u32(z,  a%d, 1);" % (i,i))
        for j in range(B-1,-1,-1) :
            print("      b  = vqdmulhq_lane_s32(vreinterpretq_s32_u32(u%d),vreinterpret_s32_u32(v%d_0),0);"%(j,i))
            print("      a%d = vsraq_n_u32(a%d,vreinterpretq_u32_s32(b),1);"%(i+j+1,i+j+1))
        print("")
        for k in range(1,4) :
            for j in range(B) :
                print("      a%d = vmlaq_lane_u32(a%d,u%d,v%d_%d,%d);"%(i+j+1,i+j+1,j,i,k//2,k%2))
            for j in range(B):
                print("      a%d = vextq_u32(a%d, a%d, 1);" % (i+j,i+j,i+j+1))
            print("      a%d = vextq_u32(a%d, t, 1);" % (i+B,i+B))
            if (k<3) : print("      t  = vextq_u32(t,  z,  1);")
            for j in range(B-1,-1,-1) :
                print("      b  = vqdmulhq_lane_s32(vreinterpretq_s32_u32(u%d),vreinterpret_s32_u32(v%d_%d),%d);"%(j,i,k//2,k%2))
                print("      a%d = vsraq_n_u32(a%d,vreinterpretq_u32_s32(b),1);"%(i+j+1,i+j+1))
                print("")
    for i in range(2*B) :
        print("      a%d=reduce3_32x4(a%d,cf);" % (i,i))
    print('''    }''')
    for i in range(B) :
        print("    a%d=reduce3_x_32x4(a%d,c3);" % (i,i))
    for i in range(B) :
        print("    vst1q_u32(&h[i+%d],a%d);" % (16*i,i))
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

uint32x4_t cf = {0x0f0f0f0f,0x0f0f0f0f,0x0f0f0f0f,0x0f0f0f0f};
uint32x4_t c3 = {0x03030303,0x03030303,0x03030303,0x03030303};

uint32x4_t reduce3_32x4(uint32x4_t x, uint32x4_t cf) {
  uint32x4_t t = vbicq_u32(x, cf);
  x = vandq_u32(x, cf);
  x = vsraq_n_u32(x, t, 4); // down to 30
  return(x);
}

uint32x4_t reduce3_x_32x4(uint32x4_t x, uint32x4_t c3) {
  uint32x4_t t = vbicq_u32(x, c3);
  x = vandq_u32(x, c3);
  x = vsraq_n_u32(x, t, 2); // down to 9
  t = vbicq_u32(x, c3);
  x = vandq_u32(x, c3);
  x = vsraq_n_u32(x, t, 2); // down to 5
  uint8x16_t t8 = vcgeq_u8(vreinterpretq_u8_u32(x),vreinterpretq_u8_u32(c3));
  t8 = vandq_u8(t8,vreinterpretq_u8_u32(c3));
  return(vsubq_u32(x,vreinterpretq_u32_u8(t8)));
}

/*
void print16(uint32x4_t x) {
  int i,j;
  printf("(");
  for(i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      printf("+%u*x^(%d)", (x[i]>>(j*8)) & 0xFF, 4*i+j);
    }
  }
  printf(")");
}
*/

void polymul3_sb(uint8_t *h,const uint8_t *f,const uint8_t *g,const int n){

  int i, j;
''')

    print("  uint32x4_t ", end="")
    for i in range(B) :
        print("u%d, a%d, a%d, " % (i,2*i,2*i+1), end="")
    print('''z, t;
  int32x4_t b;''')
    
    print("  uint32x2_t ", end="")
    for i in range(B) :
        if (i<B-1) : print("v%d_0, v%d_1, " %(i,i), end="")
        else : print("v%d_0, v%d_1;" % (i,i))

    print("  // assert(n %% %d == 0)" % (4*B))

    print("  ", end="")
    for i in range(2*B) :
        print("a%d ^= a%d; " % (i,i), end="")
    print("z ^= z;")

    print('''
  for (i = 0; i < n; i+=%d) {
    for (j = 0; j <= i; j+=%d) {
''' % (16*B, 16*B))

    schoolbook_neon_block(B)

    print('''
  for (; i < n * 2 - %d; i+=%d) {
    for (j = i - n + %d; j < n; j+=%d) {
''' % (16*B, 16*B, 16*B, 16*B))

    schoolbook_neon_block(B)
    
    for i in range(B) :
        print("  a%d = reduce3_x_32x4(a%d,c3);" % (i,i))
        print("  vst1q_u32(&h[i+%d],a%d);" % (16*i,i))
    print("}")


try : B = int(sys.argv[1])
except : B = 2    

    
schoolbook_neon_c(B) 
