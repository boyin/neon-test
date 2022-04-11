#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <arm_neon.h>

static const __attribute__((aligned(16))) uint8_t sigma[16] = "expand 32-byte k";
static const __attribute__((aligned(16))) uint64x2_t T1 = {1LL, 0LL};

#define ROUNDS 20

static inline int crypto_core_chacha20_neon(unsigned char *out, unsigned char *in){
  uint32x4_t T, E0, E1, E2, E3;
  register uint32x4_t A0, B0, C0, D0, A1, B1, C1, D1;
  register uint32x4_t A2, B2, C2, D2, A3, B3, C3, D3;
  register uint32x4_t BB0, DD0, BB1, DD1, BB2, DD2, BB3, DD3;
  int i;
  
  A0 = A1 = A2 = A3 = vld1q_u32((void *)in);
  B0 = B1 = B2 = B3 = vld1q_u32((void *)(in+16));
  C0 = C1 = C2 = C3 = vld1q_u32((void *)(in+32));
  D0 = vld1q_u32((void *)in+48);
  D1 = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D0)));
  D2 = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D1)));
  D3 = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D2)));
  T  = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D3)));
  vst1q_u32((void *)(out+ 48), D0);
  vst1q_u32((void *)(out+112), D1);
  vst1q_u32((void *)(out+176), D2);
  vst1q_u32((void *)(out+240), D3);
  vst1q_u32((void *)(in+48),T);

  for (i=ROUNDS/2; i>0; i--) {
  
    A0 = vaddq_u32(A0, B0);
    A1 = vaddq_u32(A1, B1);
    A2 = vaddq_u32(A2, B2);
    A3 = vaddq_u32(A3, B3);
    D0 = veorq_u32(D0, A0);
    D1 = veorq_u32(D1, A1);
    D2 = veorq_u32(D2, A2);
    D3 = veorq_u32(D3, A3);
    DD0 = vshlq_n_u32(D0, 16);
    DD1 = vshlq_n_u32(D1, 16);
    DD2 = vshlq_n_u32(D2, 16); 
    DD3 = vshlq_n_u32(D3, 16); 
    DD0 = vsraq_n_u32(DD0, D0, 16);
    DD1 = vsraq_n_u32(DD1, D1, 16);
    DD2 = vsraq_n_u32(DD2, D2, 16);
    DD3 = vsraq_n_u32(DD3, D3, 16);
    
    C0 = vaddq_u32(C0, DD0);
    C1 = vaddq_u32(C1, DD1);
    C2 = vaddq_u32(C2, DD2);
    C3 = vaddq_u32(C3, DD3);
    B0 = veorq_u32(B0, C0);
    B1 = veorq_u32(B1, C1);
    B2 = veorq_u32(B2, C2);
    B3 = veorq_u32(B3, C3);
    BB0 = vshlq_n_u32(B0, 12);
    BB1 = vshlq_n_u32(B1, 12);
    BB2 = vshlq_n_u32(B2, 12); 
    BB3 = vshlq_n_u32(B3, 12); 
    BB0 = vsraq_n_u32(BB0, B0, 20);
    BB1 = vsraq_n_u32(BB1, B1, 20);
    BB2 = vsraq_n_u32(BB2, B2, 20);
    BB3 = vsraq_n_u32(BB3, B3, 20);
    
    A0 = vaddq_u32(A0, BB0);
    A1 = vaddq_u32(A1, BB1);
    A2 = vaddq_u32(A2, BB2);
    A3 = vaddq_u32(A3, BB3);
    DD0 = veorq_u32(DD0, A0);
    DD1 = veorq_u32(DD1, A1);
    DD2 = veorq_u32(DD2, A2);
    DD3 = veorq_u32(DD3, A3);
    D0 = vshlq_n_u32(DD0, 8);
    D1 = vshlq_n_u32(DD1, 8);
    D2 = vshlq_n_u32(DD2, 8); 
    D3 = vshlq_n_u32(DD3, 8); 
    D0 = vsraq_n_u32(D0, DD0, 24);
    D1 = vsraq_n_u32(D1, DD1, 24);
    D2 = vsraq_n_u32(D2, DD2, 24);
    D3 = vsraq_n_u32(D3, DD3, 24);
    
    C0 = vaddq_u32(C0, D0);
    C1 = vaddq_u32(C1, D1);
    C2 = vaddq_u32(C2, D2);
    C3 = vaddq_u32(C3, D3);
    BB0 = veorq_u32(BB0, C0);
    BB1 = veorq_u32(BB1, C1);
    BB2 = veorq_u32(BB2, C2);
    BB3 = veorq_u32(BB3, C3);
    B0 = vshlq_n_u32(BB0, 7);
    B1 = vshlq_n_u32(BB1, 7);
    B2 = vshlq_n_u32(BB2, 7); 
    B3 = vshlq_n_u32(BB3, 7); 
    B0 = vsraq_n_u32(B0, BB0, 25);
    B1 = vsraq_n_u32(B1, BB1, 25);
    B2 = vsraq_n_u32(B2, BB2, 25);
    B3 = vsraq_n_u32(B3, BB3, 25);

    B0 = vextq_u32(B0, B0, 1);
    B1 = vextq_u32(B1, B1, 1);
    B2 = vextq_u32(B2, B2, 1);
    B3 = vextq_u32(B3, B3, 1);
    C0 = vextq_u32(C0, C0, 2);
    C1 = vextq_u32(C1, C1, 2);
    C2 = vextq_u32(C2, C2, 2);
    C3 = vextq_u32(C3, C3, 2);
    D0 = vextq_u32(D0, D0, 3);
    D1 = vextq_u32(D1, D1, 3);
    D2 = vextq_u32(D2, D2, 3);
    D3 = vextq_u32(D3, D3, 3);
  
    A0 = vaddq_u32(A0, B0);
    A1 = vaddq_u32(A1, B1);
    A2 = vaddq_u32(A2, B2);
    A3 = vaddq_u32(A3, B3);
    D0 = veorq_u32(D0, A0);
    D1 = veorq_u32(D1, A1);
    D2 = veorq_u32(D2, A2);
    D3 = veorq_u32(D3, A3);
    DD0 = vshlq_n_u32(D0, 16);
    DD1 = vshlq_n_u32(D1, 16);
    DD2 = vshlq_n_u32(D2, 16); 
    DD3 = vshlq_n_u32(D3, 16); 
    DD0 = vsraq_n_u32(DD0, D0, 16);
    DD1 = vsraq_n_u32(DD1, D1, 16);
    DD2 = vsraq_n_u32(DD2, D2, 16);
    DD3 = vsraq_n_u32(DD3, D3, 16);
    
    C0 = vaddq_u32(C0, DD0);
    C1 = vaddq_u32(C1, DD1);
    C2 = vaddq_u32(C2, DD2);
    C3 = vaddq_u32(C3, DD3);
    B0 = veorq_u32(B0, C0);
    B1 = veorq_u32(B1, C1);
    B2 = veorq_u32(B2, C2);
    B3 = veorq_u32(B3, C3);
    BB0 = vshlq_n_u32(B0, 12);
    BB1 = vshlq_n_u32(B1, 12);
    BB2 = vshlq_n_u32(B2, 12); 
    BB3 = vshlq_n_u32(B3, 12); 
    BB0 = vsraq_n_u32(BB0, B0, 20);
    BB1 = vsraq_n_u32(BB1, B1, 20);
    BB2 = vsraq_n_u32(BB2, B2, 20);
    BB3 = vsraq_n_u32(BB3, B3, 20);
    
    A0 = vaddq_u32(A0, BB0);
    A1 = vaddq_u32(A1, BB1);
    A2 = vaddq_u32(A2, BB2);
    A3 = vaddq_u32(A3, BB3);
    DD0 = veorq_u32(DD0, A0);
    DD1 = veorq_u32(DD1, A1);
    DD2 = veorq_u32(DD2, A2);
    DD3 = veorq_u32(DD3, A3);
    D0 = vshlq_n_u32(DD0, 8);
    D1 = vshlq_n_u32(DD1, 8);
    D2 = vshlq_n_u32(DD2, 8); 
    D3 = vshlq_n_u32(DD3, 8); 
    D0 = vsraq_n_u32(D0, DD0, 24);
    D1 = vsraq_n_u32(D1, DD1, 24);
    D2 = vsraq_n_u32(D2, DD2, 24);
    D3 = vsraq_n_u32(D3, DD3, 24);
    
    C0 = vaddq_u32(C0, D0);
    C1 = vaddq_u32(C1, D1);
    C2 = vaddq_u32(C2, D2);
    C3 = vaddq_u32(C3, D3);
    BB0 = veorq_u32(BB0, C0);
    BB1 = veorq_u32(BB1, C1);
    BB2 = veorq_u32(BB2, C2);
    BB3 = veorq_u32(BB3, C3);
    B0 = vshlq_n_u32(BB0, 7);
    B1 = vshlq_n_u32(BB1, 7);
    B2 = vshlq_n_u32(BB2, 7); 
    B3 = vshlq_n_u32(BB3, 7); 
    B0 = vsraq_n_u32(B0, BB0, 25);
    B1 = vsraq_n_u32(B1, BB1, 25);
    B2 = vsraq_n_u32(B2, BB2, 25);
    B3 = vsraq_n_u32(B3, BB3, 25);

    B0 = vextq_u32(B0, B0, 3);
    B1 = vextq_u32(B1, B1, 3);
    B2 = vextq_u32(B2, B2, 3);
    B3 = vextq_u32(B3, B3, 3);
    C0 = vextq_u32(C0, C0, 2);
    C1 = vextq_u32(C1, C1, 2);
    C2 = vextq_u32(C2, C2, 2);
    C3 = vextq_u32(C3, C3, 2);
    D0 = vextq_u32(D0, D0, 1);
    D1 = vextq_u32(D1, D1, 1);
    D2 = vextq_u32(D2, D2, 1);
    D3 = vextq_u32(D3, D3, 1);
  }
  E0 = vld1q_u32((void *)in);
  E1 = vld1q_u32((void *)(in+16));
  E2 = vld1q_u32((void *)(in+32));
  A0 = vaddq_u32(A0,E0); A1 = vaddq_u32(A1,E0);
  A2 = vaddq_u32(A2,E0); A3 = vaddq_u32(A3,E0);
  B0 = vaddq_u32(B0,E1); B1 = vaddq_u32(B1,E1);
  B2 = vaddq_u32(B2,E1); B3 = vaddq_u32(B3,E1);
  C0 = vaddq_u32(C0,E2); C1 = vaddq_u32(C1,E2);
  C2 = vaddq_u32(C2,E2); C3 = vaddq_u32(C3,E2);
  E0 = vld1q_u32((void *)(out+48));
  E1 = vld1q_u32((void *)(out+112));
  E2 = vld1q_u32((void *)(out+176));
  E3 = vld1q_u32((void *)(out+240));
  D0 = vaddq_u32(D0,E0);
  D1 = vaddq_u32(D1,E1);
  D2 = vaddq_u32(D2,E2);
  D3 = vaddq_u32(D3,E3);

  vst1q_u32((void *)(out+0)  , A0);
  vst1q_u32((void *)(out+16) , B0);
  vst1q_u32((void *)(out+32) , C0);
  vst1q_u32((void *)(out+48) , D0);
  vst1q_u32((void *)(out+64) , A1);
  vst1q_u32((void *)(out+80) , B1);
  vst1q_u32((void *)(out+96) , C1);
  vst1q_u32((void *)(out+112), D1);
  vst1q_u32((void *)(out+128), A2);
  vst1q_u32((void *)(out+144), B2);
  vst1q_u32((void *)(out+160), C2);
  vst1q_u32((void *)(out+176), D2);
  vst1q_u32((void *)(out+192), A3);
  vst1q_u32((void *)(out+208), B3);
  vst1q_u32((void *)(out+224), C3);
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
