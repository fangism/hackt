/**
	\file "main/cflat_options.h"
	$Id: cflat_options.h,v 1.2.6.1 2005/11/01 04:23:57 fang Exp $
 */

#ifndef	__MAIN_CFLAT_OPTIONS_H__
#define	__MAIN_CFLAT_OPTIONS_H__

namespace ART {

/**
	Printing and operating options for cflat.
	Add variations and options as needed.  
 */
class cflat_options {
public:
	typedef	enum {
		CONNECT_STYLE_NONE = 0,
		CONNECT_STYLE_CONNECT = 1,
		CONNECT_STYLE_EQUAL = 2,
		CONNECT_STYLE_WIRE = 3
	}				connect_style_enum;
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
		NAMESPACE_POLICY_NONE = 0, 
		NAMESPACE_POLICY_TRUNCATE = 1, 
		NAMESPACE_POLICY_MANGLE = 2,
		NAMESPACE_POLICY_OTHER = 3
	}				namespace_policy_enum;
	/**
		How to deal with namespaces.  
		Truncate? mangle?
	 */
	unsigned char			namespace_policy;

	// ignore policies...
	// warning flags...
	// error flags...

	/**
		Default values in constructor.  
	 */
	cflat_options() : connect_style(CONNECT_STYLE_EQUAL), 
		include_prs(true), dump_self_connect(false), 
		enquote_names(true), dump_non_bools(false), 
		namespace_policy(NAMESPACE_POLICY_NONE) {
	}

	~cflat_options() { }

};	// end class cflat::options
// __attribute__(packed) ? (no one cares...)

}	// end namespace ART

#endif	// __MAIN_CFLAT_OPTIONS_H__

