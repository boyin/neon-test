
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

  DD.val[0] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(D.val[0]),vreinterpretq_u64_u32(D.val[2])));
  DD.val[2] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(D.val[0]),vreinterpretq_u64_u32(D.val[2])));
  DD.val[1] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(D.val[1]),vreinterpretq_u64_u32(D.val[3])));
  DD.val[3] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(D.val[1]),vreinterpretq_u64_u32(D.val[3])));
  D.val[0] = vtrn1q_u32(DD.val[0],DD.val[1]);
  D.val[1] = vtrn2q_u32(DD.val[0],DD.val[1]);
  D.val[2] = vtrn1q_u32(DD.val[2],DD.val[3]);
  D.val[3] = vtrn2q_u32(DD.val[2],DD.val[3]);
  for (i=ROUNDS/2; i>0; i--) {
    A.val[0] = vaddq_u32  ( A.val[0], B.val[0]);
    A.val[1] = vaddq_u32  ( A.val[1], B.val[1]);
    A.val[2] = vaddq_u32  ( A.val[2], B.val[2]);
    A.val[3] = vaddq_u32  ( A.val[3], B.val[3]);
    D.val[0] = veorq_u32  ( D.val[0], A.val[0]);
    D.val[1] = veorq_u32  ( D.val[1], A.val[1]);
    D.val[2] = veorq_u32  ( D.val[2], A.val[2]);
    D.val[3] = veorq_u32  ( D.val[3], A.val[3]);
DD.val[0] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[0])));
DD.val[1] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[1])));
DD.val[2] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[2])));
DD.val[3] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[3])));
    C.val[0] = vaddq_u32  ( C.val[0],DD.val[0]);
    C.val[1] = vaddq_u32  ( C.val[1],DD.val[1]);
    C.val[2] = vaddq_u32  ( C.val[2],DD.val[2]);
    C.val[3] = vaddq_u32  ( C.val[3],DD.val[3]);
    B.val[0] = veorq_u32  ( B.val[0], C.val[0]);
    B.val[1] = veorq_u32  ( B.val[1], C.val[1]);
    B.val[2] = veorq_u32  ( B.val[2], C.val[2]);
    B.val[3] = veorq_u32  ( B.val[3], C.val[3]);
    BB.val[0]= vshlq_n_u32( B.val[0], 12);
    BB.val[1]= vshlq_n_u32( B.val[1], 12);
    BB.val[2]= vshlq_n_u32( B.val[2], 12);
    BB.val[3]= vshlq_n_u32( B.val[3], 12);
    BB.val[0]= vsraq_n_u32(BB.val[0], B.val[0], 20);
    BB.val[1]= vsraq_n_u32(BB.val[1], B.val[1], 20);
    BB.val[2]= vsraq_n_u32(BB.val[2], B.val[2], 20);
    BB.val[3]= vsraq_n_u32(BB.val[3], B.val[3], 20);
    A.val[0] = vaddq_u32  ( A.val[0],BB.val[0]);
    A.val[1] = vaddq_u32  ( A.val[1],BB.val[1]);
    A.val[2] = vaddq_u32  ( A.val[2],BB.val[2]);
    A.val[3] = vaddq_u32  ( A.val[3],BB.val[3]);
    DD.val[0]= veorq_u32  (DD.val[0], A.val[0]);
    DD.val[1]= veorq_u32  (DD.val[1], A.val[1]);
    DD.val[2]= veorq_u32  (DD.val[2], A.val[2]);
    DD.val[3]= veorq_u32  (DD.val[3], A.val[3]);
    D.val[0] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[0]), IDX));
    D.val[1] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[1]), IDX));
    D.val[2] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[2]), IDX));
    D.val[3] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[3]), IDX));
    C.val[0] = vaddq_u32  ( C.val[0], D.val[0]);
    C.val[1] = vaddq_u32  ( C.val[1], D.val[1]);
    C.val[2] = vaddq_u32  ( C.val[2], D.val[2]);
    C.val[3] = vaddq_u32  ( C.val[3], D.val[3]);
    BB.val[0]= veorq_u32  (BB.val[0], C.val[0]);
    BB.val[1]= veorq_u32  (BB.val[1], C.val[1]);
    BB.val[2]= veorq_u32  (BB.val[2], C.val[2]);
    BB.val[3]= veorq_u32  (BB.val[3], C.val[3]);
    B.val[0] = vshlq_n_u32(BB.val[0], 7);
    B.val[1] = vshlq_n_u32(BB.val[1], 7);
    B.val[2] = vshlq_n_u32(BB.val[2], 7);
    B.val[3] = vshlq_n_u32(BB.val[3], 7);
    B.val[0] = vsraq_n_u32( B.val[0],BB.val[0], 25);
    B.val[1] = vsraq_n_u32( B.val[1],BB.val[1], 25);
    B.val[2] = vsraq_n_u32( B.val[2],BB.val[2], 25);
    B.val[3] = vsraq_n_u32( B.val[3],BB.val[3], 25);

    A.val[0] = vaddq_u32  ( A.val[0], B.val[1]);
    A.val[1] = vaddq_u32  ( A.val[1], B.val[2]);
    A.val[2] = vaddq_u32  ( A.val[2], B.val[3]);
    A.val[3] = vaddq_u32  ( A.val[3], B.val[0]);
    D.val[3] = veorq_u32  ( D.val[3], A.val[0]);
    D.val[0] = veorq_u32  ( D.val[0], A.val[1]);
    D.val[1] = veorq_u32  ( D.val[1], A.val[2]);
    D.val[2] = veorq_u32  ( D.val[2], A.val[3]);
DD.val[3] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[3])));
DD.val[0] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[0])));
DD.val[1] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[1])));
DD.val[2] = vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(D.val[2])));
    C.val[2] = vaddq_u32  ( C.val[2],DD.val[3]);
    C.val[3] = vaddq_u32  ( C.val[3],DD.val[0]);
    C.val[0] = vaddq_u32  ( C.val[0],DD.val[1]);
    C.val[1] = vaddq_u32  ( C.val[1],DD.val[2]);
    B.val[1] = veorq_u32  ( B.val[1], C.val[2]);
    B.val[2] = veorq_u32  ( B.val[2], C.val[3]);
    B.val[3] = veorq_u32  ( B.val[3], C.val[0]);
    B.val[0] = veorq_u32  ( B.val[0], C.val[1]);
    BB.val[1]= vshlq_n_u32( B.val[1], 12);
    BB.val[2]= vshlq_n_u32( B.val[2], 12);
    BB.val[3]= vshlq_n_u32( B.val[3], 12);
    BB.val[0]= vshlq_n_u32( B.val[0], 12);
    BB.val[1]= vsraq_n_u32(BB.val[1], B.val[1], 20);
    BB.val[2]= vsraq_n_u32(BB.val[2], B.val[2], 20);
    BB.val[3]= vsraq_n_u32(BB.val[3], B.val[3], 20);
    BB.val[0]= vsraq_n_u32(BB.val[0], B.val[0], 20);
    A.val[0] = vaddq_u32  ( A.val[0],BB.val[1]);
    A.val[1] = vaddq_u32  ( A.val[1],BB.val[2]);
    A.val[2] = vaddq_u32  ( A.val[2],BB.val[3]);
    A.val[3] = vaddq_u32  ( A.val[3],BB.val[0]);
    DD.val[3]= veorq_u32  (DD.val[3], A.val[0]);
    DD.val[0]= veorq_u32  (DD.val[0], A.val[1]);
    DD.val[1]= veorq_u32  (DD.val[1], A.val[2]);
    DD.val[2]= veorq_u32  (DD.val[2], A.val[3]);
    D.val[3] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[3]), IDX));
    D.val[0] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[0]), IDX));
    D.val[1] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[1]), IDX));
    D.val[2] = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(DD.val[2]), IDX));
    C.val[2] = vaddq_u32  ( C.val[2], D.val[3]);
    C.val[3] = vaddq_u32  ( C.val[3], D.val[0]);
    C.val[0] = vaddq_u32  ( C.val[0], D.val[1]);
    C.val[1] = vaddq_u32  ( C.val[1], D.val[2]);
    BB.val[1]= veorq_u32  (BB.val[1], C.val[2]);
    BB.val[2]= veorq_u32  (BB.val[2], C.val[3]);
    BB.val[3]= veorq_u32  (BB.val[3], C.val[0]);
    BB.val[0]= veorq_u32  (BB.val[0], C.val[1]);
    B.val[1] = vshlq_n_u32(BB.val[1], 7);
    B.val[2] = vshlq_n_u32(BB.val[2], 7);
    B.val[3] = vshlq_n_u32(BB.val[3], 7);
    B.val[0] = vshlq_n_u32(BB.val[0], 7);
    B.val[1] = vsraq_n_u32( B.val[1],BB.val[1], 25);
    B.val[2] = vsraq_n_u32( B.val[2],BB.val[2], 25);
    B.val[3] = vsraq_n_u32( B.val[3],BB.val[3], 25);
    B.val[0] = vsraq_n_u32( B.val[0],BB.val[0], 25);

  }
  AA = vld4q_dup_u32((void *)(in+0));
  BB = vld4q_dup_u32((void *)(in+16));
  CC = vld4q_dup_u32((void *)(in+32));
  A.val[0] = vaddq_u32(A.val[0], AA.val[0]);
  B.val[0] = vaddq_u32(B.val[0], BB.val[0]);
  C.val[0] = vaddq_u32(C.val[0], CC.val[0]);
  A.val[1] = vaddq_u32(A.val[1], AA.val[1]);
  B.val[1] = vaddq_u32(B.val[1], BB.val[1]);
  C.val[1] = vaddq_u32(C.val[1], CC.val[1]);
  A.val[2] = vaddq_u32(A.val[2], AA.val[2]);
  B.val[2] = vaddq_u32(B.val[2], BB.val[2]);
  C.val[2] = vaddq_u32(C.val[2], CC.val[2]);
  A.val[3] = vaddq_u32(A.val[3], AA.val[3]);
  B.val[3] = vaddq_u32(B.val[3], BB.val[3]);
  C.val[3] = vaddq_u32(C.val[3], CC.val[3]);
  AA.val[0] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(A.val[0]),vreinterpretq_u64_u32(A.val[2])));
  BB.val[0] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(B.val[0]),vreinterpretq_u64_u32(B.val[2])));
  CC.val[0] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(C.val[0]),vreinterpretq_u64_u32(C.val[2])));
  AA.val[2] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(A.val[0]),vreinterpretq_u64_u32(A.val[2])));
  BB.val[2] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(B.val[0]),vreinterpretq_u64_u32(B.val[2])));
  CC.val[2] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(C.val[0]),vreinterpretq_u64_u32(C.val[2])));
  AA.val[1] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(A.val[1]),vreinterpretq_u64_u32(A.val[3])));
  BB.val[1] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(B.val[1]),vreinterpretq_u64_u32(B.val[3])));
  CC.val[1] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(C.val[1]),vreinterpretq_u64_u32(C.val[3])));
  AA.val[3] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(A.val[1]),vreinterpretq_u64_u32(A.val[3])));
  BB.val[3] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(B.val[1]),vreinterpretq_u64_u32(B.val[3])));
  CC.val[3] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(C.val[1]),vreinterpretq_u64_u32(C.val[3])));
  A.val[0] = vtrn1q_u32(AA.val[0],AA.val[1]);
  B.val[0] = vtrn1q_u32(BB.val[0],BB.val[1]);
  C.val[0] = vtrn1q_u32(CC.val[0],CC.val[1]);
  A.val[1] = vtrn2q_u32(AA.val[0],AA.val[1]);
  B.val[1] = vtrn2q_u32(BB.val[0],BB.val[1]);
  C.val[1] = vtrn2q_u32(CC.val[0],CC.val[1]);
  A.val[2] = vtrn1q_u32(AA.val[2],AA.val[3]);
  B.val[2] = vtrn1q_u32(BB.val[2],BB.val[3]);
  C.val[2] = vtrn1q_u32(CC.val[2],CC.val[3]);
  A.val[3] = vtrn2q_u32(AA.val[2],AA.val[3]);
  B.val[3] = vtrn2q_u32(BB.val[2],BB.val[3]);
  C.val[3] = vtrn2q_u32(CC.val[2],CC.val[3]);
  vst1q_u32((void *)(out+  0), A.val[0]);
  vst1q_u32((void *)(out+ 16), B.val[0]);
  vst1q_u32((void *)(out+ 32), C.val[0]);
  vst1q_u32((void *)(out+ 64), A.val[1]);
  vst1q_u32((void *)(out+ 80), B.val[1]);
  vst1q_u32((void *)(out+ 96), C.val[1]);
  vst1q_u32((void *)(out+128), A.val[2]);
  vst1q_u32((void *)(out+144), B.val[2]);
  vst1q_u32((void *)(out+160), C.val[2]);
  vst1q_u32((void *)(out+192), A.val[3]);
  vst1q_u32((void *)(out+208), B.val[3]);
  vst1q_u32((void *)(out+224), C.val[3]);
  DD.val[0] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(D.val[0]),vreinterpretq_u64_u32(D.val[2])));
  DD.val[2] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(D.val[0]),vreinterpretq_u64_u32(D.val[2])));
  DD.val[1] = vreinterpretq_u32_u64(vtrn1q_u64(vreinterpretq_u64_u32(D.val[1]),vreinterpretq_u64_u32(D.val[3])));
  DD.val[3] = vreinterpretq_u32_u64(vtrn2q_u64(vreinterpretq_u64_u32(D.val[1]),vreinterpretq_u64_u32(D.val[3])));
  D.val[0] = vtrn1q_u32(DD.val[0],DD.val[1]);
  D.val[1] = vtrn2q_u32(DD.val[0],DD.val[1]);
  D.val[2] = vtrn1q_u32(DD.val[2],DD.val[3]);
  D.val[3] = vtrn2q_u32(DD.val[2],DD.val[3]);
  D0 = vld1q_u32((void *)(out+ 48));
  D1 = vld1q_u32((void *)(out+112));
  D2 = vld1q_u32((void *)(out+176));
  D3 = vld1q_u32((void *)(out+240));
  D0 = vaddq_u32(D0, D.val[0]);
  D1 = vaddq_u32(D1, D.val[1]);
  D2 = vaddq_u32(D2, D.val[2]);
  D3 = vaddq_u32(D3, D.val[3]);
  vst1q_u32((void *)(out+ 48), D0);
  vst1q_u32((void *)(out+112), D1);
  vst1q_u32((void *)(out+176), D2);
  vst1q_u32((void *)(out+240), D3);
}

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
}
