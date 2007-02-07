/**
	\file "Object/expr/loop_nonmeta_expr.h"
	$Id: loop_nonmeta_expr.h,v 1.1.2.1 2007/02/07 22:44:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_LOOP_NONMETA_EXPR_H__
#define	__HAC_OBJECT_EXPR_LOOP_NONMETA_EXPR_H__

#include "Object/unroll/meta_loop_base.h"
#include "Object/expr/data_expr.h"
#include "Object/expr/expr_fwd.h"
#include "util/boolean_types.h"
#include "util/persistent.h"

namespace HAC {
namespace entity {
class template_formals_manager;
class unroll_context;
class dynamic_param_expr_list;
struct expr_dump_context;
class nonmeta_expr_visitor;
using util::good_bool;

//=============================================================================
/**
	The pattern for this class is based on its non-loop binary
	operator counterpart.  
 */
template <class E>
class loop_nonmeta_expr : 
		public meta_loop_base, 
		public E::parent_type {
	typedef	loop_nonmeta_expr<E>		this_type;
	typedef	E				binary_expr_type;
	typedef	typename binary_expr_type::parent_type	parent_type;
public:
	typedef	parent_type				expr_base_type;
	typedef	typename parent_type::const_expr_type	const_expr_type;
	typedef	typename binary_expr_type::arg_type	arg_type;
	typedef	typename binary_expr_type::value_type	value_type;
	typedef	typename binary_expr_type::operand_ptr_type
							operand_ptr_type;
	typedef	typename binary_expr_type::op_type	op_type;
	typedef	typename binary_expr_type::op_key_type	op_key_type;
	typedef	meta_loop_base::ind_var_ptr_type	ind_var_ptr_type;
	typedef	meta_loop_base::range_ptr_type		range_ptr_type;
private:
	operand_ptr_type				ex;
	const op_type*					op;
private:
	loop_nonmeta_expr();
public:
	loop_nonmeta_expr(const ind_var_ptr_type&, const range_ptr_type&, 
		const operand_ptr_type&, const op_type*);
	~loop_nonmeta_expr();


	const operand_ptr_type&
	get_operand(void) const { return this->ex; }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	count_ptr<const parent_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const parent_type>&) const;

	count_ptr<const const_param>
	nonmeta_resolve_copy(const nonmeta_context_base&, 
		const count_ptr<const parent_type>&) const;

	count_ptr<const const_expr_type>
	__nonmeta_resolve_rvalue(const nonmeta_context_base&, 
		const count_ptr<const parent_type>&) const;

	void
	accept(nonmeta_expr_visitor&) const;

protected:
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class loop_nonmeta_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_LOOP_NONMETA_EXPR_H__

