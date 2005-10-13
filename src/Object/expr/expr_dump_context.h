/**
	\file "Object/expr/expr_dump_context.h"
	Flags and modifiers for manipulating expression dumps.  
	$Id: expr_dump_context.h,v 1.1.2.1 2005/10/13 01:27:02 fang Exp $
 */

#ifndef	__OBJECT_EXPR_EXPR_DUMP_CONTEXT_H__
#define	__OBJECT_EXPR_EXPR_DUMP_CONTEXT_H__

#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
class scopespace;
using util::memory::never_ptr;

namespace PRS {
	struct expr_dump_context;
}

//=============================================================================
/**
	Contains contextual information relevant to dumping expressions.  
	Also contains modifier flags.  
 */
struct expr_dump_context {
	/**
		Operator precedence of the caller, 
		for sake of parenthesization.  
		What should default value be?
	 */
	char					caller_stamp;
	/**
		Optional: set to non-null so instance references in
		the same scope don't print with fully qualified names.  
		May make things more readable.  
	 */
	never_ptr<const scopespace>		enclosing_scope;

	/**
		Whether or not type information should be included
		with instance reference dumps.  
	 */
	bool					include_type_info;

	expr_dump_context();

	// default copy-constructor

	// implicit
	expr_dump_context(const PRS::expr_dump_context&);

	expr_dump_context(const char, const scopespace*, 
		const bool);

	~expr_dump_context();

	static const expr_dump_context		default_value;
	static const expr_dump_context		error_mode;
	static const expr_dump_context		brief;

};	// end struct expr_dump_context

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_EXPR_DUMP_CONTEXT_H__

