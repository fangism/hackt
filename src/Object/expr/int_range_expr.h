/**
	\file "Object/expr/int_range_expr.h"
	Class definitions for integer range expressions.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_range_expr.h,v 1.3 2005/10/25 20:51:51 fang Exp $
 */

#ifndef	__OBJECT_EXPR_INT_RANGE_EXPR_H__
#define	__OBJECT_EXPR_INT_RANGE_EXPR_H__

#include "Object/expr/nonmeta_range_expr_base.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class int_expr;
using util::memory::count_ptr;
using util::persistent_object_manager;
//=============================================================================
/**
	In the CHP context, we allow indices to reference both
	int_meta_instance_references and pint_meta_instance_references.  
	Should we allow (param) index expressions where we allow
	data index expressions?
 */
class int_range_expr : public nonmeta_range_expr_base {
	typedef	int_range_expr			this_type;
	typedef	nonmeta_range_expr_base		parent_type;
public:
	typedef	count_ptr<const int_expr>	bound_ptr_type;
protected:
	bound_ptr_type				lower;
	bound_ptr_type				upper;
public:
	int_range_expr();
	int_range_expr(const bound_ptr_type&, const bound_ptr_type&);
	~int_range_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }	// or bomb

#if 0
	GET_DATA_TYPE_REF_PROTO;
#endif

	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_index

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_INT_RANGE_EXPR_H__

