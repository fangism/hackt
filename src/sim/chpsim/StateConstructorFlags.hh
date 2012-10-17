/**
	\file "sim/chpsim/StateConstructorFlags.hh"
	The visitor that initializes and allocates CHPSIM state.  
	$Id: StateConstructorFlags.hh,v 1.2 2007/01/21 06:00:45 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_STATECONSTRUCTORFLAGS_H__
#define	__HAC_SIM_CHPSIM_STATECONSTRUCTORFLAGS_H__

#include "Object/lang/cflat_context_visitor.hh"

namespace HAC {
namespace SIM {
namespace CHPSIM {

/**
	Flags for directing chpsim state initialization.  
	Includes diagnostic controls, optimization details.  
 */
struct StateConstructorFlags {
	enum {
		DEFAULT_FLAGS			= 0x00
	};
	unsigned char				flags;

};	// end struct StateConstructorFlags

}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_STATECONSTRUCTORFLAGS_H__

