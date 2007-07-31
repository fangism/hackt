// "gcd.cc"
// extra library function provided for chpsim

#include "sim/chpsim/chpsim_dlfunction.h"
// #include "libhackt/sim/chpsim/chpsim_dlfunction.h"

USING_CHPSIM_DLFUNCTION_PROLOGUE

/**
	\pre a >= b
 */
static
size_t
__gcd(const size_t a, const size_t b) {
	if (b == 0)
		return a;
	else if (b == 1)
		return 1;
	else	return __gcd(b, a%b);
}

static
int_value_type
gcd(const int_value_type a, const int_value_type b) {
	if (a >= b)
		return __gcd(a, b);
	else	return __gcd(b, a);
}

CHP_DLFUNCTION_LOAD_DEFAULT("my_gcd", gcd)

