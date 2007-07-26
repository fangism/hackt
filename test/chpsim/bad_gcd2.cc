// "bad_gcd2.cc"
// extra library function provided for chpsim

#include "sim/chpsim/chpsim_dlfunction.h"
// #include "libhackt/sim/chpsim/chpsim_dlfunction.h"

USING_CHPSIM_DLFUNCTION_PROLOGUE

// bogus function, with wrong argument type

static
int_value_type
bad_gcd(const bool_value_type a, const bool_value_type b) {
	return 7;
}

CHP_DLFUNCTION_LOAD_DEFAULT("my_gcd", bad_gcd)

