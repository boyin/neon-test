#!/usr/bin/python3

import random
from math import log,ceil,floor,sqrt
#from toom_matrix_sage import Toom_Matrix3
import sys
import re

#def testgen (i) :
#    yield("    A%d = vaddq_u32  ( A%d, B%d);"  % (i,i,i))
#    yield("    D%d = veorq_u32  ( D%d, A%d);"  % (i,i,i))
#    yield("    DD%d= vshlq_n_u32( D%d, 16);"         % (i,i))
#    yield("    DD%d= vsraq_n_u32(DD%d, D%d, 16);"% (i,i,i))
#    #yield("    DD%d = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D%d)));" % (i, i));
#    yield("    C%d = vaddq_u32  ( C%d,DD%d);"  % (i,i,i))
#    yield("    B%d = veorq_u32  ( B%d, C%d);"     % (i,i,i))
#    yield("    BB%d= vshlq_n_u32( B%d, 12);"         % (i,i))
#    yield("    BB%d= vsraq_n_u32(BB%d, B%d, 20);" % (i,i,i))
#    yield("    A%d = vaddq_u32  ( A%d,BB%d);"  % (i,i,i))
#    yield("    DD%d= veorq_u32  (DD%d, A%d);"  % (i,i,i))
#    yield("    D%d = vshlq_n_u32(DD%d, 8);"          % (i,i))
#    yield("    D%d = vsraq_n_u32( D%d,DD%d, 24);" % (i,i,i))
#    #yield("    D%d = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD%d), IDX));" % (i,i))
#    yield("    C%d = vaddq_u32  ( C%d, D%d);"  % (i,i,i))
#    yield("    BB%d= veorq_u32  (BB%d, C%d);"  % (i,i,i))
#    yield("    B%d = vshlq_n_u32(BB%d, 7);"          % (i,i))
#    yield("    B%d = vsraq_n_u32( B%d,BB%d, 25);" % (i,i,i))

def testgen (i) :
    yield("    A%d = vaddq_u32  ( A%d, B%d);"  % (i,i,i))
    yield("    D%d = veorq_u32  ( D%d, A%d);"  % (i,i,i))
    #yield("    DD%d= vshlq_n_u32( D%d, 16);"         % (i,i))
    #yield("    D%d = vsraq_n_u32(DD%d, D%d, 16);"% (i,i,i))
    yield("    D%d = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D%d)));" % (i, i));
    yield("    C%d = vaddq_u32  ( C%d, D%d);"  % (i,i,i))
    yield("    B%d = veorq_u32  ( B%d, C%d);"     % (i,i,i))
    yield("    BB%d= vshlq_n_u32( B%d, 12);"         % (i,i))
    yield("    B%d = vsraq_n_u32(BB%d, B%d, 20);" % (i,i,i))
    yield("    A%d = vaddq_u32  ( A%d, B%d);"  % (i,i,i))
    yield("    D%d = veorq_u32  ( D%d, A%d);"  % (i,i,i))
    #yield("    DD%d= vshlq_n_u32( D%d, 8);"          % (i,i))
    #yield("    D%d = vsraq_n_u32(DD%d, D%d, 24);" % (i,i,i))
    yield("    D%d = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(D%d), IDX));" % (i,i))
    yield("    C%d = vaddq_u32  ( C%d, D%d);"  % (i,i,i))
    yield("    B%d = veorq_u32  ( B%d, C%d);"  % (i,i,i))
    yield("    BB%d= vshlq_n_u32( B%d, 7);"          % (i,i))
    yield("    B%d = vsraq_n_u32(BB%d, B%d, 25);" % (i,i,i))




R = 4    

print('''
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <arm_neon.h>

static const __attribute__((aligned(16))) uint8_t sigma[16] = "expand 32-byte k";
static const __attribute__((aligned(16))) uint64x2_t T1 = {1LL, 0LL};
static const __attribute__((aligned(16))) uint8x16_t IDX = {3, 0, 1, 2, 7, 4, 5, 6,11, 8, 9,10, 15,12,13,14}; 

#define ROUNDS 20

static inline int crypto_core_chacha20_neon(unsigned char *out, unsigned char *in){
  int i;
  uint32x4_t T, AA, BB, CC;''')
for i in range(R) : print("  uint32x4_t A%d, B%d, BB%d, C%d, D%d, DD%d;" % ((i,)*6))

for i in range(3) :
    print("  ",  end="")
    for j in range(R) :  print("%s%d = " % (chr(65+i),j), end="")
    print("vld1q_u32((void *)(in+%d));" % (16*i))
    
print("  D0 = vld1q_u32((void *)in+48);")
for j in range(1,R) : print("  D%d = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D%d)));" % (j,j-1))
print("  T  = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D%d)));" % (R-1))
for j in range(R) : print("  vst1q_u32((void *)(out+ %3d), D%d);" % (48+64*j,j))
print("  vst1q_u32((void *)(in+48),T);")
print('''
  for (i=ROUNDS/2; i>0; i--) {
''')

G = [testgen(j) for j in range(R)]
for i in range(14) :
    for j in range(R) : print(next(G[j]))

for i in range(1,4) :
    S = chr(65+i)
    for j in range(R) :
        print("    %s%d = vextq_u32(%s%d, %s%d, %d);" % (S,j,S,j,S,j,i))
        
G = [testgen(j) for j in range(R)]
for i in range(14) :
    for j in range(R) : print(next(G[j]))

for i in range(1,4) :
    S = chr(65+i)
    for j in range(R) :
        print("    %s%d = vextq_u32(%s%d, %s%d, %d);" % (S,j,S,j,S,j,4-i))

print("  }")

for k in range(3) :
    print("  %s = vld1q_u32((void *)(in+%d));" % (chr(65+k)*2,16*k))
for j in range(R) :
    for k in range(3) :
        print("  %s%d = vaddq_u32(%s%d, %s);" % (chr(65+k),j,chr(65+k),j,chr(65+k)*2))
        
for j in range(R) :
    print("  DD%d = vld1q_u32((void *)(out+%d));" % (j,48+64*j))
for j in range(R) :
    print("  D%d = vaddq_u32(D%d,DD%d);" % (j,j,j))
    
for j in range(R) :
    for k in range(4) :
        print("  vst1q_u32((void *)(out+%3d), %s%d);" % (64*j+16*k,chr(65+k),j)) 

print('''}

int crypto_stream_chacha20(unsigned char *c,unsigned long long clen, const unsigned char *n, const unsigned char *k)
{
  unsigned char block3[%d] __attribute__((aligned(16)));
  unsigned char setup[64] __attribute__((aligned(16)));
  uint64_t C = 0;
  int i;
  
  if (!clen) return 0;

  vst1q_u32((void *)setup,  vld1q_u32((void *)sigma));
  vst1q_u32((void *)(setup+16), vld1q_u32((void *)k));
  vst1q_u32((void *)(setup+32), vld1q_u32((void *)(k+16)));
  *((uint64_t *)(setup+48)) = C;
  *((uint64_t *)(setup+56)) = *((uint64_t *)(n));

  while(clen >= %d) {
    crypto_core_chacha20_neon(c,setup);
    clen -= %d;
    c += %d;
  }

  if (clen) {
    crypto_core_chacha20_neon(block3,setup);
    for (i = 0;i < clen;++i) c[i] = block3[i];
  }
  return 0;
}''' % ((R*64,) * 4))


      
