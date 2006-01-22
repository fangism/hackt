/**
	\file "Object/expr/int_negation_expr.h"
	Class definitions for integer negation expression.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_negation_expr.h,v 1.5 2006/01/22 18:19:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_INT_NEGATION_EXPR_H__
#define	__HAC_OBJECT_EXPR_INT_NEGATION_EXPR_H__

#include "Object/expr/int_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::persistent_object_manager;
using util::memory::count_ptr;
//=============================================================================
/**
	Signed integer data negation.  
 */
class int_negation_expr : public int_expr {
	typedef	int_negation_expr		this_type;
	typedef	int_expr			parent_type;
public:
	typedef	count_ptr<const int_expr>	operand_ptr_type;
private:
	operand_ptr_type		ex;
private:
	int_negation_expr();
public:
	explicit
	int_negation_expr(const operand_ptr_type&);

	~int_negation_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_negation_expr

//=============================================================================
// what about ! int -> bool ?
//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_INT_NEGATION_EXPR_H__

