/**
	\file "Object/expr/operators.cc"
	Meta parameter operator expressions.  
	TODO: there is so much code duplication in here...
		This NEEDS to be templated somehow...
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: operators.cc,v 1.25 2011/04/02 01:45:59 fang Exp $
 */

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/expr/pint_arith_expr.h"
#include "Object/expr/pint_relational_expr.h"
#include "Object/expr/pint_unary_expr.h"
#include "Object/expr/pbool_logical_expr.h"
#include "Object/expr/pbool_unary_expr.h"
#include "Object/expr/preal_arith_expr.h"
#include "Object/expr/preal_relational_expr.h"
#include "Object/expr/preal_unary_expr.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/const_range.h"
#include "Object/expr/loop_meta_expr.tcc"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/expr/convert_expr.tcc"
#include "Object/expr/operator_precedence.h"
#include "Object/expr/param_defined.h"
#include "Object/traits/preal_traits.h"
#include "Object/persistent_type_hash.h"
#include "Object/ref/meta_value_reference_base.h"
#include "common/TODO.h"

#include "util/numeric/sign_traits.h"
#include "util/stacktrace.h"
#include "util/qmap.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::pint_unary_expr,
		"pint-unary-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_unary_expr,
		"pbool-unary-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::pint_arith_expr, 
		"pint-arith-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::pint_relational_expr, 
		"pint-relational-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_logical_expr, 
		"logical-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_unary_expr,
		"preal-unary-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_arith_expr, 
		"preal-arith-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_relational_expr, 
		"preal-relational-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::pint_arith_loop_expr, 
		"pint-arith-loop-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_logical_loop_expr, 
		"logical-loop-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_arith_loop_expr, 
		"preal-arith-loop-expr")
SPECIALIZE_UTIL_WHAT(HAC::entity::convert_pint_to_preal_expr, 
		"convert-pint-to-preal")
SPECIALIZE_UTIL_WHAT(HAC::entity::param_defined,
		"param-defined")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_unary_expr, PINT_UNARY_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pbool_unary_expr, PBOOL_UNARY_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_arith_expr, PINT_ARITH_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_relational_expr, PINT_RELATIONAL_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pbool_logical_expr, PBOOL_LOGICAL_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_unary_expr, PREAL_UNARY_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_arith_expr, PREAL_ARITH_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_relational_expr, PREAL_RELATIONAL_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_arith_loop_expr, PINT_ARITH_LOOP_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pbool_logical_loop_expr, PBOOL_LOGICAL_LOOP_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_arith_loop_expr, PREAL_ARITH_LOOP_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::convert_pint_to_preal_expr,
	CONVERT_PINT_TO_PREAL_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::param_defined, PARAM_DEFINED_EXPR_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
using util::persistent_traits;
using util::persistent_object_manager;
using std::istream;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::numeric::signed_type;
using util::numeric::unsigned_type;

//=============================================================================
// class pint_unary_expr method definitions

/**
	Private empty constructor.  
 */
pint_unary_expr::pint_unary_expr() :
		pint_expr(), op('\0'), ex(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_unary_expr::pint_unary_expr(
		const op_type o, const operand_ptr_type& e) :
		pint_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_unary_expr::pint_unary_expr(
		const operand_ptr_type& e, const op_type o) :
		pint_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_unary_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_unary_expr::dump(ostream& o, const expr_dump_context& c) const {
	// parentheses? check operator precedence
	const bool p = c.need_parentheses(OP_PREC_UNARY);
	const expr_dump_context::stamp_modifier m(c, OP_PREC_UNARY);
	if (p) o << '(';
	ex->dump(o << op, c);
	if (p) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_unary_expr::is_static_constant(void) const {
	return ex->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_unary_expr::is_relaxed_formal_dependent(void) const {
	return ex->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: we use unsigned bitwise operations.
 */
pint_unary_expr::value_type
pint_unary_expr::evaluate(const op_type op, const value_type tmp) {
	typedef	unsigned_type<value_type>::type		unsigned_value_type;
	return (op == '-') ? -tmp : value_type(~unsigned_value_type(tmp));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_unary_expr::value_type
pint_unary_expr::static_constant_value(void) const {
	// depends on op (only two possibilities)
	const value_type tmp = ex->static_constant_value();
	return evaluate(op, tmp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_unary_expr::must_be_equivalent(const pint_expr& p) const {
	const this_type* const ue = IS_A(const this_type*, &p);
	if (ue) {
		return op == ue->op && ex->must_be_equivalent(*ue->ex);
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_unary_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if succssfully resolved.
 */
good_bool
pint_unary_expr::unroll_resolve_value(const unroll_context& c, 
		value_type& i) const {
	value_type j;
	NEVER_NULL(ex);
	const good_bool ret(ex->unroll_resolve_value(c, j));
	i = (op == '-') ? -j : ~j;		// regardless of ret
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty list, signifying a scalar value, because all 
		expressions operations only work on scalars.  
 */
const_index_list
pint_unary_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The only unary pint operation is negation.  
 */
count_ptr<const pint_const>
pint_unary_expr::__unroll_resolve_rvalue(const unroll_context& c,
		const count_ptr<const pint_expr>& p) const {
	typedef	count_ptr<const pint_const>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	INVARIANT(p == this);
	const return_type ret(ex->__unroll_resolve_rvalue(c, ex));
	if (ret) {
#if 0
		count_ptr<pint_const> pc(ret.is_a<pint_const>());
		INVARIANT(pc);	// NOT TRUE, could be scalar const_collection
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(
			new pint_const(- pc->static_constant_value()));
#else
		const value_type tmp = ret->static_constant_value();
		return return_type(
			new pint_const((op == '-') ? -tmp : ~tmp));
#endif
	} else {
		// there is an error
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pint_unary_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const pint_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_expr>
pint_unary_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const pint_expr>
pint_unary_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pint_expr>& p) const {
	typedef	count_ptr<const pint_expr>		return_type;
	INVARIANT(p == this);
	const return_type
		rex(ex->substitute_default_positional_parameters(f, e, ex));
	if (rex) {
		if (rex == ex) {
			return p;
		} else {
			return return_type(new this_type(rex, op));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_unary_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_unary_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, op);
	m.write_pointer(f, ex);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_unary_expr::load_object(const persistent_object_manager& m, 
		istream& f) {
	read_value(f, const_cast<op_type&>(op));
	m.read_pointer(f, ex);
}

//=============================================================================
// class preal_unary_expr method definitions

/**
	Private empty constructor.  
 */
preal_unary_expr::preal_unary_expr() :
		preal_expr(), op('\0'), ex(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_unary_expr::preal_unary_expr(
		const op_type o, const operand_ptr_type& e) :
		preal_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_unary_expr::preal_unary_expr(
		const operand_ptr_type& e, const op_type o) :
		preal_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(preal_unary_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
preal_unary_expr::dump(ostream& o, const expr_dump_context& c) const {
	// parentheses? check operator precedence
	const bool p = c.need_parentheses(OP_PREC_UNARY);
	const expr_dump_context::stamp_modifier m(c, OP_PREC_UNARY);
	if (p) o << '(';
	ex->dump(o << op, c);
	if (p) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_unary_expr::is_static_constant(void) const {
	return ex->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_unary_expr::is_relaxed_formal_dependent(void) const {
	return ex->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_unary_expr::value_type
preal_unary_expr::static_constant_value(void) const {
	// depends on op
	return - ex->static_constant_value();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_unary_expr::value_type
preal_unary_expr::evaluate(const op_type op, const value_type v) {
	INVARIANT(op == '-');
	return -v;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_unary_expr::must_be_equivalent(const preal_expr& p) const {
	const this_type* const ue = IS_A(const this_type*, &p);
	if (ue) {
		return op == ue->op && ex->must_be_equivalent(*ue->ex);
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_unary_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if succssfully resolved.
 */
good_bool
preal_unary_expr::unroll_resolve_value(const unroll_context& c, 
		value_type& i) const {
	value_type j;
	NEVER_NULL(ex);
	const good_bool ret(ex->unroll_resolve_value(c, j));
	i = -j;		// regardless of ret
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty list, signifying a scalar value, because all 
		expressions operations only work on scalars.  
 */
const_index_list
preal_unary_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The only unary preal operation is negation.  
 */
count_ptr<const preal_const>
preal_unary_expr::__unroll_resolve_rvalue(const unroll_context& c,
		const count_ptr<const preal_expr>& p) const {
	typedef	count_ptr<const preal_const>		return_type;
	// should return a preal_const
	// maybe make a preal_const version to avoid casting
	INVARIANT(p == this);
	const return_type ret(ex->__unroll_resolve_rvalue(c, ex));
	if (ret) {
		return return_type(
			new preal_const(-ret->static_constant_value()));
	} else {
		// there is an error
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
preal_unary_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const preal_expr>& p
		) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const preal_expr>
preal_unary_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const preal_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const preal_expr>
preal_unary_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const preal_expr>& p) const {
	typedef	count_ptr<const preal_expr>		return_type;
	INVARIANT(p == this);
	const return_type
		rex(ex->substitute_default_positional_parameters(f, e, ex));
	if (rex) {
		if (rex == ex) {
			return p;
		} else {
			return return_type(new this_type(rex, op));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_unary_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_unary_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, op);
	m.write_pointer(f, ex);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_unary_expr::load_object(const persistent_object_manager& m, 
		istream& f) {
	read_value(f, const_cast<op_type&>(op));
	m.read_pointer(f, ex);
}

//=============================================================================
// class pbool_unary_expr method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pbool_unary_expr::pbool_unary_expr() :
		pbool_expr(), op('\0'), ex(NULL) {
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_unary_expr::pbool_unary_expr(
		const op_type o, const operand_ptr_type& e) :
		pbool_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_unary_expr::pbool_unary_expr(
		const operand_ptr_type& e, const op_type o) :
		pbool_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pbool_unary_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_unary_expr::dump(ostream& o, const expr_dump_context& c) const {
	const bool p = c.need_parentheses(OP_PREC_UNARY);
	const expr_dump_context::stamp_modifier m(c, OP_PREC_UNARY);
	if (p) o << '(';
	ex->dump(o << op, c);
	if (p) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::is_static_constant(void) const {
	return ex->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::is_relaxed_formal_dependent(void) const {
	return ex->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_unary_expr::value_type
pbool_unary_expr::static_constant_value(void) const {
	return evaluate(op, ex->static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_unary_expr::value_type
pbool_unary_expr::evaluate(const op_type op, const value_type v) {
	INVARIANT(op == '~');
	return !v;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::must_be_equivalent(const pbool_expr& b) const {
	const this_type* const be = IS_A(const this_type*, &b);
	if (be) {
		return ex->must_be_equivalent(*be->ex);
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
pbool_unary_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_unary_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pbool_unary_expr::unroll_resolve_value(const unroll_context& c, 
		value_type& i) const {
	value_type j;
	NEVER_NULL(ex);
	const good_bool ret(ex->unroll_resolve_value(c, j));
	i = !j;		// regardless of ret
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The only unary pbool operation is logical negation.  
 */
count_ptr<const pbool_const>
pbool_unary_expr::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_const>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	INVARIANT(p == this);
	const return_type ret(ex->__unroll_resolve_rvalue(c, ex));
	if (ret) {
#if 0
		count_ptr<pbool_const> pc(ret.is_a<pbool_const>());
		INVARIANT(pc);	// NOT TRUE: could be scalar const_collection
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(
			new pbool_const(!pc->static_constant_value()));
#else
		return return_type(
			new pbool_const(!ret->static_constant_value()));
#endif
	} else {
		// there is an error
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pbool_unary_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
pbool_unary_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const pbool_expr>
pbool_unary_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_expr>		return_type;
	INVARIANT(p == this);
	const return_type
		rex(ex->substitute_default_positional_parameters(f, e, ex));
	if (rex) {
		if (rex == ex) {
			return p;
		} else {
			return return_type(new this_type(rex, op));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_unary_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_unary_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, op);
	m.write_pointer(f, ex);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_unary_expr::load_object(const persistent_object_manager& m, 
		istream& f) {
	read_value(f, const_cast<op_type&>(op));
	m.read_pointer(f, ex);
}

//=============================================================================
// class pint_arith_expr method definitions

// static member initializations (order matters!)

const plus<pint_value_type, pint_value_type>		pint_arith_expr::adder;
const minus<pint_value_type, pint_value_type>		pint_arith_expr::subtractor;
const multiplies<pint_value_type, pint_value_type>	pint_arith_expr::multiplier;
const divides<pint_value_type, pint_value_type>		pint_arith_expr::divider;
const modulus<pint_value_type, pint_value_type>		pint_arith_expr::remainder;
const shift_left<pint_value_type, pint_value_type>	pint_arith_expr::doubler;
const shift_right<pint_value_type, pint_value_type>	pint_arith_expr::halver;
const bitwise_and<pint_value_type, pint_value_type>	pint_arith_expr::masker;
const bitwise_or<pint_value_type, pint_value_type>	pint_arith_expr::unmasker;
const bitwise_xor<pint_value_type, pint_value_type>	pint_arith_expr::hasher;
// and Donner and Blitzen, and Comet and Cupid, ...

const pint_arith_expr::op_map_type
pint_arith_expr::op_map;

const pint_arith_expr::reverse_op_map_type
pint_arith_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
pint_arith_expr::op_map_size = pint_arith_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
pint_arith_expr::op_map_register(const char c, const op_type* o, const char p) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[c] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = op_info(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered arithmetic operators.  
 */
size_t
pint_arith_expr::op_map_init(void) {
	op_map_register('+', &adder, OP_PREC_PLUS);
	op_map_register('-', &subtractor, OP_PREC_PLUS);
	op_map_register('*', &multiplier, OP_PREC_TIMES);
	op_map_register('/', &divider, OP_PREC_TIMES);
	op_map_register('%', &remainder, OP_PREC_TIMES);
	// short for "<<"
	op_map_register('<', &doubler, OP_PREC_SHIFT);
	// short for ">>"
	op_map_register('>', &halver, OP_PREC_SHIFT);
	op_map_register('&', &masker, OP_PREC_BITWISE_AND);
	op_map_register('^', &hasher, OP_PREC_BITWISE_XOR);
	op_map_register('|', &unmasker, OP_PREC_BITWISE_OR);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
	Default to adder (bogus), set op later during load.
 */
pint_arith_expr::pint_arith_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_arith_expr::~pint_arith_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_arith_expr::pint_arith_expr(const operand_ptr_type& l, const char o,
		const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_arith_expr::pint_arith_expr(const operand_ptr_type& l, const op_type* o,
		const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_arith_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: have op include precedence information.  
 */
ostream&
pint_arith_expr::dump(ostream& o, const expr_dump_context& c) const {
#if 1
	const op_info& oi(reverse_op_map[op]);
	const bool a = op->is_associative();
	const bool p = c.need_parentheses(oi.prec, a);
	const expr_dump_context::stamp_modifier m(c, oi.prec, a);
	if (p) o << '(';
	lx->dump(o, c);
	if (oi.op == '<' || oi.op == '>') {
		o << oi.op;		// for "<<" and ">>", how cute...
	}
	o << oi.op;
	rx->dump(o, c);
	if (p) o << ')';
	return o;
#else
	return rx->dump(lx->dump(o, c) << reverse_op_map[op].op, c);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_arith_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_arith_expr::is_relaxed_formal_dependent(void) const {
	return lx->is_relaxed_formal_dependent() ||
		rx->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_arith_expr::value_type
pint_arith_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	// Oooooh, virtual operator dispatch!
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_arith_expr::value_type
pint_arith_expr::evaluate(const op_key_type o,
		const value_type l, const value_type r) {
	const op_type* op(op_map[o]);
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_arith_expr::value_type
pint_arith_expr::evaluate(const op_type* op,
		const value_type l, const value_type r) {
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_arith_expr::must_be_equivalent(const pint_expr& p) const {
	const this_type* const ae = IS_A(const this_type*, &p);
	if (ae) {
		// for now structural equivalence only,
		return (op == ae->op) &&
			lx->must_be_equivalent(*ae->lx) &&
			rx->must_be_equivalent(*ae->rx);
		// later, symbolic equivalence, Ooooh!
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty list, signifying a scalar value, because all 
		expressions operations only work on scalars.  
 */
const_index_list
pint_arith_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_arith_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if resolved.
 */
good_bool
pint_arith_expr::unroll_resolve_value(const unroll_context& c,
		value_type& i) const {
	static const expr_dump_context& dc(expr_dump_context::default_value);
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	value_type lval, rval;
	const good_bool lex(lx->unroll_resolve_value(c, lval));
	const good_bool rex(rx->unroll_resolve_value(c, rval));
	if (!lex.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else if (!rex.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else {
		i = (*op)(lval, rval);
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pint_const of the resolved value.
 */
count_ptr<const pint_const>
pint_arith_expr::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	typedef	count_ptr<const pint_const>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	INVARIANT(p == this);
	const return_type lex(lx->__unroll_resolve_rvalue(c, lx));
	const return_type rex(rx->__unroll_resolve_rvalue(c, rx));
	if (lex && rex) {
		// actually could be scalar const_collection!
#if 0
		const count_ptr<pint_const> lpc(lex.is_a<pint_const>());
		const count_ptr<pint_const> rpc(rex.is_a<pint_const>());
		INVARIANT(lpc);
		INVARIANT(rpc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		const pint_value_type lop = lpc->static_constant_value();
		const pint_value_type rop = rpc->static_constant_value();
#else
		// TODO: need a specialized unroll_resolve_rvalues for the value_type
		// for now, just assert the type
		const pint_value_type lop = lex->static_constant_value();
		const pint_value_type rop = rex->static_constant_value();
#endif
		return return_type(new pint_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pint_arith_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_expr>
pint_arith_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const pint_expr>
pint_arith_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pint_expr>& p) const {
	typedef	count_ptr<const pint_expr>		return_type;
	INVARIANT(p == this);
	const return_type
		rlx(lx->substitute_default_positional_parameters(f, e, lx)),
		rrx(rx->substitute_default_positional_parameters(f, e, rx));
	if (rlx && rrx) {
		if (rlx == lx && rrx == rx) {
			return p;
		} else {
			return return_type(new this_type(rlx, op, rrx));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_arith_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_arith_expr::write_object(const persistent_object_manager& m,
		ostream& f) const {
	write_value(f, reverse_op_map[op].op);	// writes a character
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_arith_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	char o;
	read_value(f, o);
	op = op_map[o];
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class pint_relational_expr method definitions

// static member initializations (order matters!)

const equal_to<pbool_value_type, pint_value_type>
pint_relational_expr::op_equal_to;

const not_equal_to<pbool_value_type, pint_value_type>
pint_relational_expr::op_not_equal_to;

const less<pbool_value_type, pint_value_type>
pint_relational_expr::op_less;

const greater<pbool_value_type, pint_value_type>
pint_relational_expr::op_greater;

const less_equal<pbool_value_type, pint_value_type>
pint_relational_expr::op_less_equal;

const greater_equal<pbool_value_type, pint_value_type>
pint_relational_expr::op_greater_equal;

const pint_relational_expr::op_map_type
pint_relational_expr::op_map;

const pint_relational_expr::reverse_op_map_type
pint_relational_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
pint_relational_expr::op_map_size = pint_relational_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
pint_relational_expr::op_map_register(const string& s, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[s] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered relationalmetic operators.  
 */
size_t
pint_relational_expr::op_map_init(void) {
	op_map_register("==", &op_equal_to);
	op_map_register("!=", &op_not_equal_to);
	op_map_register("<", &op_less);
	op_map_register(">", &op_greater);
	op_map_register("<=", &op_less_equal);
	op_map_register(">=", &op_greater_equal);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
	Note: pass string, not null char.  
 */
pint_relational_expr::pint_relational_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_relational_expr::~pint_relational_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_relational_expr::pint_relational_expr(const operand_ptr_type& l,
		const string& o, const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_relational_expr::pint_relational_expr(const operand_ptr_type& l,
		const op_type* o, const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_relational_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_relational_expr::dump(ostream& o, const expr_dump_context& c) const {
	return rx->dump(lx->dump(o, c) << reverse_op_map[op], c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_relational_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_relational_expr::is_relaxed_formal_dependent(void) const {
	return lx->is_relaxed_formal_dependent() ||
		rx->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return result of resolved comparison.  
 */
pint_relational_expr::value_type
pint_relational_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_relational_expr::value_type
pint_relational_expr::evaluate(const string& o, const value_type l, 
		const value_type r) {
	const op_type* const op(op_map[o]);
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_relational_expr::value_type
pint_relational_expr::evaluate(const op_type* op, const value_type l, 
		const value_type r) {
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_relational_expr::must_be_equivalent(const pbool_expr& b) const {
	const this_type* const
		re = IS_A(const this_type*, &b);
	if (re) {
		return (op == re->op) &&
			lx->must_be_equivalent(*re->lx) &&
			rx->must_be_equivalent(*re->rx);
		// this is also conservative, 
		// doesn't check symbolic equivalence... yet
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
pint_relational_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_relational_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pint_relational_expr::unroll_resolve_value(const unroll_context& c,
		value_type& i) const {
	static const expr_dump_context& dc(expr_dump_context::default_value);
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	pint_value_type lval, rval;
	const good_bool lex(lx->unroll_resolve_value(c, lval));
	const good_bool rex(rx->unroll_resolve_value(c, rval));
	if (!lex.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else if (!rex.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else {
		i = (*op)(lval, rval);
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pbool_const of the resolved value.
 */
count_ptr<const pbool_const>
pint_relational_expr::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_const>		return_type;
	typedef	count_ptr<const pint_const>		leaf_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	INVARIANT(p == this);
	const leaf_type lex(lx->__unroll_resolve_rvalue(c, lx));
	const leaf_type rex(rx->__unroll_resolve_rvalue(c, rx));
	if (lex && rex) {
#if 0
		const count_ptr<pint_const> lpc(lex.is_a<pint_const>());
		const count_ptr<pint_const> rpc(rex.is_a<pint_const>());
		// NOT TRUE, could be scalar const_collection
		INVARIANT(lpc);	
		INVARIANT(rpc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		const pint_value_type lop = lpc->static_constant_value();
		const pint_value_type rop = rpc->static_constant_value();
#else
		const pint_value_type lop = lex->static_constant_value();
		const pint_value_type rop = rex->static_constant_value();
#endif
		return return_type(new pbool_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pint_relational_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
pint_relational_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const pbool_expr>
pint_relational_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_expr>		return_type;
	typedef	count_ptr<const pint_expr>		operand_type;
	INVARIANT(p == this);
	const operand_type
		rlx(lx->substitute_default_positional_parameters(f, e, lx)),
		rrx(rx->substitute_default_positional_parameters(f, e, rx));
	if (rlx && rrx) {
		if (rlx == lx && rrx == rx) {
			return p;
		} else {
			return return_type(new this_type(rlx, op, rrx));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_relational_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_relational_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_relational_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	string s;
	read_value(f, s);
	op = op_map[s];
	NEVER_NULL(op);
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class preal_arith_expr method definitions

// static member initializations (order matters!)

const plus<preal_value_type, preal_value_type>		preal_arith_expr::adder;
const minus<preal_value_type, preal_value_type>		preal_arith_expr::subtractor;
const multiplies<preal_value_type, preal_value_type>	preal_arith_expr::multiplier;
const divides<preal_value_type, preal_value_type>	preal_arith_expr::divider;
const modulus<preal_value_type, preal_value_type>	preal_arith_expr::remainder;

const preal_arith_expr::op_map_type
preal_arith_expr::op_map;

const preal_arith_expr::reverse_op_map_type
preal_arith_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
preal_arith_expr::op_map_size = preal_arith_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
preal_arith_expr::op_map_register(const char c, const op_type* o, const char p) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[c] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = op_info(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered arithmetic operators.  
 */
size_t
preal_arith_expr::op_map_init(void) {
	op_map_register('+', &adder, OP_PREC_PLUS);
	op_map_register('-', &subtractor, OP_PREC_PLUS);
	op_map_register('*', &multiplier, OP_PREC_TIMES);
	op_map_register('/', &divider, OP_PREC_TIMES);
	op_map_register('%', &remainder, OP_PREC_TIMES);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
	Default to adder (bogus), set op later during load.
 */
preal_arith_expr::preal_arith_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_arith_expr::~preal_arith_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_arith_expr::preal_arith_expr(const operand_ptr_type& l, const op_type* o,
		const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_arith_expr::preal_arith_expr(const operand_ptr_type& l, const char o,
		const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(preal_arith_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Smart printing.  
 */
ostream&
preal_arith_expr::dump(ostream& o, const expr_dump_context& c) const {
	const op_info& oi(reverse_op_map[op]);
	const bool a = op->is_associative();
	const bool p = c.need_parentheses(oi.prec, a);
	const expr_dump_context::stamp_modifier m(c, oi.prec, a);
	if (p) o << '(';
	rx->dump(lx->dump(o, c) << oi.op, c);
	if (p) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_arith_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_arith_expr::is_relaxed_formal_dependent(void) const {
	return lx->is_relaxed_formal_dependent() ||
		rx->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_arith_expr::value_type
preal_arith_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	// Oooooh, virtual operator dispatch!
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_arith_expr::value_type
preal_arith_expr::evaluate(const op_key_type o,
		const value_type l, const value_type r) {
	const op_type* op(op_map[o]);
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_arith_expr::value_type
preal_arith_expr::evaluate(const op_type* op,
		const value_type l, const value_type r) {
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_arith_expr::must_be_equivalent(const preal_expr& p) const {
	const this_type* const ae = IS_A(const this_type*, &p);
	if (ae) {
		// for now structural equivalence only,
		return (op == ae->op) &&
			lx->must_be_equivalent(*ae->lx) &&
			rx->must_be_equivalent(*ae->rx);
		// later, symbolic equivalence, Ooooh!
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty list, signifying a scalar value, because all 
		expressions operations only work on scalars.  
 */
const_index_list
preal_arith_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_arith_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if resolved.
 */
good_bool
preal_arith_expr::unroll_resolve_value(const unroll_context& c,
		value_type& i) const {
	static const expr_dump_context& dc(expr_dump_context::default_value);
	// should return a preal_const
	// maybe make a preal_const version to avoid casting
	value_type lval, rval;
	const good_bool lex(lx->unroll_resolve_value(c, lval));
	const good_bool rex(rx->unroll_resolve_value(c, rval));
	if (!lex.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else if (!rex.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else {
		i = (*op)(lval, rval);
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return preal_const of the resolved value.
 */
count_ptr<const preal_const>
preal_arith_expr::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const preal_expr>& p) const {
	typedef	count_ptr<const preal_const>		return_type;
	// should return a preal_const
	// maybe make a preal_const version to avoid casting
	INVARIANT(p == this);
	const return_type lex(lx->__unroll_resolve_rvalue(c, lx));
	const return_type rex(rx->__unroll_resolve_rvalue(c, rx));
	if (lex && rex) {
		// actually could be scalar const_collection!
		// TODO: need a specialized unroll_resolve_rvalues for the value_type
		// for now, just assert the type
		const preal_value_type lop = lex->static_constant_value();
		const preal_value_type rop = rex->static_constant_value();
		return return_type(new preal_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
preal_arith_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const preal_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const preal_expr>
preal_arith_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const preal_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const preal_expr>
preal_arith_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const preal_expr>& p) const {
	typedef	count_ptr<const preal_expr>		return_type;
	INVARIANT(p == this);
	const return_type
		rlx(lx->substitute_default_positional_parameters(f, e, lx)),
		rrx(rx->substitute_default_positional_parameters(f, e, rx));
	if (rlx && rrx) {
		if (rlx == lx && rrx == rx) {
			return p;
		} else {
			return return_type(new this_type(rlx, op, rrx));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_arith_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_arith_expr::write_object(const persistent_object_manager& m,
		ostream& f) const {
	write_value(f, reverse_op_map[op].op);	// writes a character
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_arith_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	char o;
	read_value(f, o);
	op = op_map[o];
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class preal_relational_expr method definitions

// static member initializations (order matters!)

const equal_to<pbool_value_type, preal_value_type>
preal_relational_expr::op_equal_to;

const not_equal_to<pbool_value_type, preal_value_type>
preal_relational_expr::op_not_equal_to;

const less<pbool_value_type, preal_value_type>
preal_relational_expr::op_less;

const greater<pbool_value_type, preal_value_type>
preal_relational_expr::op_greater;

const less_equal<pbool_value_type, preal_value_type>
preal_relational_expr::op_less_equal;

const greater_equal<pbool_value_type, preal_value_type>
preal_relational_expr::op_greater_equal;

const preal_relational_expr::op_map_type
preal_relational_expr::op_map;

const preal_relational_expr::reverse_op_map_type
preal_relational_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
preal_relational_expr::op_map_size = preal_relational_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
preal_relational_expr::op_map_register(const string& s, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[s] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered relationalmetic operators.  
	Really shouldn't have == and != because floating point numbers
	are only approximations to real valued numbers. 
	We shall have the compiler issue a warning.  
 */
size_t
preal_relational_expr::op_map_init(void) {
	op_map_register("==", &op_equal_to);
	op_map_register("!=", &op_not_equal_to);
	op_map_register("<", &op_less);
	op_map_register(">", &op_greater);
	op_map_register("<=", &op_less_equal);
	op_map_register(">=", &op_greater_equal);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
	Note: pass string, not null char.  
 */
preal_relational_expr::preal_relational_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_relational_expr::~preal_relational_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_relational_expr::preal_relational_expr(const operand_ptr_type& l,
		const string& o, const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_relational_expr::preal_relational_expr(const operand_ptr_type& l,
		const op_type* o, const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(preal_relational_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
preal_relational_expr::dump(ostream& o, const expr_dump_context& c) const {
	return rx->dump(lx->dump(o, c) << reverse_op_map[op], c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_relational_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_relational_expr::is_relaxed_formal_dependent(void) const {
	return lx->is_relaxed_formal_dependent() ||
		rx->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return result of resolved comparison.  
 */
preal_relational_expr::value_type
preal_relational_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_relational_expr::value_type
preal_relational_expr::evaluate(const string& o, const value_type l, 
		const value_type r) {
	const op_type* const op(op_map[o]);
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_relational_expr::value_type
preal_relational_expr::evaluate(const op_type* op, const value_type l, 
		const value_type r) {
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_relational_expr::must_be_equivalent(const pbool_expr& b) const {
	const this_type* const
		re = IS_A(const this_type*, &b);
	if (re) {
		return (op == re->op) &&
			lx->must_be_equivalent(*re->lx) &&
			rx->must_be_equivalent(*re->rx);
		// this is also conservative, 
		// doesn't check symbolic equivalence... yet
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
preal_relational_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_relational_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
preal_relational_expr::unroll_resolve_value(const unroll_context& c,
		value_type& i) const {
	static const expr_dump_context& dc(expr_dump_context::default_value);
	// should return a preal_const
	// maybe make a preal_const version to avoid casting
	preal_value_type lval, rval;
	const good_bool lex(lx->unroll_resolve_value(c, lval));
	const good_bool rex(rx->unroll_resolve_value(c, rval));
	if (!lex.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else if (!rex.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else {
		i = (*op)(lval, rval);
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pbool_const of the resolved value.
 */
count_ptr<const pbool_const>
preal_relational_expr::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_const>		return_type;
	typedef	count_ptr<const preal_const>		leaf_type;
	// should return a preal_const
	// maybe make a preal_const version to avoid casting
	INVARIANT(p == this);
	const leaf_type lex(lx->__unroll_resolve_rvalue(c, lx));
	const leaf_type rex(rx->__unroll_resolve_rvalue(c, rx));
	if (lex && rex) {
		const preal_value_type lop = lex->static_constant_value();
		const preal_value_type rop = rex->static_constant_value();
		return return_type(new pbool_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
preal_relational_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
preal_relational_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const pbool_expr>
preal_relational_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_expr>		return_type;
	typedef	count_ptr<const preal_expr>		operand_type;
	INVARIANT(p == this);
	const operand_type
		rlx(lx->substitute_default_positional_parameters(f, e, lx)),
		rrx(rx->substitute_default_positional_parameters(f, e, rx));
	if (rlx && rrx) {
		if (rlx == lx && rrx == rx) {
			return p;
		} else {
			return return_type(new this_type(rlx, op, rrx));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_relational_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_relational_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_relational_expr::load_object(const persistent_object_manager& m,
		istream& f) {
	{
	string s;
	read_value(f, s);
	op = op_map[s];
	NEVER_NULL(op);
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class pbool_logical_expr method definitions

// static member initializations (order matters!)

const util::logical_and<pbool_value_type, pbool_value_type>
pbool_logical_expr::op_and;

const util::logical_or<pbool_value_type, pbool_value_type>
pbool_logical_expr::op_or;

const util::logical_xor<pbool_value_type, pbool_value_type>
pbool_logical_expr::op_xor;

const util::logical_xnor<pbool_value_type, pbool_value_type>
pbool_logical_expr::op_xnor;

const pbool_logical_expr::op_map_type
pbool_logical_expr::op_map;

const pbool_logical_expr::reverse_op_map_type
pbool_logical_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
pbool_logical_expr::op_map_size = pbool_logical_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
pbool_logical_expr::op_map_register(const string& s, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[s] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered logical operators.  
 */
size_t
pbool_logical_expr::op_map_init(void) {
	op_map_register("&&", &op_and);
	op_map_register("||", &op_or);
	op_map_register("!=", &op_xor);
//	op_map_register("^", &op_xor);
	op_map_register("==", &op_xnor);
//	op_map_register("!^", &op_xnor);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pbool_logical_expr::pbool_logical_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_logical_expr::~pbool_logical_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_logical_expr::pbool_logical_expr(const operand_ptr_type& l,
		const string& o, const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_logical_expr::pbool_logical_expr(const operand_ptr_type& l,
		const op_type* o, const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pbool_logical_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_logical_expr::dump(ostream& o, const expr_dump_context& c) const {
	return rx->dump(lx->dump(o, c) << reverse_op_map[op], c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_logical_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_logical_expr::is_relaxed_formal_dependent(void) const {
	return lx->is_relaxed_formal_dependent() ||
		rx->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must be truly compile-time constant.
 */
pbool_logical_expr::value_type
pbool_logical_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_logical_expr::value_type
pbool_logical_expr::evaluate(const string& o, 
		const value_type l, const value_type r) {
	const op_type* op(op_map[o]);
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_logical_expr::value_type
pbool_logical_expr::evaluate(const op_type* op, 
		const value_type l, const value_type r) {
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_logical_expr::must_be_equivalent(const pbool_expr& b) const {
	const this_type* const re = IS_A(const this_type*, &b);
	if (re) {
		return (op == re->op) &&
			lx->must_be_equivalent(*re->lx) &&
			rx->must_be_equivalent(*re->rx);
		// this is also conservative, 
		// doesn't check symbolic equivalence... yet
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
pbool_logical_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_logical_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pbool_logical_expr::unroll_resolve_value(const unroll_context& c,
		value_type& i) const {
	static const expr_dump_context& dc(expr_dump_context::default_value);
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	value_type lval, rval;
	const good_bool lex(lx->unroll_resolve_value(c, lval));
	const good_bool rex(rx->unroll_resolve_value(c, rval));
	if (!lex.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else if (!rex.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr, dc) << endl;
		return good_bool(false);
	} else {
		i = (*op)(lval, rval);
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pbool_const of the resolved value.
 */
count_ptr<const pbool_const>
pbool_logical_expr::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_const>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	INVARIANT(p == this);
	const return_type lex(lx->__unroll_resolve_rvalue(c, lx));
	const return_type rex(rx->__unroll_resolve_rvalue(c, rx));
	if (lex && rex) {
#if 0
		const count_ptr<pbool_const> lpc(lex.is_a<pbool_const>());
		const count_ptr<pbool_const> rpc(rex.is_a<pbool_const>());
		// NOT TRUE: could be scalar const_collection
		INVARIANT(lpc);
		INVARIANT(rpc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		const pbool_value_type lop = lpc->static_constant_value();
		const pbool_value_type rop = rpc->static_constant_value();
#else
		const pbool_value_type lop = lex->static_constant_value();
		const pbool_value_type rop = rex->static_constant_value();
#endif
		return return_type(new pbool_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pbool_logical_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
pbool_logical_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return expression with any positional parameters substituted.  
 */
count_ptr<const pbool_expr>
pbool_logical_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_expr>		return_type;
	INVARIANT(p == this);
	const return_type
		rlx(lx->substitute_default_positional_parameters(f, e, lx)),
		rrx(rx->substitute_default_positional_parameters(f, e, rx));
	if (rlx && rrx) {
		if (rlx == lx && rrx == rx) {
			return p;
		} else {
			return return_type(new this_type(rlx, op, rrx));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_logical_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_logical_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_logical_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	string s;
	read_value(f, s);
	op = op_map[s];
	NEVER_NULL(op);
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class param_defined method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
param_defined::param_defined() :
		pbool_expr(), arg(NULL) {
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_defined::param_defined(
		const operand_ptr_type& e) :
		pbool_expr(), arg(e) {
	NEVER_NULL(arg);
	INVARIANT(arg->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(param_defined)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_defined::dump(ostream& o, const expr_dump_context& c) const {
	const bool p = c.need_parentheses(OP_PREC_UNARY);
	if (p) o << '(';
	arg->dump(o << "#", c);
	if (p) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_defined::is_static_constant(void) const {
	return false;	// conservatively
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_defined::is_relaxed_formal_dependent(void) const {
	// need cross-cast first
	return arg.is_a<const param_expr>()->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not sure if this is correct.
 */
param_defined::value_type
param_defined::static_constant_value(void) const {
	ICE_NEVER_CALL(cerr);
//	return arg->static_constant_value();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_defined::must_be_equivalent(const pbool_expr& b) const {
#if 0
	const this_type* const be = IS_A(const this_type*, &b);
	if (be) {
		const count_ptr<const param_expr>
			pa(arg.is_a<const param_expr>());
		return pa->must_be_equivalent(*be->arg);
	} else {
		// conservatively
		return false;
	}
#else
	return false;		// conservatively
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
param_defined::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_defined::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
param_defined::unroll_resolve_value(const unroll_context& c, 
		value_type& i) const {
	NEVER_NULL(arg);
	const good_bool ret(arg->unroll_resolve_defined(c, i));
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluates whether or not reference is defined.
 */
count_ptr<const pbool_const>
param_defined::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_const>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	INVARIANT(p == this);
	bool result;
	const good_bool g(arg->unroll_resolve_defined(c, result));
	if (g.good) {
		return return_type(
			new pbool_const(result));
	} else {
		// there is an error
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
param_defined::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
param_defined::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is this operation even valid?
	\return expression with any positional parameters substituted.  
 */
count_ptr<const pbool_expr>
param_defined::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pbool_expr>& p) const {
	typedef	count_ptr<const pbool_expr>		return_type;
#if 0
	INVARIANT(p == this);
	const count_ptr<const param_expr> pa(arg.is_a<const param_expr>());
	const return_type
		rex(pa->substitute_default_positional_parameters(f, e, pa));
	if (rex) {
		if (rex == arg) {
			return p;
		} else {
			return return_type(new this_type(rex));
		}
	} else {
		return return_type(NULL);
	}
#else
	FINISH_ME(Fang);
	return return_type(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_defined::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	arg->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_defined::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer(f, arg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_defined::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, arg);
}

//=============================================================================
// explicit template instantiations

template class loop_meta_expr<pint_arith_expr>;
template class loop_meta_expr<pbool_logical_expr>;
template class loop_meta_expr<preal_arith_expr>;

template class convert_expr<preal_expr, pint_expr>;

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

