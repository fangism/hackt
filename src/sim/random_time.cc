/**
	\file "sim/random_time.cc"
	$Id: random_time.cc,v 1.2 2006/04/03 05:30:35 fang Exp $
 */

#include "sim/random_time.hh"
#include <limits>

namespace HAC {
namespace SIM {
using std::numeric_limits;

//=============================================================================
const double
random_time<unsigned short>::ln_max_val =
	log(numeric_limits<limit_type>::max());

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double
random_time<double>::ln_max_val =
	log(numeric_limits<limit_type>::max());

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

