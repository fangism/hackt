/**
	\file "Object/expr/operators.cc"
	Meta parameter operator expressions.  
	TODO: there is so much code duplication in here...
		This NEEDS to be templated somehow...
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: operators.cc,v 1.14.2.2 2006/07/01 03:42:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_OPERATORS_CC__
#define	__HAC_OBJECT_EXPR_OPERATORS_CC__

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
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/operator_precedence.h"
#include "Object/persistent_type_hash.h"

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
		const op_type o, const count_ptr<const pint_expr>& e) :
		pint_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_unary_expr::pint_unary_expr(
		const count_ptr<const pint_expr>& e, const op_type o) :
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
pint_unary_expr::value_type
pint_unary_expr::static_constant_value(void) const {
	// depends on op
	return - ex->static_constant_value();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_unary_expr::must_be_equivalent(const pint_expr& p) const {
	const pint_unary_expr* const ue = IS_A(const pint_unary_expr*, &p);
	if (ue) {
		return op == ue->op && ex->must_be_equivalent(*ue->ex);
	} else {
		// conservatively
		return false;
	}
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
	i = -j;		// regardless of ret
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns resolved value of negation expression.  
 */
good_bool
pint_unary_expr::resolve_value(value_type& i) const {
	value_type j;
	NEVER_NULL(ex);
	const good_bool ret(ex->resolve_value(j));
	i = -j;		// regardless of ret
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
		return return_type(
			new pint_const(-ret->static_constant_value()));
#endif
	} else {
		// there is an error
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
pint_unary_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const pint_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_expr>
pint_unary_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	INVARIANT(p == this);
	// lazy...
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const pint_expr>();
#endif
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
		const op_type o, const count_ptr<const preal_expr>& e) :
		preal_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
preal_unary_expr::preal_unary_expr(
		const count_ptr<const preal_expr>& e, const op_type o) :
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
bool
preal_unary_expr::must_be_equivalent(const preal_expr& p) const {
	const preal_unary_expr* const ue = IS_A(const preal_unary_expr*, &p);
	if (ue) {
		return op == ue->op && ex->must_be_equivalent(*ue->ex);
	} else {
		// conservatively
		return false;
	}
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
	Returns resolved value of negation expression.  
 */
good_bool
preal_unary_expr::resolve_value(value_type& i) const {
	value_type j;
	NEVER_NULL(ex);
	const good_bool ret(ex->resolve_value(j));
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
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
preal_unary_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const preal_expr>& p
		) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const preal_expr>
preal_unary_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const preal_expr>& p) const {
	INVARIANT(p == this);
	// lazy...
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const preal_expr>();
#endif
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
		const op_type o, const count_ptr<const pbool_expr>& e) :
		pbool_expr(), op(o), ex(e) {
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_unary_expr::pbool_unary_expr(
		const count_ptr<const pbool_expr>& e, const op_type o) :
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
bool
pbool_unary_expr::static_constant_value(void) const {
	return !ex->static_constant_value();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::must_be_equivalent(const pbool_expr& b) const {
	const pbool_unary_expr* const be = IS_A(const pbool_unary_expr*, &b);
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
good_bool
pbool_unary_expr::resolve_value(value_type& i) const {
	value_type b;
	const good_bool ret(ex->resolve_value(b));
	i = !b;
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
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
pbool_unary_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
pbool_unary_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	// lazy...
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const pbool_expr>();
#endif
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
	rx->dump(lx->dump(o, c) << oi.op, c);
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
bool
pint_arith_expr::must_be_equivalent(const pint_expr& p) const {
	const pint_arith_expr* const ae = IS_A(const pint_arith_expr*, &p);
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
good_bool
pint_arith_expr::resolve_value(value_type& i) const {
	static const expr_dump_context& c(expr_dump_context::default_value);
	arg_type a, b;
	NEVER_NULL(lx);	NEVER_NULL(rx);
	const good_bool lret(lx->resolve_value(a));
	const good_bool rret(rx->resolve_value(b));
	if (!lret.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr, c) << endl;
		return good_bool(false);
	} else if (!rret.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr, c) << endl;
		return good_bool(false);
	} else {
		// Oooooh, virtual operator dispatch!
		i = (*op)(a,b);
		return good_bool(true);
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
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
pint_arith_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_expr>
pint_arith_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	INVARIANT(p == this);
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const pint_expr>();
#endif
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
pint_relational_expr::may_be_initialized(void) const {
	return lx->may_be_initialized() && rx->may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_relational_expr::must_be_initialized(void) const {
	return lx->must_be_initialized() && rx->must_be_initialized();
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
bool
pint_relational_expr::must_be_equivalent(const pbool_expr& b) const {
	const pint_relational_expr* const re = IS_A(const pint_relational_expr*, &b);
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
	TO DO: switch on relational expression operator.  
 */
good_bool
pint_relational_expr::resolve_value(value_type& i) const {
	arg_type li, ri;
	const good_bool l_ret(lx->resolve_value(li));
	const good_bool r_ret(rx->resolve_value(ri));
	// SWITCH
	i = (*op)(li, ri);
	return l_ret && r_ret;
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
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
pint_relational_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
pint_relational_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	// lazy...
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const pbool_expr>();
#endif
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
bool
preal_arith_expr::must_be_equivalent(const preal_expr& p) const {
	const preal_arith_expr* const ae = IS_A(const preal_arith_expr*, &p);
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
good_bool
preal_arith_expr::resolve_value(value_type& i) const {
	static const expr_dump_context& c(expr_dump_context::default_value);
	arg_type a, b;
	NEVER_NULL(lx);	NEVER_NULL(rx);
	const good_bool lret(lx->resolve_value(a));
	const good_bool rret(rx->resolve_value(b));
	if (!lret.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr, c) << endl;
		return good_bool(false);
	} else if (!rret.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr, c) << endl;
		return good_bool(false);
	} else {
		// Oooooh, virtual operator dispatch!
		i = (*op)(a,b);
		return good_bool(true);
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
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
preal_arith_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const preal_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const preal_expr>
preal_arith_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const preal_expr>& p) const {
	INVARIANT(p == this);
	// lazy...
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const preal_expr>();
#endif
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
preal_relational_expr::may_be_initialized(void) const {
	return lx->may_be_initialized() && rx->may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_relational_expr::must_be_initialized(void) const {
	return lx->must_be_initialized() && rx->must_be_initialized();
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
bool
preal_relational_expr::must_be_equivalent(const pbool_expr& b) const {
	const preal_relational_expr* const re = IS_A(const preal_relational_expr*, &b);
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
	TO DO: switch on relational expression operator.  
 */
good_bool
preal_relational_expr::resolve_value(value_type& i) const {
	arg_type li, ri;
	const good_bool l_ret(lx->resolve_value(li));
	const good_bool r_ret(rx->resolve_value(ri));
	// SWITCH
	i = (*op)(li, ri);
	return l_ret && r_ret;
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
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
preal_relational_expr::unroll_resolve_rvalues(const unroll_context& c,
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
preal_relational_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	// lazy...
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const pbool_expr>();
#endif
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
	Static initialization of registered logicalmetic operators.  
 */
size_t
pbool_logical_expr::op_map_init(void) {
	op_map_register("&&", &op_and);
	op_map_register("||", &op_or);
	op_map_register("^", &op_xor);
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
bool
pbool_logical_expr::must_be_equivalent(const pbool_expr& b) const {
	const pbool_logical_expr* const re = IS_A(const pbool_logical_expr*, &b);
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
	TO DO: switch on logical expression operator.  
 */
good_bool
pbool_logical_expr::resolve_value(value_type& i) const {
	arg_type lb, rb;
	const good_bool l_ret(lx->resolve_value(lb));
	const good_bool r_ret(rx->resolve_value(rb));
	i = (*op)(lb, rb);
	return l_ret && r_ret;
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
#if COW_UNROLL_RESOLVE_RVALUES
count_ptr<const const_param>
pbool_logical_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	return __unroll_resolve_rvalue(c, p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_expr>
pbool_logical_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	// lazy...
#if COW_UNROLL_RESOLVE_RVALUES
	return __unroll_resolve_rvalue(c, p);
#else
	return unroll_resolve_rvalues(c).is_a<const pbool_expr>();
#endif
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

#endif	// __HAC_OBJECT_EXPR_OPERATORS_CC__

