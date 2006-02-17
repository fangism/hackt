/**
	\file "Object/expr/preal_unary_expr.h"
	Unary negation of meta real value.  
	$Id: preal_unary_expr.h,v 1.2.4.1 2006/02/17 05:07:37 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_UNARY_EXPR_H__
#define __HAC_OBJECT_EXPR_PREAL_UNARY_EXPR_H__

#include "Object/expr/preal_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::memory::count_ptr;

//=============================================================================
/**
	Only possibility: arithmetic negation.
 */
class preal_unary_expr : public preal_expr {
	typedef	preal_unary_expr			this_type;
public:
	typedef	preal_value_type		value_type;
	typedef	char			op_type;
protected:
	const op_type			op;
	/** expression argument must be 0-dimensional */
	count_ptr<const preal_expr>	ex;
private:
	preal_unary_expr();
public:
	preal_unary_expr(const op_type o, const count_ptr<const preal_expr>& e);
	preal_unary_expr(const count_ptr<const preal_expr>& e, const op_type o);

	// default destructor

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

#if ENABLE_STATIC_DIMENSION_ANALYSIS
	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const;
#endif

	bool
	may_be_initialized(void) const { return ex->may_be_initialized(); }

	bool
	must_be_initialized(void) const { return ex->must_be_initialized(); }

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

#if WANT_IS_TEMPLATE_DEPENDENT
	bool
	is_template_dependent(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;
#endif

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const preal_expr& ) const;

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
};	// end class preal_unary_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_UNARY_EXPR_H__

