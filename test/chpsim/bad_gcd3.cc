// "bad_gcd3.cc"
// extra library function provided for chpsim

#include "sim/chpsim/chpsim_dlfunction.h"
// #include "libhackt/sim/chpsim/chpsim_dlfunction.h"

USING_CHPSIM_DLFUNCTION_PROLOGUE

// wrong number of arguments -- run-time error
static
int_value_type
gcd(const int_value_type a, const int_value_type b, const int_value_type c) {
	return 666;
}

CHP_DLFUNCTION_LOAD_DEFAULT("my_gcd", gcd)

