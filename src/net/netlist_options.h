/**
	\file "net/netlist_options.h"
	$Id: netlist_options.h,v 1.18 2011/03/30 04:19:02 fang Exp $
 */

#ifndef	__HAC_NET_NETLIST_OPTIONS_H__
#define	__HAC_NET_NETLIST_OPTIONS_H__

#include <string>
#include <list>
#include <set>
#include <map>
#include "net/common.h"
#include "util/optparse.h"
#include "util/named_ifstream_manager.h"
#include "Object/common/dump_flags.h"

/**
	Define to 1 to enable Cadence Spectre output format support.
	Goal: 1
 */
#define	SPECTRE_SUPPORT			1

namespace HAC {
namespace NET {
using std::map;
using std::set;
using std::string;
using std::list;
using std::istream;
using util::option_value;
using util::option_value_list;
using util::ifstream_manager;
using entity::dump_flags;

// for error-handling
// just use the same enum
typedef	error_status	option_error_policy;

/**
	Store a set of strings as a map, where the key-string is 
	determined by case-sensitivity checking, and the
	value-string is always the case-preserved string.
 */
#if 0
typedef	set<string>		string_set_type;
#else
typedef	map<string, string>	string_set_type;
#endif

//=============================================================================
/**
	Overrideable options (configure).
	Many names borrowed from netgen for consistency and compatibility.
 */
struct netlist_options {
	typedef	netlist_options		this_type;
	static	const	netlist_options	default_value;
	typedef	bool (this_type::*option_memfun_type)(const option_value&);
	/**
		Sorted set of option strings, that don't belong anywhere else.
		This map can be queried for existence of options, 
		and their values.  
	 */
	typedef	map<string, list<string> >	misc_options_map_type;
	typedef	misc_options_map_type::value_type	misc_option;
	ifstream_manager		file_manager;
	dump_flags			__dump_flags;
	misc_options_map_type		misc_options_map;
#if SPECTRE_SUPPORT
	/**
		Output format style options.
		The styles can be mixed and matched, 
		not just preset bundled options. 
	 */
	typedef	enum {
		STYLE_SPICE,
		STYLE_SPECTRE,
		STYLE_VERILOG
	} style_enum;
#endif
	option_error_policy		unknown_option_policy;
	option_error_policy		internal_node_supply_mismatch_policy;
	option_error_policy		undriven_node_policy;
	option_error_policy		case_collision_policy;
	option_error_policy		non_CMOS_precharge_policy;
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
	string				capacitance_unit;
	string				resistance_unit;
	string				inductance_unit;
	/**
		String to emit between levels of instance hierarchy, 
		usually '.' or '/'.
		These mangle substitutions apply to type and instance names.
	 */
	string				mangle_process_member_separator;
	string				mangle_struct_member_separator;
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
	string				mangle_implicit_bang;

	/**
		Typically the card name for transistors, 'M' in spice.
	 */
	string				transistor_prefix;
	/**
		Typically the card name for subcircuits, 'x' in spice.
	 */
	string				subckt_instance_prefix;

	/**
		String to emit before newline of a continued line.
	 */
	string				pre_line_continue;
	/**
		String to emit after newline of a continued line.
	 */
	string				post_line_continue;
	/**
		String to emit for comments.  Default "* ".
		Spectre uses "// ".
	 */
	string				comment_prefix;
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
		Set of reserved names with special meanings to other 
		back-end tools.
	 */
	string_set_type			reserved_names;
	/**
		Set of preferred local node names, that take precedence
		over shortest-canonical-name or port-alias.
	 */
	string_set_type			preferred_names;
	/**
		If true, emit area/perimeter of source/drain diffusions.
	 */
	bool				emit_parasitics;
	/**
		If true, parasitic perimieter estimation includes gate-edge, 
		otherwise, subtracts out.  
		Default true, for compatibility with netgen.
	 */
	bool				fet_perimeter_gate_edge;
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
		If true, emit ports that are unused.
		Useful with empty_subcircuits.
	 */
	bool				unused_ports;
	/**
		Prefer any port name for an alias or unique node
		over the shortest-canonical name.
		Can make netlists more readable.
	 */
	bool				prefer_port_aliases;
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
		If true, includes bools (nodes) in ports lists.
		Default: true
	 */
	bool				node_ports;
	/**
		If true, include user-defined channels and structs
		in ports lists.
		Default: false, typically only enable for verilog.
	 */
	bool				struct_ports;
#if SPECTRE_SUPPORT
	/**
		Subckt definition style.
	 */
	style_enum			subckt_def_style;
	/**
		Style of printing instance ports.  
	 */
	style_enum			instance_port_style;
#endif
	/**
		If true, emit top-level instances and rules, otherwise, 
		emit only subcircuit definitions (library-only).
	 */
	bool				emit_top;
	/**
		Emit sets of nodes aliased.
		Should these names be mangled?
	 */
	bool				emit_node_aliases;
	/**
		If true, emit mapping between mangled and unmangled names
		in comments.
	 */
	bool				emit_mangle_map;


	netlist_options();

	/**
		Processes raw options.
		\return true if there is an error.
	 */
	bool
	set_option(const option_value&);

	bool
	set_options(const option_value_list&);

#if SPECTRE_SUPPORT
	bool
	preset_output_format(const option_value&);
#endif

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

	bool
	collides_reserved_name(const string&) const;

	bool
	matches_preferred_name(const string&) const;

	ostream&
	dump_preferred_names(ostream&) const;

	static
	ostream&
	help(ostream&);

	ostream&
	dump(ostream&) const;

	void
	commit(void);

private:
	// not copy-able
	netlist_options(const netlist_options&);

	bool
	__open_config_file(const option_value&, 
		option_value_list (*)(istream&));

};	// end struct netlist_options

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETLIST_OPTIONS_H__

