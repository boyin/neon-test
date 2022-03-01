#!/usr/bin/python3 
from math import log,ceil,floor,sqrt
import sys
import re



def schoolbook_neon_block (B) :
    for i in range(B) : print("      u%d = vld1q_s16(&g[i-j+%d]);" % (i,8*i))
    for i in range(B) :
        print("      v%d0 = vld1_s16(&f[j+%d]);" % (i, 8*i))
        print("      v%d1 = vld1_s16(&f[j+%d]);" % (i, 8*i+4))
        print("")
        for j in range(B-1,-1,-1) :
            print("      a%d = vmlaq_lane_s16(a%d,u%d,v%d0,0);"%(i+j,i+j,j,i))
        print("      t  = vextq_s16(a%d,  z, 1);" % (B+i))
        for j in range(B,0,-1) :
            print("      a%d = vextq_s16(a%d, a%d, 1);" % (i+j,i+j-1,i+j))
        print("      a%d = vextq_s16(z,  a%d, 1);" % (i,i))		    
        print("")
        for k in range(1,8) :
            for j in range(B) :
                print("      a%d = vmlaq_lane_s16(a%d,u%d,v%d%d,%d);"%(i+j+1,i+j+1,j,i,k//4,k%4))
            for j in range(B):
                print("      a%d = vextq_s16(a%d, a%d, 1);" % (i+j,i+j,i+j+1))
            print("      a%d = vextq_s16(a%d, t,  1);" % (i+B,i+B)) 
            if (k<7) : print("      t  = vextq_s16(t,  z,  1);")
            print("")
    print("    }")
    for i in range(B) :
        print("    vst1q_s16(&h[i+%d],a%d);" % (8*i,i))
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

void polymul_sb(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n){

  int i, j;''')

    print("  int16x4_t ", end="")
    for i in range(B) :
        print("v%d1, v%d0" % (i,i), end="")
        if (i<B-1) : print(", ", end="")
    print(";")

    print("  int16x8_t ", end="")
    for i in range(B) :
        print("u%d, a%d, a%d, " %(i, i*2,i*2+1), end="")
    print("t%d, z, t, b;" % (B))

    print("  // assert(n %% %d == 0)" % (4*B))

    print("  ", end="")
    for i in range(2*B) :
        print("a%d ^= a%d; " % (i,i), end="")
    print("z ^= z;")

    print('''
  for (i = 0; i < n; i+=%d) {
    for (j = 0; j <= i; j+=%d) {
''' % (8*B, 8*B))

    schoolbook_neon_block(B)

    print('''
  for (; i < n * 2 - %d; i+=%d) {
    for (j = i - n + %d; j < n; j+=%d) {
''' % (8*B, 8*B, 8*B, 8*B))

    schoolbook_neon_block(B)
    
    for i in range(B) :
        print("  vst1q_s16(&h[i+%d],a%d);" % (8*i,i))
    print("}")


try : B = int(sys.argv[1])
except : B = 2    

    
schoolbook_neon_c(B) 
