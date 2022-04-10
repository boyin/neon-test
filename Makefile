CC = gcc
CFLAGS = -O3 -march=native -mtune=native -mcpu=native

AARCH := $(shell uname -m)
ifeq ($(AARCH),armv7l)
	CFLAGS += -mfpu=neon -marm 
endif

all: schoolbook schoolbook_neon_2 schoolbook_neon_3 schoolbook_neon_4 schoolbook_neon_6 schoolbook_neon_old ka_neon_48 ka_neon_48_4 ka_neon_24 ka_neon_96 ka_neon_96_4 schoolbook3_neon_3 schoolbook3_neon_2 tc_neon_192x4

schoolbook: schoolbook.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook schoolbook.c test.c

schoolbook_neon_old: schoolbook_neon_old.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon_old schoolbook_neon_old.c test.c

schoolbook_neon_1.c: schoolbook_neon.py
	python3 schoolbook_neon.py 1 > schoolbook_neon_1.c

schoolbook_neon_1: schoolbook_neon_1.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon_1 schoolbook_neon_1.c test.c

schoolbook_neon_2.c: schoolbook_neon.py
	python3 schoolbook_neon.py 2 > schoolbook_neon_2.c

schoolbook_neon_2: schoolbook_neon_2.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon_2 schoolbook_neon_2.c test.c

schoolbook_neon_3.c: schoolbook_neon.py
	python3 schoolbook_neon.py 3 > schoolbook_neon_3.c

schoolbook_neon_3: schoolbook_neon_3.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon_3 schoolbook_neon_3.c test.c

schoolbook_neon_4.c: schoolbook_neon.py
	python3 schoolbook_neon.py 4 > schoolbook_neon_4.c

schoolbook_neon_4: schoolbook_neon_4.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon_4 schoolbook_neon_4.c test.c

schoolbook_neon_6.c: schoolbook_neon.py
	python3 schoolbook_neon.py 6 > schoolbook_neon_6.c

schoolbook_neon_6: schoolbook_neon_6.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon_6 schoolbook_neon_6.c test.c


ka_neon_48.c: karatsuba_neon.py
	python3 karatsuba_neon.py 48 > ka_neon_48.c

ka_neon_48: schoolbook_neon_6.c ka_neon_48.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DKARATSUBA -o ka_neon_48 ka_neon_48.c schoolbook_neon_6.c test.c

ka_neon_48_4: schoolbook_neon_4.c ka_neon_48.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DKARATSUBA -o ka_neon_48_4 ka_neon_48.c schoolbook_neon_4.c test.c

ka_neon_24.c: karatsuba_neon.py
	python3 karatsuba_neon.py 24 > ka_neon_24.c

ka_neon_24: schoolbook_neon_3.c ka_neon_24.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DKARATSUBA -o ka_neon_24 ka_neon_24.c schoolbook_neon_3.c test.c

ka_neon_96.c: karatsuba_neon.py
	python3 karatsuba_neon.py 96 > ka_neon_96.c

ka_neon_96: schoolbook_neon_6.c ka_neon_96.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DKARATSUBA -o ka_neon_96 ka_neon_96.c schoolbook_neon_6.c test.c

ka_neon_96_4: schoolbook_neon_4.c ka_neon_96.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DKARATSUBA -o ka_neon_96_4 ka_neon_96.c schoolbook_neon_4.c test.c

tc_neon_16x4.c: toom_cook_neon.py
	python3 toom_cook_neon.py 16 4 > tc_neon_16x4.c

tc_neon_16x4: schoolbook_neon_2.c tc_neon_16x4.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DSIZE=64 -DTOOM -o tc_neon_16x4 schoolbook_neon_2.c tc_neon_16x4.c test.c

tc_neon_192x4.c: toom_cook_neon.py
	python3 toom_cook_neon.py 192 4 > tc_neon_192x4.c

tc_neon_192x4: schoolbook_neon_4.c tc_neon_192x4.c ka_neon_48.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DTOOM -DKARATSUBA -o tc_neon_192x4 schoolbook_neon_4.c tc_neon_192x4.c ka_neon_48.c test.c

ka_neon_32.c: karatsuba_neon.py
	python3 karatsuba_neon.py 32 > ka_neon_32.c

tc_neon_128x6.c: toom_cook_neon.py
	python3 toom_cook_neon.py 128 6 > tc_neon_128x6.c

tc_neon_128x6: schoolbook_neon_4.c tc_neon_128x6.c ka_neon_32.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -DTOOM -DKARATSUBA -o tc_neon_128x6 schoolbook_neon_4.c tc_neon_128x6.c ka_neon_32.c test.c

schoolbook_neon_negc_4.c: schoolbook_neon_negc.py
	python3 schoolbook_neon_negc.py 4 > schoolbook_neon_negc_4.c

schoolbook_neon_negc_4: schoolbook_neon_negc_4.c test_negc.c
	$(CC) $(CFLAGS) -o $@ $^

schoolbook3_neon_2.c: schoolbook_neon_mod3.py
	python3 schoolbook_neon_mod3.py 2 > schoolbook3_neon_2.c

schoolbook3_neon_2: schoolbook3_neon_2.c cpucycles.h mock_std_mult.h test3.c
	$(CC) $(CFLAGS) -o schoolbook3_neon_2 schoolbook3_neon_2.c test3.c

schoolbook3_neon_3.c: schoolbook_neon_mod3.py
	python3 schoolbook_neon_mod3.py 3 > schoolbook3_neon_3.c

schoolbook3_neon_3: schoolbook3_neon_3.c cpucycles.h mock_std_mult.h test3.c
	$(CC) $(CFLAGS) -o schoolbook3_neon_3 schoolbook3_neon_3.c test3.c


schoolbook2p16_neon.c: schoolbook_neon2p16.py
	python3 schoolbook_neon2p16.py 1 > schoolbook2p16_neon.c

schoolbook2p16_neon: schoolbook2p16_neon.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -o schoolbook2p16_neon schoolbook2p16_neon.c test2p16.c

schoolbook2p16_neon_2.c: schoolbook_neon2p16.py
	python3 schoolbook_neon2p16.py 2 > schoolbook2p16_neon_2.c

schoolbook2p16_neon_2: schoolbook2p16_neon_2.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -o schoolbook2p16_neon_2 schoolbook2p16_neon_2.c test2p16.c

schoolbook2p16_neon_3.c: schoolbook_neon2p16.py
	python3 schoolbook_neon2p16.py 3 > schoolbook2p16_neon_3.c

schoolbook2p16_neon_3: schoolbook2p16_neon_3.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -o schoolbook2p16_neon_3 schoolbook2p16_neon_3.c test2p16.c

schoolbook2p16_neon_4.c: schoolbook_neon2p16.py
	python3 schoolbook_neon2p16.py 4 > schoolbook2p16_neon_4.c

schoolbook2p16_neon_4: schoolbook2p16_neon_4.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -o schoolbook2p16_neon_4 schoolbook2p16_neon_4.c test2p16.c


ka2p16_neon_24.c: karatsuba_neon2p16.py
	python3 karatsuba_neon2p16.py 24 > ka2p16_neon_24.c

ka2p16_neon_48.c: karatsuba_neon2p16.py
	python3 karatsuba_neon2p16.py 48 > ka2p16_neon_48.c

ka2p16_neon_16.c: karatsuba_neon2p16.py
	python3 karatsuba_neon2p16.py 16 > ka2p16_neon_16.c

ka2p16_neon_32.c: karatsuba_neon2p16.py
	python3 karatsuba_neon2p16.py 32 > ka2p16_neon_32.c

ka2p16_neon_48: schoolbook2p16_neon_2.c ka2p16_neon_48.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DKARATSUBA -o ka2p16_neon_48 ka2p16_neon_48.c schoolbook2p16_neon_2.c test2p16.c

ka2p16_neon_96.c: karatsuba_neon2p16.py
	python3 karatsuba_neon2p16.py 96 > ka2p16_neon_96.c

ka2p16_neon_96: schoolbook2p16_neon_2.c ka2p16_neon_96.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DKARATSUBA -o ka2p16_neon_96 ka2p16_neon_96.c schoolbook2p16_neon_2.c test2p16.c

tc2p16_neon_16x4.c: toom_cook_neon2p16.py
	python3 toom_cook_neon2p16.py 16 4 > tc2p16_neon_16x4.c

tc2p16_neon_16x4: schoolbook2p16_neon_2.c tc2p16_neon_16x4.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DSIZE=64 -DTOOM -o tc2p16_neon_16x4 schoolbook2p16_neon_2.c tc2p16_neon_16x4.c test2p16.c

tc2p16_neon_48x4_2.c: toom_cook_neon2p16.py
	python3 toom_cook_neon2p16.py 48 4 8192 polymul_tc_48x4 > tc2p16_neon_48x4_2.c

tc2p16_neon_192x4.c: toom_cook_neon2p16.py
	python3 toom_cook_neon2p16.py 192 4 > tc2p16_neon_192x4.c

tc2p16_neon_192x4_2.c: toom_cook_neon2p16.py
	python3 toom_cook_neon2p16.py 192 4 8192 polymul_tc polymul_tc_48x4 > tc2p16_neon_192x4_2.c

tc2p16_neon_192x4: schoolbook2p16_neon_3.c tc2p16_neon_192x4.c ka2p16_neon_48.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DTOOM -DKARATSUBA -o tc2p16_neon_192x4 schoolbook2p16_neon_3.c tc2p16_neon_192x4.c ka2p16_neon_48.c test2p16.c

tc2p16_neon_192x4_2: schoolbook2p16_neon_3.c tc2p16_neon_192x4_2.c ka2p16_neon_24.c tc2p16_neon_48x4_2.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DTOOM -DKARATSUBA -o tc2p16_neon_192x4_2 schoolbook2p16_neon_3.c ka2p16_neon_24.c tc2p16_neon_192x4_2.c tc2p16_neon_48x4_2.c test2p16.c

tc2p16_neon_192x4_3: schoolbook2p16_neon_3.c tc2p16_neon_192x4.c ka2p16_neon_24.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DTOOM -DKARATSUBA -o tc2p16_neon_192x4_3 schoolbook2p16_neon_3.c tc2p16_neon_192x4.c ka2p16_neon_24.c test2p16.c

tc2p16_neon_256x3.c: toom_cook_neon2p16.py
	python3 toom_cook_neon2p16.py 256 3 > tc2p16_neon_256x3.c

tc2p16_neon_256x3: schoolbook2p16_neon_2.c tc2p16_neon_256x3.c ka2p16_neon_32.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DTOOM -DKARATSUBA -o tc2p16_neon_256x3 schoolbook2p16_neon_2.c tc2p16_neon_256x3.c ka2p16_neon_32.c test2p16.c

tc2p16_neon_256x3_2: schoolbook2p16_neon_2.c tc2p16_neon_256x3.c ka2p16_neon_16.c cpucycles.h mock_std_mult.h test2p16.c
	$(CC) $(CFLAGS) -DTOOM -DKARATSUBA -o tc2p16_neon_256x3_2 schoolbook2p16_neon_2.c tc2p16_neon_256x3.c ka2p16_neon_16.c test2p16.c


toom_matrices.py : toom_matrix.sage
	sage toom_matrix.sage > toom_matrices.py

test-chacha20: test-chacha20.c chacha20.c
	$(CC) $(CFLAGS) test-chacha20.c chacha20.c

test-chacha20-S: test-chacha20.c chacha20.S
	$(CC) $(CFLAGS) test-chacha20.c chacha20.S

test-chacha20-neon: test-chacha20.c chacha20-neon.c
	$(CC) $(CFLAGS) test-chacha20.c chacha20-neon.c

clean:
	rm -f schoolbook schoolbook_neon_{2,3,4,6,old} schoolbook_neon_{2,3,4,6}.c ka_neon* schoolbook3_neon_? schoolbook3_neon_?.c schoolbook2p16_neon_? schoolbook2p16_neon_?.c  tc2p16_neon* ka2p16_neon*
