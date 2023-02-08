
#include <stdint.h>
#include <arm_neon.h>
#include <assert.h>
#include <stdio.h>

void tmvp_sb(int16_t *h,const int16_t *f,const int16_t *g,const int16_t n){

  int i, j;
  int16x4_t b0, b1;
  int16x8_t a, z0, z1, t;

  for (i = 0; i < n; i+=8) {
    a ^= a;
    for (j = 0; j < n; j+=8) {
      z1 = vld1q_s16(&g[n+i-j]);
      z0 = vld1q_s16(&g[n+i-j-8]);
      b0 = vld1_s16(&f[j]);
      b1 = vld1_s16(&f[j+4]);

      a = vmlaq_lane_s16(a,z1,b0,0);
      t = vextq_s16(z0,z1,7);
      a = vmlaq_lane_s16(a,t,b0,1);
      t = vextq_s16(z0,z1,6);
      a = vmlaq_lane_s16(a,t,b0,2);
      t = vextq_s16(z0,z1,5);
      a = vmlaq_lane_s16(a,t,b0,3);
      t = vextq_s16(z0,z1,4);
      a = vmlaq_lane_s16(a,t,b1,0);
      t = vextq_s16(z0,z1,3);
      a = vmlaq_lane_s16(a,t,b1,1);
      t = vextq_s16(z0,z1,2);
      a = vmlaq_lane_s16(a,t,b1,2);
      t = vextq_s16(z0,z1,1);
      a = vmlaq_lane_s16(a,t,b1,3);
    }
    vst1q_s16(&h[i],a);
  }
}
