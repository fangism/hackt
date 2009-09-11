/**
	\file "net/netlist_options.h"
	$Id: netlist_options.h,v 1.2.2.7 2009/09/11 02:46:06 fang Exp $
 */

#ifndef	__HAC_NET_NETLIST_OPTIONS_H__
#define	__HAC_NET_NETLIST_OPTIONS_H__

#include <string>
#include "net/common.h"
#include "util/optparse.h"
#include "util/named_ifstream_manager.h"

namespace HAC {
namespace NET {
using std::string;
using std::istream;
using util::option_value;
using util::option_value_list;
using util::ifstream_manager;

//=============================================================================
/**
	Overrideable options (configure).
	Many names borrowed from netgen for consistency and compatibility.
 */
struct netlist_options {
	typedef	netlist_options		this_type;
	static	const	netlist_options	default_value;
	typedef	bool (this_type::*option_memfun_type)(const option_value&);
	ifstream_manager		file_manager;
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
		These mangle substitutions apply to type and instance names.
	 */
	string				mangle_instance_member_separator;
	string				mangle_underscore;
	string				mangle_array_index_open;
	string				mangle_array_index_close;
	string				mangle_template_empty;
	string				mangle_template_open;
	string				mangle_template_close;
	string				mangle_parameter_separator;
	string				mangle_parameter_group_open;
	string				mangle_parameter_group_close;
	string				mangle_scope;
	string				mangle_colon;
	string				mangle_internal_at;
	string				mangle_auxiliary_pound;

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
		If true, also emit empty subcircuits, i.e. those with
		no transistors, for the sake of name aliases.  
	 */
	bool				empty_subcircuits;
	/**
		If true, wrap the top-level instances in its respective
		subcircuit, with ports listed.
		Useful when using a chosen type as the top-level.
		TODO: finish me
		Problem: need topfp to be overridden with chosen type's 
		footprint.
	 */
	bool				top_type_ports;
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
	set(const option_value&);

	bool
	set(const option_value_list&);

	real_type
	get_default_width(const bool d, const bool k) const;

	real_type
	get_default_length(const bool d, const bool k) const;

	bool
	no_mangling(const option_value&);

	bool
	open_config_file(const option_value&);

	bool
	open_config_file_compat(const option_value&);

	bool
	add_config_path(const option_value&);

	string&
	mangle_instance(string&) const;

	string&
	mangle_type(string&) const;

	const string&
	emit_scope(void) const;

	const string&
	emit_colon(void) const;

	const string&
	emit_internal_at(void) const;

	const string&
	emit_auxiliary_pound(void) const;

	ostream&
	line_continue(ostream&) const;

	static
	ostream&
	help(ostream&);

	ostream&
	dump(ostream&) const;

private:
	// not copy-able
	netlist_options(const netlist_options&);

	bool
	__open_config_file(const option_value&, 
		option_value_list (*)(istream&));

	string&
	mangle_name(string&) const;

};	// end struct netlist_options

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETLIST_OPTIONS_H__

