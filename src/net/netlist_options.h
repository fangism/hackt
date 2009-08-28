/**
	\file "net/netlist_options.h"
	$Id: netlist_options.h,v 1.2 2009/08/28 20:45:15 fang Exp $
 */

#ifndef	__HAC_NET_NETLIST_OPTIONS_H__
#define	__HAC_NET_NETLIST_OPTIONS_H__

#include <string>
#include "net/common.h"
#include "util/optparse.h"

namespace HAC {
namespace NET {
using std::string;

//=============================================================================
/**
	Overrideable options (configure).
	Many names borrowed from netgen for consistency and compatibility.
 */
struct netlist_options {
// geneeration-time options:
	/**
		Dimensions of standard devices to use when unspecified.  
		In absolute units instead of lambda.
	 */
	real_type			std_n_width;
	real_type			std_p_width;
	real_type			std_n_length;
	real_type			std_p_length;
	/**
		Dimensions of feedback (staticizer, keeper) devices 
		when unspecified.  
		In absolute units instead of lambda.
	 */
	real_type			stat_n_width;
	real_type			stat_p_width;
	real_type			stat_n_length;
	real_type			stat_p_length;

// output-time options:
	/**
		Units to be appended, e.g. "u" (micron), "e-1u", 
		"n" (nanometer).
	 */
	string				length_unit;
	/**
		lambda, or unit scale factor multiplier.
	 */
	real_type			lambda;
	/**
		Emit nested subcircuits.
		If true, print internal subcircuits locally to
		each super-subcircuit where used, else print
		all subcircuit definitions in flat namespace.
		Othewise, emit subcircuit definitions prior to use.  
	 */
	bool				nested_subcircuits;
	/**
		If true, emit top-level instances and rules, otherwise, 
		emit only subcircuit definitions (library-only).
	 */
	bool				emit_top;
	netlist_options();

	/**
		Processes raw options.
		\return true if there is an error.
	 */
	bool
	set(const util::option_value_list&);

	static
	ostream&
	help(ostream&);

	ostream&
	dump(ostream&) const;

};	// end struct netlist_options

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETLIST_OPTIONS_H__

