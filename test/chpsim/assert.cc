/**
	\file "assert.cc"
	$Id: assert.cc,v 1.1.2.1 2007/08/09 23:05:52 fang Exp $
	Conditional operator.
	And selection operator (one of many), eventually.
 */

#include <iostream>
#include "sim/chpsim/chpsim_dlfunction.h"

USING_CHPSIM_DLFUNCTION_PROLOGUE
using std::cerr;
using std::endl;

static
void
my_assert(const bool_value_type z) {
	if (!z) {
		cerr << "Run-time assertion failed!" << endl;
		THROW_EXIT;		// just generic std::exception
	}
	// expect caller to catch and give a more useful message of context
}

CHP_DLFUNCTION_LOAD_DEFAULT("assert", my_assert)

