/**
	\file "sim/prsim/Exception.hh"
	Generic run-time exceptions for simulator.  
	$Id: Exception.hh,v 1.3 2008/11/27 11:09:37 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXCEPTION_H__
#define	__HAC_SIM_PRSIM_EXCEPTION_H__

#include <iosfwd>
#include "sim/common.hh"	// for node_index_type
#include "sim/command_error_codes.hh"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;
class State;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic simulation exception base class, 
	when something goes wrong.
 */
struct step_exception {
virtual	~step_exception() { }

virtual	error_policy_enum
	inspect(const State&, std::ostream&) const = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Minimalist generic exception for events on nodes.
	Includes error policy for the type of exception.
 */
struct generic_exception : public step_exception {
	node_index_type			node_id;
	error_policy_enum		policy;

	generic_exception(const node_index_type n,
		const error_policy_enum e) :
		node_id(n), policy(e) { }

	void
	save(ostream&) const;

	void
	load(istream&);

virtual error_policy_enum
	inspect(const State&, ostream&) const;
};      // end struct generic_exception


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXCEPTION_H__

