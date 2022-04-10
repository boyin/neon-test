
	//.cpu	cortex-m4

	.macro	QR, a, b, c, d
	add	\a, \a, \b, ROR #25
	eor	\d, \a, \d, ROR #24
	add	\c, \c, \d, ROR #16
	eor	\b, \c, \b, ROR #25
	add	\a, \a, \b, ROR #20
	eor	\d, \a, \d, ROR #16
	add	\c, \c, \d, ROR #24
	eor	\b, \c, \b, ROR #20
	.ENDM

	.macro	spillx12x14
    	vmov	s16, r12
	vmov	s18, r14
	vmov	r12, s17
	vmov	r14, s19
	.endm

	.macro	spillx13x15
    	vmov	s17, r12
	vmov	s19, r14
	vmov	r12, s16
	vmov	r14, s18
	.endm

	
	// int crypto_stream_chacha20(unsigned char *c,
	// 			    unsigned long long clen,
	//			    unsigned char *n,
	//			    unsigned char *k);

    	// s0-s15 = j0-j15, s16-s19 = x12-x15
	// s20, s21 = #1.0, #9.0 (loop counter), 
	// s22 = dup output pointer, s23 = clen,  
	// s24 = return address, s25 = output pointer

	.p2align	2,,3	
	.syntax		unified
	.data
block:
	.space	64

	.p2align	2,,3	
	.syntax		unified
	.text
sigma:
    	.ascii	"expand 32-byte k"
	.global crypto_stream_chacha20
	.type	crypto_stream_chacha20, %function

crypto_stream_chacha20:
	// if len == 0, return 0
//	cbnz.n r2, temp0
//	mov.n r0, r2
//	bx.n r14
// temp0:
// 	cbz.n r3, temp1
//	mvns.n	r0, r2
//	bx.n r14
// temp1:
	cmp	r3, #0	
	itt	ne
	movne	r0, #-1
	bxne	lr	// if r3 != 0 then overflow
    	cmp	r2, #0
	itt	eq
    	moveq	r0, r2
	bxeq	lr	// if r2 == 0 then return 
       	// r0 = out, r2,r3 = len > 0, [sp] = nonce, [sp, #4] = key
	vmov	s12, r3
	vmov	s13, r3		// clear s12-13 = counter
	vmov.f32	s20, #1.0	// float 1
    	ldr	r1, [sp]	// nonce
	ldr	r3, [sp, #4]	// key 
	push	{r4-r11,lr}
	vpush	{s16-s25}

	
	vmov	s25, r0		// save out
    	vmov	s23, r2		// save len
    	adr	r12, sigma
	// note can't use vldm since address may be unaligned
	vldr	s14, [r1]
	vldr	s15, [r1, #4]	// s14-15 = nonce

	vldr	s0, [r12, #0]
	vldr	s1, [r12, #4]
	vldr	s2, [r12, #8]
	vldr	s3, [r12, #12]
	vldr	s4, [r3, #0]
	vldr	s5, [r3, #4]
	vldr	s6, [r3, #8]
	vldr	s7, [r3, #12]
	vldr	s8, [r3, #16]
	vldr	s9, [r3, #20]
	vldr	s10, [r3, #24]
	vldr	s11, [r3, #28]

	//vldm	r12, {s0-s3}
crypto_stream_chacha20_asm_0:
	vmov	r12, s23
	subs	r14, r12, #64	// len >= 64?
	bcc	crypto_stream_chacha20_asm_1
    	vmov	s23, r14
    	bl	crypto_core_chacha20
    	vmov	s25, r12
	// vmov	r6, r7, d6	// s12-13 (counter)
	// adds	r6, r6, #1
	// adc	r7, r7, #0	// increment counter
    	// vmov	d6, r6, r7
    	b	crypto_stream_chacha20_asm_0

crypto_stream_chacha20_asm_1:
	cmp	r12, #0
	beq	crypto_stream_chacha20_asm_2
    	movw	r14, #:lower16:block
    	movt	r14, #:upper16:block
	vmov	s22, s25
	vmov	s25, r14
    	bl	crypto_core_chacha20
	sub	r1, r12, #64
	vmov	r0, s22
	vmov	r2, s23
    	bl	memcpy

crypto_stream_chacha20_asm_2:
	mov	r0, #0


	vpop	{s16-s25}
	pop	{r4-r11,pc}

    
	.p2align	2,,3	
	.syntax		unified
	.text
crypto_core_chacha20:		// s0-s15 initial state s25 output
	vmov	s24, lr		// save return address

	vmov	r0, r1, d0
	vmov	r2, r3, d1
	vmov	r4, r5, d2
	vmov	r6, r7, d3
	vmov	r8, r9, d6
	vmov	r10, r11, d7
	vmov	s17, s9
	vmov	s19, s11
	vmov	r12, s8
	vmov	r14, s10
	ror	r4, r4, #7
	ror	r8, r8, #8
	ror	r5, r5, #7
	ror	r9, r9, #8
	ror	r6, r6, #7
	ror	r10, r10, #8
	ror	r7, r7, #7
	ror	r11, r11, #8
	vmov.f32	s21, # 5.0	// counter
block_chacha20_loop0_1:

	QR	r0, r4,r12, r8
	QR	r2, r6,r14,r10
	spillx12x14
	QR	r1, r5,r12, r9
	QR	r3, r7,r14,r11
	QR 	r1, r6,r14, r8
	QR 	r3, r4,r12,r10
	spillx13x15
	QR 	r0, r5,r14,r11
	QR 	r2, r7,r12, r9


	QR	r0, r4,r12, r8
	QR	r2, r6,r14,r10
	spillx12x14
	QR	r1, r5,r12, r9
	QR	r3, r7,r14,r11
	QR 	r1, r6,r14, r8
	QR 	r3, r4,r12,r10
	spillx13x15
	QR 	r0, r5,r14,r11
	QR 	r2, r7,r12, r9


    	vsub.f32	s21, s21, s20	// counter -= 1
    	vcmp.f32	s21, #0.0	// counter = 0?
	vmrs 	APSR_nzcv, FPSCR
	bgt	block_chacha20_loop0_1
block_chacha20_end:
	vmov	s16, r12		// s16 = x8
	vmov	r12, s10		// j10
	add	r14, r12, r14		// x10+j10
	//
    	vmov	r12, s25		// out
    	//
	str	r14, [r12, #40]
	//

	vmov	r14, s0
	add	r0, r14, r0
	vmov	r14, s4
	add	r4, r14, r4, ROR #25
	vmov	r14, s1
	add	r1, r14, r1
	vmov	r14, s5
	add	r5, r14, r5, ROR #25
	vmov	r14, s2
	add	r2, r14, r2
	vmov	r14, s6
	add	r6, r14, r6, ROR #25
	vmov	r14, s3
	add	r3, r14, r3
	vmov	r14, s7
	add	r7, r14, r7, ROR #25
	str	r1, [r12, #4]
	str	r2, [r12, #8]
	str	r3, [r12, #12]
	str	r4, [r12, #16]
	str	r5, [r12, #20]
	str	r6, [r12, #24]
	str	r7, [r12, #28]
	str	r0, [r12], #32
	vmov	r0, s8
	vmov	r4, s16
	vmov	r1, s9
	vmov	r5, s17
	vmov	r3, s11
	vmov	r7, s19
	add	r4, r0, r4
	add	r5, r1, r5
	add	r7, r3, r7
	vmov	r0, s12
	add	r8, r0, r8, ROR #24
	vmov	r1, s13
	add	r9, r1, r9, ROR #24
	vmov	r2, s14
	add	r10, r2, r10, ROR #24
	vmov	r3, s15
	add	r11, r3, r11, ROR #24
	
	adds	r0, r0, #1
	adc	r1, r1, #0
	vmov	d6, r0, r1

	str	r8, [r12, #16]
	str	r9, [r12, #20]
	str	r10, [r12, #24]
	str	r11, [r12, #28]
	str	r5, [r12, #4]
	str	r7, [r12, #12]
	str	r4, [r12], #32
	vmov	lr, s24
	bx	lr	// return output block address in r12
