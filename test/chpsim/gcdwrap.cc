/**
	\file "gcdwrap.cc"
	$Id: gcdwrap.cc,v 1.2 2007/07/31 23:24:20 fang Exp $
	Wraps and links a static library as a dynamic library
	chpsim function module.  
 */

#include "gcdstatic.h"
#include "sim/chpsim/chpsim_dlfunction.h"

USING_CHPSIM_DLFUNCTION_PROLOGUE

/**
	Adhering to types recognized by chpsim wrapper library.  
 */
static
int_value_type
wrap_gcd(const int_value_type a, const int_value_type b) {
	return gcd(a, b);
}

CHP_DLFUNCTION_LOAD_DEFAULT("my_gcd", wrap_gcd)


