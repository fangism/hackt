/**
	\file "util/numeric/random.cc"
	Static initializers for random generators.  
	$Id: random.cc,v 1.4 2011/05/11 15:42:20 fang Exp $
 */

#include <limits>		// the C++ numeric_limits header
#include <cmath>
#include <iostream>
#include "util/numeric/random.hh"
#include "util/utypes.h"	// for ushort
#include "util/IO_utils.tcc"

namespace util {
namespace numeric {
using std::istream;
using std::ostream;
#if 0
using std::numeric_limits;
using std::log;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
seed48_zeros(void) {
	ushort seed[3] = {0,0,0};
	seed48(seed);
	// ALERT: not every libc resets with the same 0-seed!!!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
write_seed48(ostream& o) {
	// save the random seed
	ushort seed[3] = {0, 0, 0};
	const ushort* old_seed = seed48(seed);  // libc
	seed[0] = old_seed[0];
	seed[1] = old_seed[1];
	seed[2] = old_seed[2];
	// put it back
	seed48(seed);
	write_value(o, seed[0]);
	write_value(o, seed[1]);
	write_value(o, seed[2]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
read_seed48(istream& i) {
	// restore random seed
	ushort seed[3];
	read_value(i, seed[0]);
	read_value(i, seed[1]);
	read_value(i, seed[2]);
	seed48(seed);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace numeric
}	// end namespace util

