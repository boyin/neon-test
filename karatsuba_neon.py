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
NS = int(floor(log(N)/log(2.0))) - 1
N3 = round((R<<NS)/N)
N2 = round(R**2/N/2)
MC = 2**(int(floor(log(R**2/(N/2.0)**2/2)/log(2.0))))

def karatsuba_neon_c (B) :
    print('''
#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>
#include <stdio.h>


int16x8_t reduce16x8 (int16x8_t a) {
  const int16x4_t N = {%d,%d,0,0};
  int16x8_t a1 = vqdmulhq_lane_s16(a, N, 1);
  a1 = vrshrq_n_s16(a1, %d);  
  a = vmlsq_lane_s16(a, a1, N, 0);
  return(a);
}



extern void polymul_sb(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n);

void polymul_ka(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n){

  int i, j, k, l, ll, L, _K, _2pK, b;
  int16x8_t t0, t1, t2;
''' % (N, N3, NS+1))

    print("  assert(n %% %d == 0); _2pK = (n / %d); L=n;" % (B,B))
    print('''  for (_K = 0; (1<<_K) < _2pK; _K++) L+=(L>>1);
  assert(_2pK == (1 << _K)); // _K = number of layers
  int16_t ff[L-n], gg[L-n], hh[2*L-2*n];
    
  for (l=ll=n, k=0; k<_K; k++){
    ll>>=1;
    for (i=0; i<n; i+=2*ll) {
#ifdef DEBUG
      printf("f%d_%d=",i,2*ll);for(j=0;j<2*ll;j++) printf("%+d*x^%d",f[i+j],j); printf("\\n");
      printf("g%d_%d=",i,2*ll);for(j=0;j<2*ll;j++) printf("%+d*x^%d",g[i+j],j); printf("\\n");
#endif
      for (j=0; j<ll; j+=8) {
	t0 = vaddq_s16(*(int16x8_t *)(f+i+j),*(int16x8_t *)(f+i+j+ll));
	t1 = vaddq_s16(*(int16x8_t *)(g+i+j),*(int16x8_t *)(g+i+j+ll));
	*(int16x8_t *)(ff+l-n+i/2+j) = reduce16x8(t0);
	*(int16x8_t *)(gg+l-n+i/2+j) = reduce16x8(t1);
      }
    }
    for (i=0; i<l-n; i+=2*ll) {
#ifdef DEBUG
      printf("f%d_%d=",i+n,2*ll);for(j=0;j<2*ll;j++)printf("%+d*x^%d",ff[i+j],j); printf("\\n");
      printf("g%d_%d=",i+n,2*ll);for(j=0;j<2*ll;j++)printf("%+d*x^%d",gg[i+j],j); printf("\\n");
#endif
      for (j=0; j<ll; j+=8) {
        t0 = vaddq_s16(*(int16x8_t *)(ff+i+j),*(int16x8_t *)(ff+i+j+ll));
        t1 = vaddq_s16(*(int16x8_t *)(gg+i+j),*(int16x8_t *)(gg+i+j+ll));
        *(int16x8_t *)(ff+l+i/2-n/2+j) = reduce16x8(t0);
        *(int16x8_t *)(gg+l+i/2-n/2+j) = reduce16x8(t1);
      }
    }
    l += (l>>1);
  }''')
    print('''  for (i=0; i<n; i+=%d) {
#ifdef DEBUG
    printf("f%%d_%d=",i);for(j=0;j<%d;j++)printf("%%+d*x^%%d",f[i+j],j); printf("\\n");
    printf("g%%d_%d=",i);for(j=0;j<%d;j++)printf("%%+d*x^%%d",g[i+j],j); printf("\\n");
#endif
    polymul_sb(h+2*i,f+i,g+i,%d);
#ifdef DEBUG
    printf("h%%d_%d=",i);for(j=0;j<2*%d;j++)printf("%%+d*x^%%d",h[2*i+j],j); printf("\\n");
#endif
  }   
  for (i=0; i<L-n; i+=%d) {
#ifdef DEBUG
    printf("f%%d_%d=",i+n);for(j=0;j<%d;j++)printf("%%+d*x^%%d",ff[i+j],j); printf("\\n");
    printf("g%%d_%d=",i+n);for(j=0;j<%d;j++)printf("%%+d*x^%%d",gg[i+j],j); printf("\\n");
#endif
    polymul_sb(hh+2*i,ff+i,gg+i,%d);
#ifdef DEBUG
    printf("h%%d_%d=",i+n);for(j=0;j<2*%d;j++)printf("%%+d*x^%%d",hh[2*i+j],j); printf("\\n");
#endif
  }   
  for (k=0; k<_K; k++) {
    l /= 3; l<<=1;
    b = (%d<<k);''' % ((B,) * 17))
    print('''    for (i=0; i<2*n; i+=4*b) {
      for (j=0; j<b; j+=8) {
        t0 = vsubq_s16(*(int16x8_t *)(h+i+j+b),*(int16x8_t *)(h+i+j+2*b));
        t1 = vsubq_s16(t0,*(int16x8_t *)(h+i+j));
        t2 = vaddq_s16(t0,*(int16x8_t *)(h+i+j+3*b));
        t1 = reduce16x8(vaddq_s16(t1,*(int16x8_t *)(hh+2*l-2*n+i/2+j)));
        t2 = reduce16x8(vsubq_s16(*(int16x8_t *)(hh+2*l-2*n+i/2+j+b),t2));
        *(int16x8_t *)(h+i+j+b) = t1;
        *(int16x8_t *)(h+i+j+2*b) = t2;
      }
#ifdef DEBUG
      printf("h%d_%d=",i/2,2*b);for(j=0;j<4*b;j++)printf("%+d*x^%d",h[i+j],j); printf("\\n");
#endif
    }
    for (i=0; i<2*l-2*n; i+=4*b) {
      for (j=0; j<b; j+=8) {
        t0 = vsubq_s16(*(int16x8_t *)(hh+i+j+b),*(int16x8_t *)(hh+i+j+2*b));
        t1 = vsubq_s16(t0,*(int16x8_t *)(hh+i+j));
        t2 = vaddq_s16(t0,*(int16x8_t *)(hh+i+j+3*b));
        t1 = reduce16x8(vaddq_s16(t1,*(int16x8_t *)(hh+2*l-n+i/2+j)));
        t2 = reduce16x8(vsubq_s16(*(int16x8_t *)(hh+2*l-n+i/2+j+b),t2));
        *(int16x8_t *)(hh+i+j+b) = t1;
        *(int16x8_t *)(hh+i+j+2*b) = t2;
      }
#ifdef DEBUG
      printf("h%d_%d=",i/2+n,2*b);for(j=0;j<4*b;j++)printf("%+d*x^%d",hh[i+j],j); printf("\\n");
#endif
    }
  }   

}''')



    



    
try : B = int(sys.argv[1])
except : B = 32    
    
karatsuba_neon_c(B) 
