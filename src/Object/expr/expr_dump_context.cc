/**
	\file "Object/expr/exp_dump_context.cc"
	$Id: expr_dump_context.cc,v 1.1.2.1 2005/10/13 01:27:02 fang Exp $
 */

#include "Object/expr/expr_dump_context.h"
#include "Object/lang/PRS_base.h"	// for PRS::expr_dump_context

namespace ART {
namespace entity {
//=============================================================================

const expr_dump_context
expr_dump_context::default_value;

const expr_dump_context
expr_dump_context::error_mode(0, NULL, true);

//-----------------------------------------------------------------------------
expr_dump_context::expr_dump_context() : caller_stamp(0), enclosing_scope(), 
	include_type_info(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_dump_context::expr_dump_context(const char cs,
		const scopespace* s, const bool t) :
		caller_stamp(0), enclosing_scope(s), 
		include_type_info(t) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_dump_context::expr_dump_context(const PRS::expr_dump_context& c) :
		caller_stamp(0), enclosing_scope(c.parent_scope), 
		include_type_info(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_dump_context::~expr_dump_context() { }

//=============================================================================
}	// end namespace entity
}	// end namespace ART

