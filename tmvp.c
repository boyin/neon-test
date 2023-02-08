#define SB0 144

void tc5(uint16_t *restrict w, uint16_t *restrict polynomial) {
    uint16_t *w0_mem = &w[0*SB0],
             *w1_mem = &w[1*SB0],
             *w2_mem = &w[2*SB0],
             *w3_mem = &w[3*SB0],
             *w4_mem = &w[4*SB0],
             *w5_mem = &w[5*SB0],
             *w6_mem = &w[6*SB0],
             *w7_mem = &w[7*SB0],
             *w8_mem = &w[8*SB0],
             *c0 = &polynomial[0*SB0],
             *c1 = &polynomial[1*SB0],
             *c2 = &polynomial[2*SB0],
             *c3 = &polynomial[3*SB0],
             *c4 = &polynomial[4*SB0];
    uint16x8_t r0, r1, r2, r3, r4, p0, p1, p_1, tp;
    uint16x8_t zero;
    zero = vmovq_n_u16(0);
    for (uint16_t addr = 0; addr < SB0; addr+= 8){
        r0 = vld1q_u16(&c0[addr]);
        r1 = vld1q_u16(&c1[addr]);
        r2 = vld1q_u16(&c2[addr]);
        r3 = vld1q_u16(&c3[addr]);
        r4 = vld1q_u16(&c4[addr]);

        p0 = vaddq_u16(r0, r2);  // p0  = r0 + r2
        p0 = vaddq_u16(p0, r4);  // p0  = p0 + r4 = r0 + r2 + r4
        tp = vaddq_u16(r1, r3);  // tp  = r1 + r3

        p1 = vaddq_u16( p0, tp); // p1  = p0 + tp = r0 + r2 + r4 + r1 + r3
        p_1 = vsubq_u16(p0, tp); // p_1 = p0 - tp = r0 + r2 + r4 - r1 - r3
        vst1q_u16(&w0_mem[addr], r0); // A(0)   = r0
        vst1q_u16(&w1_mem[addr], p1); // A(1)   = r0 + r2 + r4 + r1 + r3
        vst1q_u16(&w2_mem[addr], p_1);// A(-1)  = r0 + r2 + r4 - r1 - r3
        vst1q_u16(&w8_mem[addr], r4); // A(inf) = r4

        // deal w/ A(2), A(-2)
        p0 = vshlq_n_u16(r4, 2);  // p0 = (4) *(r4)
        p0 = vaddq_u16(p0, r2); // p0 = (4) *(r4) + r2
        p0 = vshlq_n_u16(p0, 2);  // p0 = (16)*(r4) + (4)*r2
        p0 = vaddq_u16(p0, r0); // p0 = (16)*(r4) + (4)*r2 + r0

        tp = vshlq_n_u16(r3,  2); // tp = (4)*(r3)
        tp = vaddq_u16(tp, r1); // tp = (4)*(r3) + r1
        tp = vshlq_n_u16(tp, 1);  // tp = (8)*(r3) + (2)*r1

        p1 = vaddq_u16( p0, tp); // p1  = p0 + tp = (16)*(r4) + (4)*r2 + r0 + (8)*(r3) + (2)*r1
        p_1 = vsubq_u16(p0, tp); // p_1 = p0 - tp = (16)*(r4) + (4)*r2 + r0 - (8)*(r3) - (2)*r1
        vst1q_u16(&w3_mem[addr], p1); // A(2)    = (16)*(r4) + (4)*r2 + r0 + (8)*(r3) + (2)*r1
        vst1q_u16(&w4_mem[addr], p_1);// A(-2)   = (16)*(r4) + (4)*r2 + r0 - (8)*(r3) - (2)*r1

        // deal w/ A(3)
        p0 = vmulq_n_u16(r4, 9);  // p0 = (9) *(r4)
        p0 = vaddq_u16(p0, r2); // p0 = (9) *(r4) + r2
        p0 = vmulq_n_u16(p0, 9);  // p0 = (81)*(r4) + (9)*r2
        p0 = vaddq_u16(p0, r0); // p0 = (81)*(r4) + (9)*r2 + r0

        tp = vmulq_n_u16(r3, 9);   // tp = (9)*(r3)
        tp = vaddq_u16(tp, r1);    // tp = (9)*(r3) + r1
        tp = vmulq_n_u16(tp, 3);   // tp = (27)*(r3) + (3)*r1

        p1 = vaddq_u16( p0, tp); // p1  = (81)*(r4) + (9)*r2 + r0 + (27)*(r3) + (3)*r1
        vst1q_u16(&w5_mem[addr], p1);  // A(3)    = (81)*(r4) + (9)*r2 + r0 + (27)*(r3) + (3)*r1

        // deal w/ A(1/2), A(-1/2)
        p0 = vshlq_n_u16(r0, 2);  // p0 = (4) *(r0)
        p0 = vaddq_u16(p0, r2); // p0 = (4) *(r0) + r2
        p0 = vshlq_n_u16(p0, 2);  // p0 = (16)*(r0) + (4)*r2
        p0 = vaddq_u16(p0, r4); // p0 = (16)*(r0) + (4)*r2 + r4

        tp = vshlq_n_u16(r1,  2); // tp = (4)*(r1)
        tp = vaddq_u16(tp, r3); // tp = (4)*(r1) + r3
        tp = vshlq_n_u16(tp, 1);  // tp = (8)*(r1) + (2)*r3

        p1 = vaddq_u16( p0, tp); // p1  = p0 + tp = (16)*(r0) + (4)*r2 + r4 + (8)*(r1) + (2)*r3
        p_1 = vsubq_u16(p0, tp); // p_1 = p0 - tp = (16)*(r0) + (4)*r2 + r4 - (8)*(r1) - (2)*r3

        vst1q_u16(&w6_mem[addr], p1);  // A(1/2)   = (16)*(r0) + (4)*r2 + r4 + (8)*(r1) + (2)*r3
        vst1q_u16(&w7_mem[addr], p_1); // A(-1/2)  = (16)*(r0) + (4)*r2 + r4 - (8)*(r1) - (2)*r3
    }
}
