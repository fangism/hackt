// "installcheck.cc"
// Don't try to compile this, this is only intended as an install-time test!

#include <sim/chpsim/chpsim_dlfunction.hh>

USING_CHPSIM_DLFUNCTION_PROLOGUE

static
int_value_type
bogus(const bool_value_type b) {
	return 0;
}

CHP_DLFUNCTION_LOAD_DEFAULT("bogus", bogus)

