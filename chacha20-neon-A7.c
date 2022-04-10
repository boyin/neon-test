#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <arm_neon.h>

static const __attribute__((aligned(16))) uint8_t sigma[16] = "expand 32-byte k";
static const __attribute__((aligned(16))) uint64x2_t T1 = {1LL, 0LL};

#define ROUNDS 20

static int crypto_core_chacha20_neon(unsigned char *out, unsigned char *in){
  uint32x4_t A0, B0, C0, D0, A1, B1, C1, D1, A2, B2, C2, D2, T, E0, E1, E2;
  int i;
  
  A0 = A1 = A2 = vld1q_u32((void *)in);
  B0 = B1 = B2 = vld1q_u32((void *)(in+16));
  C0 = C1 = C2 = vld1q_u32((void *)(in+32));
  D0 = vld1q_u32((void *)in+48);
  D1 = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D0)));
  D2 = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D1)));
  T  = vreinterpretq_u32_u64(vaddq_u64(T1,vreinterpretq_u64_u32(D2)));
  vst1q_u32((void *)(out+48), D0);
  vst1q_u32((void *)(out+112), D1);
  vst1q_u32((void *)(out+176), D2);
  vst1q_u32((void *)(in+48),T);

  for (i=ROUNDS/2; i>0; i--) {
  
    A0 = vaddq_u32(A0, B0); A1 = vaddq_u32(A1, B1); A2 = vaddq_u32(A2, B2);
    D0 = veorq_u32(D0, A0); D1 = veorq_u32(D1, A1); D2 = veorq_u32(D2, A2);
    E0 = vshlq_n_u32(D0, 16);
    E1 = vshlq_n_u32(D1, 16);
    E2 = vshlq_n_u32(D2, 16); 
    D0 = vsriq_n_u32(E0, D0, 16);
    D1 = vsriq_n_u32(E1, D1, 16);
    D2 = vsriq_n_u32(E2, D2, 16);
    
    C0 = vaddq_u32(C0, D0); C1 = vaddq_u32(C1, D1); C2 = vaddq_u32(C2, D2);
    B0 = veorq_u32(B0, C0); B1 = veorq_u32(B1, C1); B2 = veorq_u32(B2, C2);
    E0 = vshlq_n_u32(B0, 12);
    E1 = vshlq_n_u32(B1, 12);
    E2 = vshlq_n_u32(B2, 12); 
    B0 = vsriq_n_u32(E0, B0, 20);
    B1 = vsriq_n_u32(E1, B1, 20);
    B2 = vsriq_n_u32(E2, B2, 20);
    
    A0 = vaddq_u32(A0, B0); A1 = vaddq_u32(A1, B1); A2 = vaddq_u32(A2, B2);
    D0 = veorq_u32(D0, A0); D1 = veorq_u32(D1, A1); D2 = veorq_u32(D2, A2);
    E0 = vshlq_n_u32(D0, 8);
    E1 = vshlq_n_u32(D1, 8);
    E2 = vshlq_n_u32(D2, 8); 
    D0 = vsriq_n_u32(E0, D0, 24);
    D1 = vsriq_n_u32(E1, D1, 24);
    D2 = vsriq_n_u32(E2, D2, 24);
    
    C0 = vaddq_u32(C0, D0); C1 = vaddq_u32(C1, D1); C2 = vaddq_u32(C2, D2);
    B0 = veorq_u32(B0, C0); B1 = veorq_u32(B1, C1); B2 = veorq_u32(B2, C2);
    E0 = vshlq_n_u32(B0, 7);
    E1 = vshlq_n_u32(B1, 7);
    E2 = vshlq_n_u32(B2, 7); 
    B0 = vsriq_n_u32(E0, B0, 25);
    B1 = vsriq_n_u32(E1, B1, 25);
    B2 = vsriq_n_u32(E2, B2, 25);

    B0 = vextq_u32(B0, B0, 1);
    B1 = vextq_u32(B1, B1, 1);
    B2 = vextq_u32(B2, B2, 1);
    C0 = vextq_u32(C0, C0, 2);
    C1 = vextq_u32(C1, C1, 2);
    C2 = vextq_u32(C2, C2, 2);
    D0 = vextq_u32(D0, D0, 3);
    D1 = vextq_u32(D1, D1, 3);
    D2 = vextq_u32(D2, D2, 3);
  
    A0 = vaddq_u32(A0, B0); A1 = vaddq_u32(A1, B1); A2 = vaddq_u32(A2, B2);
    D0 = veorq_u32(D0, A0); D1 = veorq_u32(D1, A1); D2 = veorq_u32(D2, A2);
    E0 = vshlq_n_u32(D0, 16);
    E1 = vshlq_n_u32(D1, 16);
    E2 = vshlq_n_u32(D2, 16); 
    D0 = vsriq_n_u32(E0, D0, 16);
    D1 = vsriq_n_u32(E1, D1, 16);
    D2 = vsriq_n_u32(E2, D2, 16);
    
    C0 = vaddq_u32(C0, D0); C1 = vaddq_u32(C1, D1); C2 = vaddq_u32(C2, D2);
    B0 = veorq_u32(B0, C0); B1 = veorq_u32(B1, C1); B2 = veorq_u32(B2, C2);
    E0 = vshlq_n_u32(B0, 12);
    E1 = vshlq_n_u32(B1, 12);
    E2 = vshlq_n_u32(B2, 12); 
    B0 = vsriq_n_u32(E0, B0, 20);
    B1 = vsriq_n_u32(E1, B1, 20);
    B2 = vsriq_n_u32(E2, B2, 20);
    
    A0 = vaddq_u32(A0, B0); A1 = vaddq_u32(A1, B1); A2 = vaddq_u32(A2, B2);
    D0 = veorq_u32(D0, A0); D1 = veorq_u32(D1, A1); D2 = veorq_u32(D2, A2);
    E0 = vshlq_n_u32(D0, 8);
    E1 = vshlq_n_u32(D1, 8);
    E2 = vshlq_n_u32(D2, 8); 
    D0 = vsriq_n_u32(E0, D0, 24);
    D1 = vsriq_n_u32(E1, D1, 24);
    D2 = vsriq_n_u32(E2, D2, 24);
    
    C0 = vaddq_u32(C0, D0); C1 = vaddq_u32(C1, D1); C2 = vaddq_u32(C2, D2);
    B0 = veorq_u32(B0, C0); B1 = veorq_u32(B1, C1); B2 = veorq_u32(B2, C2);
    E0 = vshlq_n_u32(B0, 7);
    E1 = vshlq_n_u32(B1, 7);
    E2 = vshlq_n_u32(B2, 7); 
    B0 = vsriq_n_u32(E0, B0, 25);
    B1 = vsriq_n_u32(E1, B1, 25);
    B2 = vsriq_n_u32(E2, B2, 25);

    B0 = vextq_u32(B0, B0, 3);
    B1 = vextq_u32(B1, B1, 3);
    B2 = vextq_u32(B2, B2, 3);
    C0 = vextq_u32(C0, C0, 2);
    C1 = vextq_u32(C1, C1, 2);
    C2 = vextq_u32(C2, C2, 2);
    D0 = vextq_u32(D0, D0, 1);
    D1 = vextq_u32(D1, D1, 1);
    D2 = vextq_u32(D2, D2, 1);
  }
  E0 = vld1q_u32((void *)in);
  E1 = vld1q_u32((void *)(in+16));
  E2 = vld1q_u32((void *)(in+32));
  A0 = vaddq_u32(A0,E0); A1 = vaddq_u32(A1,E0); A2 = vaddq_u32(A2,E0);
  B0 = vaddq_u32(B0,E1); B1 = vaddq_u32(B1,E1); B2 = vaddq_u32(B2,E1);
  C0 = vaddq_u32(C0,E2); C1 = vaddq_u32(C1,E2); C2 = vaddq_u32(C2,E2);
  E0 = vld1q_u32((void *)(out+48));
  E1 = vld1q_u32((void *)(out+112));
  E2 = vld1q_u32((void *)(out+176));
  D0 = vaddq_u32(D0,E0);
  D1 = vaddq_u32(D1,E1);
  D2 = vaddq_u32(D2,E2);

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
  
}

int crypto_stream_chacha20(unsigned char *c,unsigned long long clen, const unsigned char *n, const unsigned char *k)
{
  unsigned char block3[192] __attribute__((aligned(16)));
  unsigned char setup[64] __attribute__((aligned(16)));
  uint64_t C = 0;
  int i;
  
  if (!clen) return 0;

  vst1q_u32((void *)setup,  vld1q_u32((void *)sigma));
  vst1q_u32((void *)(setup+16), vld1q_u32((void *)k));
  vst1q_u32((void *)(setup+32), vld1q_u32((void *)(k+16)));
  *((uint64_t *)(setup+48)) = C;
  *((uint64_t *)(setup+52)) = C;
  *((uint64_t *)(setup+56)) = *((uint64_t *)(n));
  *((uint64_t *)(setup+60)) = *((uint64_t *)(n+4));

  while(clen >= 192) {
    crypto_core_chacha20_neon(c,setup);
    clen -= 192;
    c += 192;
  }

  if (clen) {
    crypto_core_chacha20_neon(block3,setup);
    for (i = 0;i < clen;++i) c[i] = block3[i];
  }
  return 0;
}
