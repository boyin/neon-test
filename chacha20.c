/* Based on the public domain implemntation in
 * crypto_stream/chacha20/e/ref from http://bench.cr.yp.to/supercop.html
 * by Daniel J. Bernstein */

#include <stdint.h>

#define ROUNDS 20

typedef uint32_t uint32;

static uint32 load_littleendian(const unsigned char *x)
{
  return
    //      (uint32) (x[0])    \
    //  | (((uint32) (x[1])) << 8)		\
    //  | (((uint32) (x[2])) << 16)		\
    //  | (((uint32) (x[3])) << 24);
    //
    (*((uint32 *)x));
}

static void store_littleendian(unsigned char *x,uint32 u)
{
  *((uint32 *)x) = u;
  // x[0] = u; u >>= 8;
  // x[1] = u; u >>= 8;
  // x[2] = u; u >>= 8;
  // x[3] = u;
}

static uint32 rotate(uint32 a, int d)
{
  uint32 t;
  t = a >> (32-d);
  a <<= d;
  return a | t;
}

static uint32 ror(uint32 a, int d)
{
  uint32 t;
  t = a >> d;
  a <<= (32-d);
  return a | t;
}

#define quarterround_asm(a,b,c,d)				\
  __asm__ volatile ("add %0, %0, %1, ROR #25\n\t"		\
		    "eor %3, %0, %3, ROR #24\n\t"		\
		    "add %2, %2, %3, ROR #16\n\t"		\
		    "eor %1, %2, %1, ROR #25\n\t"		\
		    "add %0, %0, %1, ROR #20\n\t"		\
		    "eor %3, %0, %3, ROR #16\n\t"		\
		    "add %2, %2, %3, ROR #24\n\t"		\
		    "eor %1, %2, %1, ROR #20\n\t"		\
		    :"+r"(a),"+r"(b),"+r"(c),"+r"(d)		\
		    );						\

#define quarterround_asm1(a,b,c,d)				\
  __asm__ volatile ("add %0, %0, %1\n\t"		\
		    "eor %3, %0, %3\n\t"		\
		    "add %2, %2, %3, ROR #16\n\t"		\
		    "eor %1, %2, %1\n\t"		\
		    "add %0, %0, %1, ROR #20\n\t"		\
		    "eor %3, %0, %3, ROR #16\n\t"		\
		    "add %2, %2, %3, ROR #24\n\t"		\
		    "eor %1, %2, %1, ROR #20\n\t"		\
		    :"+r"(a),"+r"(b),"+r"(c),"+r"(d)		\
		    );						\



static void quarterround(uint32 *a, uint32 *b, uint32 *c, uint32 *d)
{
  // *a = *a + *b;
  // *d = *d ^ *a;
  // *c = *c + ror(*d, 16);
  // *b = *b ^ *c;
  // *a = *a + ror(*b, 20);
  // *d = *a ^ ror(*d, 16);
  // *c = *c + ror(*d, 24);
  // *b = *c ^ ror(*b, 20);
  // *b = ror(*b, 25);
  // *d = ror(*d, 24);
  *a = *a + *b;
  *d = *d ^ *a;
  *d = rotate(*d, 16);
  
  *c = *c + *d;
  *b = *b ^ *c;
  *b = rotate(*b, 12);
  
  *a = *a + *b;
  *d = *d ^ *a;
  *d = rotate(*d, 8);
  
  *c = *c + *d;
  *b = *b ^ *c;
  *b = rotate(*b, 7);
}

static int crypto_core_chacha20(
        unsigned char *out,
  const unsigned char *in,
  const unsigned char *k,
  const unsigned char *c
)
{
  uint32 x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
  uint32 j0, j1, j2, j3, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14, j15;
  int i;

  j0  = x0  = load_littleendian(c +  0);
  j1  = x1  = load_littleendian(c +  4);
  j2  = x2  = load_littleendian(c +  8);
  j3  = x3  = load_littleendian(c + 12);
  j4  = x4  = load_littleendian(k +  0);
  j5  = x5  = load_littleendian(k +  4);
  j6  = x6  = load_littleendian(k +  8);
  j7  = x7  = load_littleendian(k + 12);
  j8  = x8  = load_littleendian(k + 16);
  j9  = x9  = load_littleendian(k + 20);
  j10 = x10 = load_littleendian(k + 24);
  j11 = x11 = load_littleendian(k + 28);
  j12 = x12 = load_littleendian(in+  8);
  j13 = x13 = load_littleendian(in+ 12);
  j14 = x14 = load_littleendian(in+  0);
  j15 = x15 = load_littleendian(in+  4);

#ifndef crypto_core_chacha20_asm
  for (i = ROUNDS;i > 0;i -= 2) {
    quarterround(&x0, &x4,& x8,&x12);
    quarterround(&x1, &x5,& x9,&x13);
    quarterround(&x2, &x6,&x10,&x14);
    quarterround(&x3, &x7,&x11,&x15);
    quarterround(&x0, &x5,&x10,&x15);
    quarterround(&x1, &x6,&x11,&x12);
    quarterround(&x2, &x7,& x8,&x13);
    quarterround(&x3, &x4,& x9,&x14);
  }
#else
  quarterround_asm1(x0, x4, x8,x12);
  quarterround_asm1(x1, x5, x9,x13);
  quarterround_asm1(x2, x6,x10,x14);
  quarterround_asm1(x3, x7,x11,x15);
  quarterround_asm(x0, x5,x10,x15);
  quarterround_asm(x1, x6,x11,x12);
  quarterround_asm(x2, x7, x8,x13);
  quarterround_asm(x3, x4, x9,x14);
   
  for (i = ROUNDS-2;i > 0;i -= 2) {
    quarterround_asm(x0, x4, x8,x12);
    quarterround_asm(x1, x5, x9,x13);
    quarterround_asm(x2, x6,x10,x14);
    quarterround_asm(x3, x7,x11,x15);
    quarterround_asm(x0, x5,x10,x15);
    quarterround_asm(x1, x6,x11,x12);
    quarterround_asm(x2, x7, x8,x13);
    quarterround_asm(x3, x4, x9,x14);
  }
  x4 = ror(x4, 25);
  x5 = ror(x5, 25);
  x6 = ror(x6, 25);
  x7 = ror(x7, 25);
  x12= ror(x12, 24);
  x13= ror(x13, 24);
  x14= ror(x14, 24);
  x15= ror(x15, 24);
#endif

  
  x0 += j0;
  x1 += j1;
  x2 += j2;
  x3 += j3;
  x4 += j4;
  x5 += j5;
  x6 += j6;
  x7 += j7;
  x8 += j8;
  x9 += j9;
  x10 += j10;
  x11 += j11;
  x12 += j12;
  x13 += j13;
  x14 += j14;
  x15 += j15;

  store_littleendian(out + 0,x0);
  store_littleendian(out + 4,x1);
  store_littleendian(out + 8,x2);
  store_littleendian(out + 12,x3);
  store_littleendian(out + 16,x4);
  store_littleendian(out + 20,x5);
  store_littleendian(out + 24,x6);
  store_littleendian(out + 28,x7);
  store_littleendian(out + 32,x8);
  store_littleendian(out + 36,x9);
  store_littleendian(out + 40,x10);
  store_littleendian(out + 44,x11);
  store_littleendian(out + 48,x12);
  store_littleendian(out + 52,x13);
  store_littleendian(out + 56,x14);
  store_littleendian(out + 60,x15);

  return 0;
}
//#else
//$#include "chacha20_core.S"



static const unsigned char sigma[16] = "expand 32-byte k";

int crypto_stream_chacha20(unsigned char *c,unsigned long long clen, const unsigned char *n, const unsigned char *k)
{
  unsigned char in[16] __attribute__((aligned(4)));
  unsigned char block[64] __attribute__((aligned(4)));
  unsigned char kcopy[32] __attribute__((aligned(4)));
  unsigned long long i;
  unsigned int u;

  if (!clen) return 0;

  for (i = 0;i < 32;++i) kcopy[i] = k[i];
  for (i = 0;i < 8;++i) in[i] = n[i];
  for (i = 8;i < 16;++i) in[i] = 0;

  while (clen >= 64) {
    crypto_core_chacha20(c,in,kcopy,sigma);

    u = 1;
    for (i = 8;i < 16;++i) {
      u += (unsigned int) in[i];
      in[i] = u;
      u >>= 8;
    }

    clen -= 64;
    c += 64;
  }

  if (clen) {
    crypto_core_chacha20(block,in,kcopy,sigma);
    for (i = 0;i < clen;++i) c[i] = block[i];
  }
  return 0;
}
