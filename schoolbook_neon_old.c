#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>
#include <stdio.h>

int32x4_t N = {4591,4591,4591,4591};
int32x4_t N1 = {935519,935519,935519,935519};
int32x4_t N2 = {467759,467759,467759,467759};

int32x4_t T;

//int32x4_t NN = vld1q_s32(N);
//int32x4_t NN2 = vld1q_s32(N2);

int32x4_t reduce32x4 (int32x4_t a) {
  int32x4_t a1 = vqrdmulhq_s32(a, N2);
  a = vmlsq_s32(a,a1,N);
  return(a);
}

void polymul(int16_t *h, const int16_t *f, const int16_t *g, const int16_t n) {

  int i, j;
  int16x4_t v0, v1, u0, u1;
  int32x4_t a0, a1, a2, a3, t0, t1, t2, z;
  int16x4_t tt0, tt1;
  
  //assert(n % 16 == 0);

  a0 ^= a0; a1 ^= a1; a2 ^= a2; a3 ^= a3; z ^= z;
  
  for (i = 0; i < n; i+=8) {
    for (j = 0; j <= i; j+=8) {
      v0 = vld1_s16(&f[j]); v1 = vld1_s16(&f[j+4]);
      u0 = vld1_s16(&g[i-j]); u1 = vld1_s16(&g[i-j+4]);

      a0 = vmlal_lane_s16(a0,u0,v0,0); a1 = vmlal_lane_s16(a1,u1,v0,0);

      t0 = vmull_lane_s16(u0,v0,1); t1 = vmull_lane_s16(u1,v0,1);
      t2 = vextq_s32(t1,z,3); t1 = vextq_s32(t0,t1,3); t0 = vextq_s32(z,t0,3);
      a0 = vaddq_s32(a0,t0); a1 = vaddq_s32(a1,t1); a2 = vaddq_s32(a2,t2);

      t0 = vmull_lane_s16(u0,v0,2); t1 = vmull_lane_s16(u1,v0,2);
      t2 = vextq_s32(t1,z,2); t1 = vextq_s32(t0,t1,2); t0 = vextq_s32(z,t0,2);
      a0 = vaddq_s32(a0,t0); a1 = vaddq_s32(a1,t1); a2 = vaddq_s32(a2,t2);

      t0 = vmull_lane_s16(u0,v0,3); t1 = vmull_lane_s16(u1,v0,3);
      t2 = vextq_s32(t1,z,1); t1 = vextq_s32(t0,t1,1); t0 = vextq_s32(z,t0,1);
      a0 = vaddq_s32(a0,t0); a1 = vaddq_s32(a1,t1); a2 = vaddq_s32(a2,t2);
     
      a1 = vmlal_lane_s16(a1,u0,v1,0); a2 = vmlal_lane_s16(a2,u1,v1,0);
      
      t0 = vmull_lane_s16(u0,v1,1); t1 = vmull_lane_s16(u1,v1,1);
      t2 = vextq_s32(t1,z,3); t1 = vextq_s32(t0,t1,3); t0 = vextq_s32(z,t0,3);
      a1 = vaddq_s32(a1,t0); a2 = vaddq_s32(a2,t1); a3 = vaddq_s32(a3,t2);
      
      t0 = vmull_lane_s16(u0,v1,2); t1 = vmull_lane_s16(u1,v1,2);
      t2 = vextq_s32(t1,z,2); t1 = vextq_s32(t0,t1,2); t0 = vextq_s32(z,t0,2);
      a1 = vaddq_s32(a1,t0); a2 = vaddq_s32(a2,t1); a3 = vaddq_s32(a3,t2);
      
      t0 = vmull_lane_s16(u0,v1,3); t1 = vmull_lane_s16(u1,v1,3);
      t2 = vextq_s32(t1,z,1); t1 = vextq_s32(t0,t1,1); t0 = vextq_s32(z,t0,1);
      a1 = vaddq_s32(a1,t0); a2 = vaddq_s32(a2,t1); a3 = vaddq_s32(a3,t2);
      
      if (j & 0xf8== 0xf8) {
	a0=reduce32x4(a0); a1=reduce32x4(a1);
	a2=reduce32x4(a2); a3=reduce32x4(a3);
      }
    }
    a0=reduce32x4(a0); a1=reduce32x4(a1); a2=reduce32x4(a2); a3=reduce32x4(a3);
    a0=reduce32x4(a0); a1=reduce32x4(a1); a2=reduce32x4(a2); a3=reduce32x4(a3);

    tt0 = vmovn_s32(a0); vst1_s16(&h[i],tt0);
    tt1 = vmovn_s32(a1); vst1_s16(&h[i+4],tt1);
    a0 = a2; a1 = a3; a2 ^= a2; a3 ^= a3;
  }  

  for (; i < n * 2 - 8; i+=8) {
    for (j = i - n + 8; j < n; j+=8) {
      v0 = vld1_s16(&f[j]); v1 = vld1_s16(&f[j+4]);
      u0 = vld1_s16(&g[i-j]); u1 = vld1_s16(&g[i-j+4]);

      a0 = vmlal_lane_s16(a0,u0,v0,0); a1 = vmlal_lane_s16(a1,u1,v0,0);

      t0 = vmull_lane_s16(u0,v0,1); t1 = vmull_lane_s16(u1,v0,1);
      t2 = vextq_s32(t1,z,3); t1 = vextq_s32(t0,t1,3); t0 = vextq_s32(z,t0,3);
      a0 = vaddq_s32(a0,t0); a1 = vaddq_s32(a1,t1); a2 = vaddq_s32(a2,t2);

      t0 = vmull_lane_s16(u0,v0,2); t1 = vmull_lane_s16(u1,v0,2);
      t2 = vextq_s32(t1,z,2); t1 = vextq_s32(t0,t1,2); t0 = vextq_s32(z,t0,2);
      a0 = vaddq_s32(a0,t0); a1 = vaddq_s32(a1,t1); a2 = vaddq_s32(a2,t2);

      t0 = vmull_lane_s16(u0,v0,3); t1 = vmull_lane_s16(u1,v0,3);
      t2 = vextq_s32(t1,z,1); t1 = vextq_s32(t0,t1,1); t0 = vextq_s32(z,t0,1);
      a0 = vaddq_s32(a0,t0); a1 = vaddq_s32(a1,t1); a2 = vaddq_s32(a2,t2);
     
      a1 = vmlal_lane_s16(a1,u0,v1,0); a2 = vmlal_lane_s16(a2,u1,v1,0);
      
      t0 = vmull_lane_s16(u0,v1,1); t1 = vmull_lane_s16(u1,v1,1);
      t2 = vextq_s32(t1,z,3); t1 = vextq_s32(t0,t1,3); t0 = vextq_s32(z,t0,3);
      a1 = vaddq_s32(a1,t0); a2 = vaddq_s32(a2,t1); a3 = vaddq_s32(a3,t2);
      
      t0 = vmull_lane_s16(u0,v1,2); t1 = vmull_lane_s16(u1,v1,2);
      t2 = vextq_s32(t1,z,2); t1 = vextq_s32(t0,t1,2); t0 = vextq_s32(z,t0,2);
      a1 = vaddq_s32(a1,t0); a2 = vaddq_s32(a2,t1); a3 = vaddq_s32(a3,t2);
      
      t0 = vmull_lane_s16(u0,v1,3); t1 = vmull_lane_s16(u1,v1,3);
      t2 = vextq_s32(t1,z,1); t1 = vextq_s32(t0,t1,1); t0 = vextq_s32(z,t0,1);
      a1 = vaddq_s32(a1,t0); a2 = vaddq_s32(a2,t1); a3 = vaddq_s32(a3,t2);
      
      if (j & 0xf8== 0xf8) {
	a0=reduce32x4(a0); a1=reduce32x4(a1);
	a2=reduce32x4(a2); a3=reduce32x4(a3);
      }
    }
    a0=reduce32x4(a0); a1=reduce32x4(a1); a2=reduce32x4(a2); a3=reduce32x4(a3);
    a0=reduce32x4(a0); a1=reduce32x4(a1); a2=reduce32x4(a2); a3=reduce32x4(a3);

    tt0 = vmovn_s32(a0); vst1_s16(&h[i],tt0);
    tt1 = vmovn_s32(a1); vst1_s16(&h[i+4],tt1);
    a0 = a2; a1 = a3; a2 ^= a2; a3 ^= a3;
  }
  tt0 = vmovn_s32(a0); vst1_s16(&h[i],tt0);
  tt1 = vmovn_s32(a1); vst1_s16(&h[i+4],tt1);

}
  
