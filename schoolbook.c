#include <stdint.h>

void polymul_sb(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n) {

  int64_t a;

  int i, j;

  for (i = 0; i < n; ++i) {
    a = 0;
    for (j = 0; j <= i; ++j) {
        a += g[j] * f[i - j];
    }
    // for n <= 2560, |a| < 2^34
    a -= (4591LL) * ((a * 935519LL + 2147483648) >> 32);
    a -= (4591LL) * ((a * 935519LL + 2147483648) >> 32);
    h[i] = a;
  }
  for (; i < n * 2 - 1; ++i) {
    a = 0;
    for (j = i - n + 1; j < n; ++j) {
        a += g[j] * f[i - j];
    }
    a -= (4591LL) * ((a * 935519LL + 2147483648) >> 32);
    a -= (4591LL) * ((a * 935519LL + 2147483648) >> 32);
    h[i] = a;
  }
  h[n * 2 - 1] = 0;
}
  
