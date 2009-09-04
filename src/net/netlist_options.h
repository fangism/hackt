/**
	\file "net/netlist_options.h"
	$Id: netlist_options.h,v 1.2.2.2 2009/09/04 22:21:50 fang Exp $
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
	static	const	netlist_options	default_value;
// generation-time options:
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
		Units to be appended, e.g. "p" (pico) in area units.
		Use needs to keep this consistent with length unit!
	 */
	string				area_unit;
	/**
		String to emit between levels of instance hierarchy, 
		usually '.' or '/'.
	 */
	string				instance_member_separator;
	/**
		String to emit before newline of a continued line.
	 */
	string				pre_line_continue;
	/**
		String to emit after newline of a continued line.
	 */
	string				post_line_continue;
	/**
		lambda, or unit scale factor multiplier.
	 */
	real_type			lambda;
	/**
		Transistor size limits, all in lambda.
	 */
	real_type			min_width;
	real_type			min_length;
	real_type			max_p_width;
	real_type			max_n_width;
	/**
		End transistor overhang length in lambda.
		Only relevant with emit_parasitic=1.
	 */
	real_type			fet_diff_overhang;
	/**
		FET to FET spacing in diffusion.
		Only relevant with emit_parasitic=1.
	 */
	real_type			fet_spacing_diffonly;
	/**
		If true, emit area/perimeter of source/drain diffusions.
	 */
	bool				emit_parasitics;
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

	real_type
	get_default_width(const bool d, const bool k) const;

	real_type
	get_default_length(const bool d, const bool k) const;


	ostream&
	line_continue(ostream&) const;

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

