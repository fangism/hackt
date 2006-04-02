/**
	\file "sim/random_time.cc"
	$Id: random_time.cc,v 1.1.2.1 2006/04/02 23:11:04 fang Exp $
 */

#include "sim/random_time.h"
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

