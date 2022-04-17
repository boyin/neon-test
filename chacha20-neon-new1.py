#!/usr/bin/python3

import random
from math import log,ceil,floor,sqrt
#from toom_matrix_sage import Toom_Matrix3
import sys
import re

def testgen (i, j) :
    iA = i; iB = (i+j) % 4; iC = (i + 2*j) % 4; iD = (i + 3*j) % 4 
    yield("    A.val[%d] = vaddq_u32  ( A.val[%d], B.val[%d]);"  % (iA,iA,iB))
    yield("    D.val[%d] = veorq_u32  ( D.val[%d], A.val[%d]);"  % (iD,iD,iA))
    #yield("    DD.val[%d]= vshlq_n_u32( D.val[%d], 16);"         % (iD,iD))
    #yield("    DD.val[%d]= vsraq_n_u32(DD.val[%d], D.val[%d], 16);"% (iD,iD,iD))
    yield("DD.val[%d] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[%d])));" % (iD, iD));
    yield("    C.val[%d] = vaddq_u32  ( C.val[%d],DD.val[%d]);"  % (iC,iC,iD))
    yield("    B.val[%d] = veorq_u32  ( B.val[%d], C.val[%d]);"     % (iB,iB,iC))
    yield("    BB.val[%d]= vshlq_n_u32( B.val[%d], 12);"         % (iB,iB))
    yield("    BB.val[%d]= vsraq_n_u32(BB.val[%d], B.val[%d], 20);" % (iB,iB,iB))
    yield("    A.val[%d] = vaddq_u32  ( A.val[%d],BB.val[%d]);"  % (iA,iA,iB))
    yield("    DD.val[%d]= veorq_u32  (DD.val[%d], A.val[%d]);"  % (iD,iD,iA))
    #yield("    D.val[%d] = vshlq_n_u32(DD.val[%d], 8);"          % (iD,iD))
    #yield("    D.val[%d] = vsraq_n_u32( D.val[%d],DD.val[%d], 24);" % (iD,iD,iD))
    yield("    D.val[%d] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[%d]), IDX));" % (iD,iD))
    yield("    C.val[%d] = vaddq_u32  ( C.val[%d], D.val[%d]);"  % (iC,iC,iD))
    yield("    BB.val[%d]= veorq_u32  (BB.val[%d], C.val[%d]);"  % (iB,iB,iC))
    yield("    B.val[%d] = vshlq_n_u32(BB.val[%d], 7);"          % (iB,iB))
    yield("    B.val[%d] = vsraq_n_u32( B.val[%d],BB.val[%d], 25);" % (iB,iB,iB))

    
def transpose_u32(S,T) :
    yield("  %s.val[0] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(%s.val[0]),vreinterpretq_u64_u32(%s.val[2])));" % (T,S,S))
    yield("  %s.val[2] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(%s.val[0]),vreinterpretq_u64_u32(%s.val[2])));" % (T,S,S))
    yield("  %s.val[1] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(%s.val[1]),vreinterpretq_u64_u32(%s.val[3])));" % (T,S,S))
    yield("  %s.val[3] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(%s.val[1]),vreinterpretq_u64_u32(%s.val[3])));" % (T,S,S))
    yield("  %s.val[0] = vtrn1q_u32(%s.val[0],%s.val[1]);" % (S,T,T))
    yield("  %s.val[1] = vtrn2q_u32(%s.val[0],%s.val[1]);" % (S,T,T))
    yield("  %s.val[2] = vtrn1q_u32(%s.val[2],%s.val[3]);" % (S,T,T))
    yield("  %s.val[3] = vtrn2q_u32(%s.val[2],%s.val[3]);" % (S,T,T))

    

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
  uint32x4_t T, D0, D1, D2, D3;
  uint32x4x4_t A, B, C, D, AA, BB, CC, DD;

  A = vld4q_dup_u32((void *)(in));
  B = vld4q_dup_u32((void *)(in+16));
  C = vld4q_dup_u32((void *)(in+32));
  D.val[0]=vld1q_u32((void *)in+48);
  D.val[1]=vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D.val[0])));
  D.val[2]=vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D.val[1])));
  D.val[3]=vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D.val[2])));
  T       =vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D.val[3])));
  vst1q_u32((void *)(out+ 48), D.val[0]);
  vst1q_u32((void *)(out+112), D.val[1]);
  vst1q_u32((void *)(out+176), D.val[2]);
  vst1q_u32((void *)(out+240), D.val[3]);
  vst1q_u32((void *)(in+48),T);
''')

for i in transpose_u32("D","DD") : print(i)

print("  for (i=ROUNDS/2; i>0; i--) {")

g = [[testgen(i,j) for j in range(2)] for i in range(4)]
for k in range(2) :
    for i in range(14) :
        for j in range(4) :
            print(next(g[j][k]))
    print("")

print("  }")

for k in range(3) :
    print("  %s = vld4q_dup_u32((void *)(in+%d));" % (chr(65+k)*2,16*k))
for j in range(4) :
    for k in range(3) :
        print("  %s.val[%d] = vaddq_u32(%s.val[%d], %s.val[%d]);" % (chr(65+k),j,chr(65+k),j,chr(65+k)*2,j))

g = [transpose_u32("A","AA"),transpose_u32("B","BB"),transpose_u32("C","CC")]
        
for i in range(8) :
    for k in range(3) : print(next(g[k]))
for j in range(4) :
    for k in range(3) :
        print("  vst1q_u32((void *)(out+%3d), %s.val[%d]);" % (64*j+16*k,chr(65+k),j)) 

for i in transpose_u32("D","DD") : print(i)       
for j in range(4) : print("  D%d = vld1q_u32((void *)(out+%3d));" %(j,48+64*j))
for j in range(4) : print("  D%d = vaddq_u32(D%d, D.val[%d]);" % (j,j,j))
for j in range(4) : print("  vst1q_u32((void *)(out+%3d), D%d);" % (48+64*j,j))
print('''}

int crypto_stream_chacha20(unsigned char *c,unsigned long long clen, const unsigned char *n, const unsigned char *k)
{
  unsigned char block3[256] __attribute__((aligned(16)));
  unsigned char setup[64] __attribute__((aligned(16)));
  uint64_t C = 0;
  int i;
  
  if (!clen) return 0;

  vst1q_u32((void *)setup,  vld1q_u32((void *)sigma));
  vst1q_u32((void *)(setup+16), vld1q_u32((void *)k));
  vst1q_u32((void *)(setup+32), vld1q_u32((void *)(k+16)));
  *((uint64_t *)(setup+48)) = C;
  *((uint64_t *)(setup+56)) = *((uint64_t *)(n));

  while(clen >= 256) {
    crypto_core_chacha20_neon(c,setup);
    clen -= 256;
    c += 256;
  }

  if (clen) {
    crypto_core_chacha20_neon(block3,setup);
    for (i = 0;i < clen;++i) c[i] = block3[i];
  }
  return 0;
}''')


      
