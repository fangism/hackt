/**
	\file "Object/expr/pbool_unary_expr.h"
	Unary negation of meta boolean.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: pbool_unary_expr.h,v 1.2 2005/07/20 21:00:48 fang Exp $
 */

#ifndef __OBJECT_EXPR_PBOOL_UNARY_EXPR_H__
#define __OBJECT_EXPR_PBOOL_UNARY_EXPR_H__

#include "Object/expr/pbool_expr.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
using util::memory::count_ptr;

//=============================================================================
/**
	Only possibility, NOT-expr.  
	Character may be '~' or '!'.  
 */
class pbool_unary_expr : public pbool_expr {
	typedef	pbool_unary_expr		this_type;
public:
	typedef	pbool_value_type	value_type;
	typedef	char			op_type;
protected:
	const op_type			op;
	/** argument expression must be 0-dimensional */
	count_ptr<const pbool_expr>	ex;
private:
	pbool_unary_expr();
public:
	pbool_unary_expr(const op_type o, const count_ptr<const pbool_expr>& e);
	pbool_unary_expr(const count_ptr<const pbool_expr>& e, const op_type o);

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const;

	bool
	may_be_initialized(void) const { return ex->may_be_initialized(); }

	bool
	must_be_initialized(void) const { return ex->must_be_initialized(); }

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const pbool_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pbool_unary_expr

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_PBOOL_UNARY_EXPR_H__

