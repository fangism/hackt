/**
	\file "sim/time.cc"
	$Id: time.cc,v 1.3 2010/06/14 00:22:31 fang Exp $
 */

#include "sim/time.h"

namespace HAC {
namespace SIM {
//=============================================================================

const real_time
delay_policy<real_time>::invalid_value = -1.0;

const real_time
delay_policy<real_time>::default_delay = 10.0;

const real_time
delay_policy<real_time>::zero = 0.0;

const real_time
delay_policy<real_time>::infinity = std::numeric_limits<real_time>::infinity();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const discrete_time
delay_policy<discrete_time>::infinity =
	std::numeric_limits<discrete_time>::max();

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

