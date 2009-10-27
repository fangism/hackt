/**
	\file "AST/parse_options.h"
	$Id: parse_options.h,v 1.3 2009/10/27 18:21:44 fang Exp $
 */

#ifndef	__HAC_AST_PARSE_OPTIONS_H__
#define	__HAC_AST_PARSE_OPTIONS_H__

#include "Object/devel_switches.h"
#include "common/status.h"

namespace HAC {
namespace parser {
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

	parse_options() : 
#if REQUIRE_DEFINITION_EXPORT
		export_all(false),
#else
		export_all(true),
#endif
		namespace_instances(true), 
		array_internal_nodes(true),
		case_collision_policy(OPTION_WARN)
		{ }

};	// end class parse_options

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_PARSE_OPTIONS_H__

