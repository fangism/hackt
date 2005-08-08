/**
	\file "Object/expr/basic_param.cc"
	Class definitions for basic parameter expression types.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: basic_param.cc,v 1.4 2005/08/08 23:08:28 fang Exp $
 */

#ifndef	__OBJECT_EXPR_BASIC_PARAM_CC_
#define	__OBJECT_EXPR_BASIC_PARAM_CC_

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

#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/inst/param_value_collection.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/pint_traits.h"	// needed for assign
#include "Object/traits/pbool_traits.h"	// needed for assign
#include "Object/type/data_type_reference.h"
#include "Object/unroll/expression_assignment.h"
#include "Object/persistent_type_hash.h"

#include "util/what.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::pint_const, "pint-const")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_const, "pbool-const")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_const, CONST_PINT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_const, CONST_PBOOL_TYPE_KEY, 0)

namespace memory {
	// pool-allocator managed types that are safe to destroy lazily
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::pbool_const)
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::pint_const)
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
using util::write_value;
using util::read_value;
using util::persistent_traits;
#include "util/using_ostream.h"

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
// the robust list_vector_pool requires this.  
#endif

//=============================================================================
// class param_expr method_definitions

/**
	When pint is interpreted as an int, in non-meta language...
 */
count_ptr<const data_type_reference>
pint_expr::get_data_type_ref(void) const {
	return int_traits::int32_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs an expression assignment object of the appropriate
	value type.  
	Wrapped calls to private constructors.  
	\param p the right-hand-side expression of the assignment object.  
	\return newly constructed and initialized assignment object.  
 */
excl_ptr<param_expression_assignment>
param_expr::make_param_expression_assignment(
		const count_ptr<const param_expr>& p) {
	typedef	excl_ptr<param_expression_assignment>	return_type;
	if (!p->may_be_initialized()) {
		p->dump(cerr << "ERROR: rhs of expr-assignment is "
			"not initialized or dependent on formals: ") << endl;
		THROW_EXIT;		// temporary
		return return_type(NULL);
	} else	return p->make_param_expression_assignment_private(p);
}

//-----------------------------------------------------------------------------
// class const_param method definitions

//-----------------------------------------------------------------------------
// class pbool_expr method definitions

#if 0
pbool_expr::~pbool_expr() {
	STACKTRACE_DTOR("~pbool_expr()");
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When pint is interpreted as an int, in non-meta language...
 */
count_ptr<const data_type_reference>
pbool_expr::get_data_type_ref(void) const {
	return bool_traits::built_in_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_expr::may_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pbool_expr::may_be_equivalent_generic()");
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_value() ==
				b->static_constant_value();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_expr::must_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pbool_expr::must_be_equivalent_generic()");
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
#if 0
		if (is_static_constant() && b->is_static_constant())
			return static_constant_value() ==
				b->static_constant_value();
		// else check template formals?  more cases needed
		else	return false;
#else
		return must_be_equivalent(*b);
#endif
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_ptr<const const_param>
pbool_expr::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pbool_const(static_constant_value()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<param_expression_assignment>
pbool_expr::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	typedef	excl_ptr<param_expression_assignment>	return_type;
	INVARIANT(p == this);
	return return_type(
		new pbool_expression_assignment(p.is_a<const pbool_expr>()));
}

//-----------------------------------------------------------------------------
// class pint_expr method definitions

#if 0
pint_expr::~pint_expr() {
	STACKTRACE_DTOR("~pint_expr()");
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::may_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pint_expr::may_be_equivalent_generic()");
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
		if (is_static_constant() && i->is_static_constant())
			return static_constant_value() ==
				i->static_constant_value();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::must_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pint_expr::must_be_equivalent_generic()");
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
#if 0
		if (is_static_constant() && i->is_static_constant())
			return static_constant_value() ==
				i->static_constant_value();
		else	return false;
#else
		return must_be_equivalent(*i);
#endif
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_ptr<const const_param>
pint_expr::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pint_const(static_constant_value()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<param_expression_assignment>
pint_expr::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	typedef	excl_ptr<param_expression_assignment>	return_type;
	INVARIANT(p == this);
	return return_type(
		new pint_expression_assignment(p.is_a<const pint_expr>()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return deep copy of resolve constant integer value, 
	if it is successfully resolved.  
 */
count_ptr<const_index>
pint_expr::unroll_resolve_index(const unroll_context& c) const {
	STACKTRACE("pint_expr::unroll_resolve_index()");
	typedef count_ptr<const_index> return_type;
	value_type i;
	return (unroll_resolve_value(c, i).good) ? 
		return_type(new pint_const(i)) :
		return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return deep copy of resolve constant integer value, 
	if it is successfully resolved.  
 */
count_ptr<const_index>
pint_expr::resolve_index(void) const {
	STACKTRACE("pint_expr::resolve_index()");
	typedef count_ptr<const_index> return_type;
	value_type i;
	return (resolve_value(i).good) ? 
		return_type(new pint_const(i)) :
		return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::must_be_equivalent_index(const meta_index_expr& i) const {
	const pint_expr* const p = IS_A(const pint_expr*, &i);
	if (p) {
		return must_be_equivalent(*p);
	} else {
		return false;
	}
}

//=============================================================================
// class param_expr_collective method defintions

#if 0
param_expr_collective::param_expr_collective() : param_expr(), elist() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expr_collective::~param_expr_collective() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_expr_collective::what(ostream& o) const {
	return o << "param-expr-collective";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
param_expr_collective::hash_string(void) const {
	string ret("{");
	list<excl_ptr<param_expr> >::const_iterator i(elist.begin());
	for ( ; i!=elist.end(); i++) {
		const never_ptr<const param_expr> p(*i);
		NEVER_NULL(p);
		ret += p->hash_string();
		ret += ",";
	}
	ret += "}";
	return ret;
}
#endif

//=============================================================================
// class pint_const method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
	ALERT: we allocate one of these during the static initialization
	of built-ins, we may need a safeguard to ensure that
	the allocator is initialized first!
***/
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(pint_const, 1024)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private inline empty constructor, uninitialized.
 */
inline
pint_const::pint_const() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const::~pint_const() {
	STACKTRACE_DTOR("~pint_const()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_const::dump_brief(ostream& o) const {
	return dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_const::dump(ostream& o) const {
	return o << val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A pint const is scalar, thus dimension list is empty.
 */
const_range_list
pint_const::static_constant_dimensions(void) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
	Just copy-constructs.  
 */
count_ptr<const const_param>
pint_const::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pint_const(val));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_const::operator == (const const_range& c) const {
	return (val == c.first) && (val == c.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_const::must_be_equivalent(const pint_expr& p) const {
	STACKTRACE("pint_const::must_be_equivalent()");
	return p.is_static_constant() && (val == p.static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const::value_type
pint_const::lower_bound(void) const {
	return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const::value_type
pint_const::upper_bound(void) const {
	return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pint_const::resolve_value(value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return deep copy of this constant integer, always succeeds.  
 */
count_ptr<const_index>
pint_const::resolve_index(void) const {
	return count_ptr<const_index>(new pint_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty range list signifying that expression is scalar, 0-D.  
 */
const_index_list
pint_const::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is range equivalent in size?
	\return always false, because an int is really a collapsed range.
 */
bool
pint_const::range_size_equivalent(const const_index& i) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<param_expression_assignment>
pint_const::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	return pint_expr::make_param_expression_assignment_private(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pint_const::unroll_resolve_value(const unroll_context& c, value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const_param>
pint_const::unroll_resolve(const unroll_context& c) const {
	return count_ptr<const_param>(new pint_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const_index>
pint_const::unroll_resolve_index(const unroll_context& c) const {
	return count_ptr<const_index>(new pint_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does nothing, no pointers to visit.  
 */
void
pint_const::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const::write_object(const persistent_object_manager& m, ostream& f) const {
	write_value(f, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const::load_object(const persistent_object_manager& m, istream& f) {
	read_value(f, val);
}

//=============================================================================
// class pbool_const method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(pbool_const, 1024)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pbool_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private inline empty constructor, uninitialized.
 */
inline
pbool_const::pbool_const() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_const::dump_brief(ostream& o) const {
	return dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_const::dump(ostream& o) const {
	return o << ((val) ? "true" : "false");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
	Just copy-constructs.  
 */
count_ptr<const const_param>
pbool_const::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pbool_const(val));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A pbool const is scalar, thus dimension list is empty.
 */
const_range_list
pbool_const::static_constant_dimensions(void) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<param_expression_assignment>
pbool_const::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	return pbool_expr::make_param_expression_assignment_private(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
pbool_const::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pbool_const::resolve_value(value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const_param>
pbool_const::unroll_resolve(const unroll_context& c) const {
	return count_ptr<const_param>(new pbool_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_const::must_be_equivalent(const pbool_expr& b) const {
	return b.is_static_constant() && (val == b.static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::load_object(const persistent_object_manager& m, istream& f) {
	read_value(f, val);
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

#endif	// __OBJECT_EXPR_BASIC_PARAM_CC_

