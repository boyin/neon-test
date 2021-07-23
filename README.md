# neon-test

# Testing ground for multiplications on the ARM

# `./karatsuba_neon.py NN > ka_neon_NN.c`

generate Karatsuba multiplication with base size NN.
ka_neon_NN.c requires a separate multiplication called 

`polymul_sb (h, f, g, n)`

where n-long f and g are multiplied together into 2n-long h, to function

	* Note that NN needs to divide the size we want to use


# `./schoolbook_neon.py N > schoolbook_neon_N.c`

generate schoolbook multiplication with base size 8N

	* Note that 8N needs to divide the size we want to use.

# test.c

size 768 polynomial multiplication mod 4591

requires -DKARATSUBA for polymul_ka to function as polymul

if you add another variation on multiplications, my suggestion is that you use a similar compiler switch



# enable_ccr

module to enable the performance counters.
from Raspberry Pi OS (Raspbian), 

`sudo apt install raspberrypi-kernel-headers`

to enable kernel build.  go to enable_ccr
and type `make; make install`
