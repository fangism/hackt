/**
	\file "sim/time.cc"
	$Id: time.cc,v 1.2 2006/04/23 07:37:25 fang Exp $
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

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC
