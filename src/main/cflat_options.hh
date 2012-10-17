/**
	\file "main/cflat_options.hh"
	$Id: cflat_options.hh,v 1.24 2011/05/02 21:27:19 fang Exp $
 */

#ifndef	__HAC_MAIN_CFLAT_OPTIONS_H__
#define	__HAC_MAIN_CFLAT_OPTIONS_H__

#include <string>
#include "main/compile_options.hh"
#include "Object/common/dump_flags.hh"

namespace HAC {
using entity::dump_flags;

/**
	Printing and operating options for cflat.
	Add variations and options as needed.  
 */
struct cflat_options {
	/**
		The preset modes will set the primary_tool
		to one of these values.  
		These values will be used to determine
		the output behavior w.r.t. macros and attributes.  
		Another mode would be to use bitfields.  
	 */
	typedef	enum {
		TOOL_NONE = 0,
		/**
			prsim that reads in cflattened output
		 */
		TOOL_PRSIM,
		/**
			production rule QDI checker
		 */
		TOOL_PRLINT,
		/**
			layout vs. schematic checking
		 */
		TOOL_LVS,
		/**
			hierarchical semi-procedural layout directives
		 */
		TOOL_LAYOUT,
		/**
			VCD trace file header
		 */
		TOOL_VCD
		// add more tools as they are needed
	}				primary_tool_enum;
	/**
		These enumerations dictate the connection-style
		to print.  
	 */
	typedef	enum {
		/**
			default value.  
		 */
		TOOL_OPTIONS_DEFAULT = 0x00,
		/**
			single-event-upset modeling for tools.  
			This case should always include baseline cases.  
			Current relevant tools affected: PRSIM, LAYOUT.  
		 */
		TOOL_OPTIONS_SEU = 0x01
	}				tool_options_enum;
	typedef	enum {
		CONNECT_STYLE_NONE = 0,
		CONNECT_STYLE_CONNECT,
		CONNECT_STYLE_EQUAL,
		CONNECT_STYLE_WIRE,
		CONNECT_STYLE_HIERARCHICAL	///< nested scopes
	}				connect_style_enum;
public:
	/**
		Use the primary_tool_enum values.  
	 */
	unsigned char			primary_tool;
	/**
		Bitfields for special modes of tools.  
		Use tool_option_enum values as masks.  
	 */
	unsigned char			tool_options;
	/**
		-connect style: connect "a" "b"
		-prsim style: = "a" "b"
	 */
	unsigned char			connect_style;
	/**
		Whether or not production rules are printed.
	 */
	bool				include_prs;

	/**
		Whether or not a = a should be printed.  
	 */
	bool				dump_self_connect;

	// bool				canonical_first;
	/**
		Print node attributes.  Default=false.
	 */
	bool				node_attributes;
	/**
		Print instance attributes one-per-line.  Default=false.
	 */
	bool				split_instance_attributes;
	/**
		Print additional non-size attributes of node literals 
		in rules.  Default=false.
	 */
	bool				literal_attributes;
	/**
		Whether or not to expand passn/passp directives
		to their equivalent (uni-directional) production rules.
		Default=true.
	 */
	bool				expand_pass_gates;
	/**
		Whether or not node names should be wrapped in quote.  
	 */
	bool				enquote_names;
	/**
		Whether or not aliases for non-bool (structures)
		are printed.  
	 */
	bool				dump_non_bools;

	typedef	enum {
		/**
			Means: print namespaces normally, using :: delimiter.  
		 */
		NAMESPACE_POLICY_NONE = 0, 
		NAMESPACE_POLICY_TRUNCATE,
		NAMESPACE_POLICY_MANGLE,
		NAMESPACE_POLICY_OTHER
	}				namespace_policy_enum;
	/**
		How to deal with namespaces.  
		Truncate? mangle?
	 */
	unsigned char			namespace_policy;

	/**
		Check-prs mode: if true, then doesn't print prs.  
	 */
	bool				check_prs;
	/**
		In wire mode, aliases are accumulated, and printed together.  
		wire(a, b, c...);
	 */
	bool				wire_mode;

	/**
		csim-style production rules.  
	 */
	bool				csim_style_prs;
	/**
		Whether or not prs should be wrapped in dsim { }
	 */
	bool				dsim_prs;
	/**
		Whether or not PRS literal sizes are included in output.
	 */
	bool				size_prs;
	/**
		Enable conductance computation and reporting.  
	 */
	bool				compute_conductances;
	/**
		Enable printing of precharge expressions.  
	 */
	bool				show_precharges;
	/**
		Annotate rules with power supplies.  
		For voltage domain checking.
	 */
	bool				show_supply_nodes;
	/**
		As the hierarchy is traversed wrap output
		in hierarchy delimiters to show where rules come from.
	 */
	bool				show_hierarchy;
	/**
		Print out producers and consumers of channels
		as processes are traversed.
		More meaningful with show_hierarchy=true.
	 */
	bool				show_channel_terminals;
	/**
		Ignore top-level instances and flatten one anonymous
		instance of a named complete process type.  
		Flag to do 'cast2lvs'-like behavior.  
		Tip: protect argument with "quotes" in command-line
			to protect shell-characters.  
	 */
	bool		use_referenced_type_instead_of_top_level;
	/**
		If true, use the non-readable base-94 ASCII charecters for
		unique IDs, otherwise, use a more readable unique <ID> number.  
		Default: true
	 */
	bool				mangled_vcd_ids;
	/**
		The string of the complete process type to process
		in lieu of the top-level instance hierarchy.  
	 */
	std::string			named_process_type;
	/**
		Options to forward to compiler driver.  
	 */
	compile_options			comp_opt;
	/**
		Global printing options.
		Should rarely have to modify these.
	 */
	dump_flags			__dump_flags;
	/**
		For alias pass only, print alternative name map.
	 */
	string				alt_tool_name;
	/**
		e.g. "xspice.x"
	 */
	string				alt_name_prefix;
	/**
		Flags to be used for printing alternate tool name map.
	 */
	dump_flags			alt_dump_flags;
	// ignore policies...
	// warning flags...
	// error flags...

	/**
		Default values in constructor.  
	 */
	cflat_options() : primary_tool(TOOL_NONE), 
		tool_options(TOOL_OPTIONS_DEFAULT), 
		connect_style(CONNECT_STYLE_EQUAL), 
		include_prs(true), dump_self_connect(false), 
		node_attributes(false), 
		split_instance_attributes(false), 
		literal_attributes(false),
		expand_pass_gates(true), 
		enquote_names(true), dump_non_bools(false), 
		namespace_policy(NAMESPACE_POLICY_NONE), 
		check_prs(false), wire_mode(false), 
		csim_style_prs(false), dsim_prs(false), 
		size_prs(false), 
		compute_conductances(false),
		show_precharges(false), 
		show_supply_nodes(false),
		show_hierarchy(false),
		show_channel_terminals(false),
		use_referenced_type_instead_of_top_level(false), 
		mangled_vcd_ids(true),
		named_process_type(), 
		comp_opt(), 
		__dump_flags(dump_flags::no_owners),
		alt_tool_name(),
		alt_dump_flags(dump_flags::no_owners) {
	}

	~cflat_options() { }

	bool
	with_SEU(void) const { return tool_options & TOOL_OPTIONS_SEU; }

	bool
	emit_alt_names(void) const { return alt_tool_name.length(); }

};	// end class cflat::options
// __attribute__(packed) ? (no one cares...)

}	// end namespace HAC

#endif	// __HAC_MAIN_CFLAT_OPTIONS_H__

