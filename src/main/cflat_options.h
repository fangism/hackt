/**
	\file "main/cflat_options.h"
	$Id: cflat_options.h,v 1.6 2006/01/25 20:26:04 fang Exp $
 */

#ifndef	__MAIN_CFLAT_OPTIONS_H__
#define	__MAIN_CFLAT_OPTIONS_H__

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
	 */
	typedef	enum {
		TOOL_NONE = 0,
		TOOL_PRSIM,
		TOOL_PRLINT,
		TOOL_LVS
		// add more tools as they are needed
	}				primary_tool_enum;
	/**
		These enumerations dictate the connection-style
		to print.  
	 */
	typedef	enum {
		CONNECT_STYLE_NONE = 0,
		CONNECT_STYLE_CONNECT,
		CONNECT_STYLE_EQUAL,
		CONNECT_STYLE_WIRE
	}				connect_style_enum;
public:
	unsigned char			primary_tool;
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

	// ignore policies...
	// warning flags...
	// error flags...

	/**
		Default values in constructor.  
	 */
	cflat_options() : primary_tool(TOOL_NONE), 
		connect_style(CONNECT_STYLE_EQUAL), 
		include_prs(true), dump_self_connect(false), 
		enquote_names(true), dump_non_bools(false), 
		namespace_policy(NAMESPACE_POLICY_NONE), 
		check_prs(false), wire_mode(false), 
		csim_style_prs(false), dsim_prs(false) {
	}

	~cflat_options() { }

};	// end class cflat::options
// __attribute__(packed) ? (no one cares...)

}	// end namespace HAC

#endif	// __MAIN_CFLAT_OPTIONS_H__

