/**
	\file "Object/expr/operators.cc"
	Meta parameter operator expressions.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: operators.cc,v 1.3.6.1 2005/08/08 19:07:54 fang Exp $
 */

#ifndef	__OBJECT_EXPR_OPERATORS_CC__
#define	__OBJECT_EXPR_OPERATORS_CC__

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/expr/pint_arith_expr.h"
#include "Object/expr/pint_relational_expr.h"
#include "Object/expr/pint_unary_expr.h"
#include "Object/expr/pbool_logical_expr.h"
#include "Object/expr/pbool_unary_expr.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/const_range.h"
#include "Object/persistent_type_hash.h"

#include "util/stacktrace.h"
#include "util/qmap.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::pint_unary_expr,
		"pint-unary-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_unary_expr,
		"pbool-unary-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_arith_expr, 
		"arith-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_relational_expr, 
		"relational-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_logical_expr, 
		"logical-expr")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_unary_expr, PINT_UNARY_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_unary_expr, PBOOL_UNARY_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_arith_expr, META_ARITH_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_relational_expr, META_RELATIONAL_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_logical_expr, META_LOGICAL_EXPR_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace ART {
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
pint_unary_expr::dump_brief(ostream& o) const {
	return ex->dump_brief(o << op);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_unary_expr::dump(ostream& o) const {
	// parentheses? check operator precedence
	return ex->dump(o << op);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list
pint_unary_expr::static_constant_dimensions(void) const {
	return const_range_list();
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
bool
pint_unary_expr::is_loop_independent(void) const {
	return ex->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_unary_expr::is_unconditional(void) const {
	return ex->is_unconditional();
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
count_ptr<const_param>
pint_unary_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type ret(ex->unroll_resolve(c));
	if (ret) {
#if 0
		count_ptr<pint_const> pc(ret.is_a<pint_const>());
		INVARIANT(pc);	// NOT TRUE, could be scalar const_collection
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(
			new pint_const(- pc->static_constant_value()));
#else
		return return_type(new pint_const(
			-ret.is_a<const pint_expr>()->static_constant_value()));
#endif
	} else {
		// there is an error
		// discard intermediate result
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
pbool_unary_expr::dump_brief(ostream& o) const {
	// parentheses?
	return ex->dump_brief(o << op);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_unary_expr::dump(ostream& o) const {
	// parentheses?
	return ex->dump(o << op);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list
pbool_unary_expr::static_constant_dimensions(void) const {
	return const_range_list();
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
pbool_unary_expr::is_loop_independent(void) const {
	return ex->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::is_unconditional(void) const {
	return ex->is_unconditional();
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
count_ptr<const_param>
pbool_unary_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type ret(ex->unroll_resolve(c));
	if (ret) {
#if 0
		count_ptr<pbool_const> pc(ret.is_a<pbool_const>());
		INVARIANT(pc);	// NOT TRUE: could be scalar const_collection
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(
			new pbool_const(!pc->static_constant_value()));
#else
		return return_type(new pbool_const(
			!ret.is_a<const pbool_expr>()
				->static_constant_value()));
#endif
	} else {
		// there is an error
		// discard intermediate result
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
pint_arith_expr::op_map_register(const char c, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[c] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered arithmetic operators.  
 */
size_t
pint_arith_expr::op_map_init(void) {
	op_map_register('+', &adder);
	op_map_register('-', &subtractor);
	op_map_register('*', &multiplier);
	op_map_register('/', &divider);
	op_map_register('%', &remainder);
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
ostream&
pint_arith_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_arith_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list
pint_arith_expr::static_constant_dimensions(void) const {
	return const_range_list();
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
bool
pint_arith_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_arith_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
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
	arg_type a, b;
	NEVER_NULL(lx);	NEVER_NULL(rx);
	const good_bool lret(lx->resolve_value(a));
	const good_bool rret(rx->resolve_value(b));
	if (!lret.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr) << endl;
		return good_bool(false);
	} else if (!rret.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr) << endl;
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
pint_arith_expr::unroll_resolve_value(const unroll_context& c, value_type& i) const {
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	value_type lval, rval;
	const good_bool lex(lx->unroll_resolve_value(c, lval));
	const good_bool rex(rx->unroll_resolve_value(c, rval));
	if (!lex.good) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr) << endl;
		return good_bool(false);
	} else if (!rex.good) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr) << endl;
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
count_ptr<const_param>
pint_arith_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type lex(lx->unroll_resolve(c));
	const return_type rex(rx->unroll_resolve(c));
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
		// TODO: need a specialized unroll_resolve for the value_type
		// for now, just assert the type
		const pint_value_type lop =
			lex.is_a<const pint_expr>()->static_constant_value();
		const pint_value_type rop =
			rex.is_a<const pint_expr>()->static_constant_value();
#endif
		return return_type(new pint_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
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
	write_value(f, reverse_op_map[op]);	// writes a character
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
pint_relational_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_relational_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
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
const_range_list
pint_relational_expr::static_constant_dimensions(void) const {
	return const_range_list();
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
bool
pint_relational_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_relational_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
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
count_ptr<const_param>
pint_relational_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type lex(lx->unroll_resolve(c));
	const return_type rex(rx->unroll_resolve(c));
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
		const pint_value_type lop =
			lex.is_a<const pint_expr>()->static_constant_value();
		const pint_value_type rop =
			rex.is_a<const pint_expr>()->static_constant_value();
#endif
		return return_type(new pbool_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
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
pbool_logical_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_logical_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list
pbool_logical_expr::static_constant_dimensions(void) const {
	return const_range_list();
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
bool
pbool_logical_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_logical_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
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
count_ptr<const_param>
pbool_logical_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type lex(lx->unroll_resolve(c));
	const return_type rex(rx->unroll_resolve(c));
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
		const pbool_value_type lop =
			lex.is_a<const pbool_expr>()->static_constant_value();
		const pbool_value_type rop =
			rex.is_a<const pbool_expr>()->static_constant_value();
#endif
		return return_type(new pbool_const((*op)(lop, rop)));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
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
}	// end namepace entity
}	// end namepace ART

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

#endif	// __OBJECT_EXPR_OPERATORS_CC__

