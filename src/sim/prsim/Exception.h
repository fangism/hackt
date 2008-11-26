/**
	\file "sim/prsim/Exception.h"
	Generic run-time exceptions for simulator.  
	$Id: Exception.h,v 1.2.6.1 2008/11/26 05:16:28 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXCEPTION_H__
#define	__HAC_SIM_PRSIM_EXCEPTION_H__

#include <iosfwd>
#include "sim/prsim/enums.h"

namespace HAC {
namespace SIM {
namespace PRSIM {

class State;

/**
	Generic simulation exception base class, 
	when something goes wrong.
 */
struct step_exception {
virtual	~step_exception() { }

virtual	error_policy_enum
	inspect(const State&, std::ostream&) const = 0;
};

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXCEPTION_H__

