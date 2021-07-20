CC = gcc
CFLAGS = -O3 -march=native -mtune=native

AARCH := $(shell uname -m)
ifeq ($(AARCH),armv7l)
	CFLAGS += -mfpu=neon -marm
endif

all: schoolbook schoolbook_neon

schoolbook: schoolbook.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook schoolbook.c test.c

schoolbook_neon: schoolbook_neon.c cpucycles.h mock_std_mult.h test.c
	$(CC) $(CFLAGS) -o schoolbook_neon schoolbook_neon.c test.c

clean:
	rm schoolbook schoolbook_neon a.out
