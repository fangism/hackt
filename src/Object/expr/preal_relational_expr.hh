/**
	\file "Object/expr/preal_relational_expr.hh"
	Boolean relations between real-valued parameters.  
	$Id: preal_relational_expr.hh,v 1.12 2007/01/21 05:59:07 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_RELATIONAL_EXPR_H__
#define __HAC_OBJECT_EXPR_PREAL_RELATIONAL_EXPR_H__

#include <map>
#include "Object/expr/pbool_expr.hh"
#include "util/memory/count_ptr.hh"
#include "util/string_fwd.hh"
#include "util/operators.hh"

namespace HAC {
namespace entity {
class preal_expr;
class pbool_const;
using std::string;
USING_UTIL_OPERATIONS
using std::map;
using util::memory::count_ptr;

//=============================================================================
/**
	Binary relational expression accepts reals and returns a bool.  
 */
class preal_relational_expr : public pbool_expr {
	typedef	preal_relational_expr			this_type;
	typedef	pbool_expr				parent_type;
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
	typedef	map<string, const op_type*>	op_map_type;
	typedef	map<const op_type*, string>	reverse_op_map_type;
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

	const operand_ptr_type&
	get_first(void) const { return lx; }

	const operand_ptr_type&
	get_second(void) const { return rx; }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	value_type
	static_constant_value(void) const;

	static
	value_type
	evaluate(const string&, const arg_type, const arg_type);

	static
	value_type
	evaluate(const op_type*, const arg_type, const arg_type);

	bool
	must_be_equivalent(const pbool_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const pbool_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const;

	UNROLL_RESOLVE_COPY_PBOOL_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PBOOL_PROTO;
	using parent_type::substitute_default_positional_parameters;

protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class preal_relational_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_RELATIONAL_EXPR_H__

