/**
	\file "sim/prsim/Exception.h"
	Generic run-time exceptions for simulator.  
	$Id: Exception.h,v 1.1.2.1 2008/02/29 22:42:21 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXCEPTION_H__
#define	__HAC_SIM_PRSIM_EXCEPTION_H__

namespace HAC {
namespace SIM {
namespace PRSIM {

/**
	Generic simulation exception base class, 
	when something goes wrong.
 */
struct step_exception {
virtual	~step_exception() { }
};

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXCEPTION_H__

