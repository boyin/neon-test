CC = gcc
CFLAGS = -O3 -march=native -mtune=native -mcpu=native

AARCH := $(shell uname -m)
ifeq ($(AARCH),armv7l)
	CFLAGS += -mfpu=neon -marm 
endif

all: schoolbook schoolbook_neon_2 schoolbook_neon_3 schoolbook_neon_4 schoolbook_neon_6 schoolbook_neon_old ka_neon_48 ka_neon_48_4 ka_neon_24 ka_neon_96 ka_neon_96_4

schoolbook: schoolbook.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook schoolbook.c test.c

schoolbook_neon_old: schoolbook_neon_old.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon_old schoolbook_neon_old.c test.c

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

schoolbook_neon_negc_4.c: schoolbook_neon_negc.py
	python3 schoolbook_neon_negc.py 4 > schoolbook_neon_negc_4.c

schoolbook_neon_negc_4: schoolbook_neon_negc_4.c test_negc.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f schoolbook schoolbook_neon_{2,3,4,6,old} schoolbook_neon_{2,3,4,6}.c ka_neon_*
