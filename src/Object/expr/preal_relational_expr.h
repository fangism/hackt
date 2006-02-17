/**
	\file "Object/expr/preal_relational_expr.h"
	Boolean relations between real-valued parameters.  
	$Id: preal_relational_expr.h,v 1.2.4.1 2006/02/17 05:07:37 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_RELATIONAL_EXPR_H__
#define __HAC_OBJECT_EXPR_PREAL_RELATIONAL_EXPR_H__

#include "Object/expr/pbool_expr.h"
#include "util/memory/count_ptr.h"
#include "util/string_fwd.h"
#include "util/qmap.h"
#include "util/operators.h"

namespace HAC {
namespace entity {
class preal_expr;
using std::string;
USING_UTIL_OPERATIONS
using util::qmap;
using util::memory::count_ptr;

//=============================================================================
/**
	Binary relational expression accepts reals and returns a bool.  
 */
class preal_relational_expr : public pbool_expr {
	typedef	preal_relational_expr			this_type;
public:
	typedef	pbool_value_type		value_type;
	typedef	preal_value_type			arg_type;
	typedef	count_ptr<const preal_expr>	operand_ptr_type;
	typedef	binary_relational_operation<value_type, arg_type>
							op_type;
	static const equal_to<value_type, arg_type>	op_equal_to;
	static const not_equal_to<value_type, arg_type>	op_not_equal_to;
	static const less<value_type, arg_type>		op_less;
	static const greater<value_type, arg_type>	op_greater;
	static const less_equal<value_type, arg_type>	op_less_equal;
	static const greater_equal<value_type, arg_type>
							op_greater_equal;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
public:
	static const op_map_type		op_map;
private:
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	operand_ptr_type		lx;
	operand_ptr_type		rx;
	/**
		Points to the operator functor.  
	 */
	const op_type*			op;

private:
	preal_relational_expr();
public:
	preal_relational_expr(const operand_ptr_type& l, const string& o, 
		const operand_ptr_type& r);
	preal_relational_expr(const operand_ptr_type& l, const op_type* o, 
		const operand_ptr_type& r);

	~preal_relational_expr();

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
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

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
};	// end class preal_relational_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_RELATIONAL_EXPR_H__

