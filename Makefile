CC = gcc
CFLAGS = -O3 -march=native -mtune=native -mcpu=native

AARCH := $(shell uname -m)
ifeq ($(AARCH),armv7l)
	CFLAGS += -mfpu=neon -marm 
endif

all: schoolbook schoolbook_neon_2 schoolbook_neon_3 schoolbook_neon_4 schoolbook_neon_6 schoolbook_neon_old

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
	$(CC) $(CFLAGS) -o ka_neon_48 ka_neon_48.c schoolbook_neon_6.c test.c

clean:
	rm schoolbook schoolbook_neon_{2,3,4,6,old} schoolbook_neon_{2,3,4,6}.c
