/**
	\file "AST/parse_options.h"
	$Id: parse_options.h,v 1.1.2.1 2007/09/27 16:14:23 fang Exp $
 */

#ifndef	__HAC_AST_PARSE_OPTIONS_H__
#define	__HAC_AST_PARSE_OPTIONS_H__

#include "Object/devel_switches.h"

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
		TODO: consider variants export-strict, export-all, export-warn.
		Default false.
	 */
	bool		export_all;

	parse_options() : 
#if REQUIRE_DEFINITION_EXPORT
		export_all(false)
#else
		export_all(true)
#endif
		 { }

};	// end class parse_options

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_PARSE_OPTIONS_H__

