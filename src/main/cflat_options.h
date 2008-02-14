/**
	\file "main/cflat_options.h"
	$Id: cflat_options.h,v 1.13.14.1 2008/02/14 04:09:12 fang Exp $
 */

#ifndef	__HAC_MAIN_CFLAT_OPTIONS_H__
#define	__HAC_MAIN_CFLAT_OPTIONS_H__

#include <string>
#include "main/compile_options.h"
#include "Object/devel_switches.h"	// for CFLAT_PATH_CONDUCTANCE

namespace HAC {

/**
	Printing and operating options for cflat.
	Add variations and options as needed.  
 */
class cflat_options {
public:
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
		TOOL_LAYOUT
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
		CONNECT_STYLE_WIRE
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
#if CFLAT_WITH_CONDUCTANCES
	/**
		Enable conductance computation and reporting.  
	 */
	bool				compute_conductances;
#endif

	/**
		Ignore top-level instances and flatten one anonymous
		instance of a named complete process type.  
		Flag to do 'cast2lvs'-like behavior.  
		Tip: protect argument with "quotes" in command-line
			to protect shell-characters.  
	 */
	bool		use_referenced_type_instead_of_top_level;

	/**
		The string of the complete process type to process
		in lieu of the top-level instance hierarchy.  
	 */
	std::string			named_process_type;
	/**
		Options to forward to compiler driver.  
	 */
	compile_options			comp_opt;
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
		enquote_names(true), dump_non_bools(false), 
		namespace_policy(NAMESPACE_POLICY_NONE), 
		check_prs(false), wire_mode(false), 
		csim_style_prs(false), dsim_prs(false), 
		size_prs(false), 
#if CFLAT_WITH_CONDUCTANCES
		compute_conductances(false),
#endif
		use_referenced_type_instead_of_top_level(false), 
		named_process_type(), 
		comp_opt() {
	}

	~cflat_options() { }

	bool
	with_SEU(void) const { return tool_options & TOOL_OPTIONS_SEU; }

};	// end class cflat::options
// __attribute__(packed) ? (no one cares...)

}	// end namespace HAC

#endif	// __HAC_MAIN_CFLAT_OPTIONS_H__

