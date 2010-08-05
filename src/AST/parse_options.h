/**
	\file "AST/parse_options.h"
	$Id: parse_options.h,v 1.5 2010/08/05 18:25:23 fang Exp $
 */

#ifndef	__HAC_AST_PARSE_OPTIONS_H__
#define	__HAC_AST_PARSE_OPTIONS_H__

#include <iosfwd>
#include "Object/devel_switches.h"
#include "common/status.h"

namespace HAC {
namespace parser {
using std::istream;
using std::ostream;
//=============================================================================
/**
	User-controllable switches for tuning compilation.  
 */
struct parse_options {
	/**
		If set to true, all definitions are treated as exported, 
		for backwards HAC compatibility.  
		This flag however does NOT affect the lookup procedure.  
		TODO: consider variants export-strict, export-all, export-warn.
		Default false.  (ACT-mode: false)
	 */
	bool		export_all;
	/**
		If true, allow instance-management statements 
		in subnamespace from the global namespace.  
		Default true.  (ACT-mode: false)
	 */
	bool		namespace_instances;
	/**
		Set to true to allow implicit arrays of internal nodes.  
		Default true  (ACT-mode: false).
	 */
	bool		array_internal_nodes;
	/**
		Whether or not to check for case collisions in scopes, 
		and whether or not to signal an error.
		Default: warn
	 */
	error_policy	case_collision_policy;
	/**
		What to about unknown spec directives.
	 */
	error_policy	unknown_spec_policy;

	/// default ctor with default values
	parse_options() : 
#if REQUIRE_DEFINITION_EXPORT
		export_all(false),
#else
		export_all(true),
#endif
		namespace_instances(true), 
		array_internal_nodes(true),
		case_collision_policy(OPTION_WARN),
		unknown_spec_policy(OPTION_ERROR)
		{ }

	ostream&
	dump(ostream&) const;

	void
	write_object(ostream&) const;

	void
	load_object(istream&);

};	// end class parse_options

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_PARSE_OPTIONS_H__

