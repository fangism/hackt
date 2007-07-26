// "bad_gcd1.cc"
// extra library function provided for chpsim

#include "sim/chpsim/chpsim_dlfunction.h"
// #include "libhackt/sim/chpsim/chpsim_dlfunction.h"

USING_CHPSIM_DLFUNCTION_PROLOGUE

// bogus function, with wrong return type

static
bool_value_type
bad_gcd(const int_value_type a, const int_value_type b) {
	return false;
}

CHP_DLFUNCTION_LOAD_DEFAULT("my_gcd", bad_gcd)

