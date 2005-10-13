/**
	\file "Object/expr/bool_negation_expr.h"
	Class definitions for boolean negation expression.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: bool_negation_expr.h,v 1.2.22.1 2005/10/13 01:27:00 fang Exp $
 */

#ifndef	__OBJECT_EXPR_BOOL_NEGATION_EXPR_H__
#define	__OBJECT_EXPR_BOOL_NEGATION_EXPR_H__

#include "Object/expr/bool_expr.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
using util::memory::count_ptr;
using util::persistent_object_manager;
//=============================================================================
/**
	Boolean data negation.  
 */
class bool_negation_expr : public bool_expr {
	typedef	bool_negation_expr		this_type;
	typedef	bool_expr			parent_type;
public:
	typedef	count_ptr<const bool_expr>	operand_ptr_type;
private:
	operand_ptr_type		ex;
private:
	bool_negation_expr();
public:
	explicit
	bool_negation_expr(const operand_ptr_type&);

	~bool_negation_expr();

	ostream&
	what(ostream& o) const;

#if USE_EXPR_DUMP_CONTEXT
	ostream&
	dump(ostream& o, const expr_dump_context&) const;
#else
	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;
#endif

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class bool_negation_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_BOOL_NEGATION_EXPR_H__

