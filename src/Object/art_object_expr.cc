/**
	\file "art_object_expr.cc"
	Class method definitions for semantic expression.  
 	$Id: art_object_expr.cc,v 1.36.4.5 2005/01/23 01:33:54 fang Exp $
 */

#ifndef	__ART_OBJECT_EXPR_CC__
#define	__ART_OBJECT_EXPR_CC__

// #define	DEBUG_LIST_VECTOR_POOL		1

#include <exception>
#include <iostream>
#include <algorithm>

#include "memory/pointer_classes.h"
#include "sstream.h"			// for ostringstring, used by dump
#include "discrete_interval_set.tcc"

#include "what.tcc"
#include "STL/list.tcc"
#include "qmap.tcc"
#include "stacktrace.h"

// consider: (for reducing expression storage overhead)
// #define NO_OBJECT_SANITY	1
// this will override the definition in "art_object_base.h"

#include "art_object_index.h"
#include "art_object_expr.h"		// includes "art_object_expr_const.h"
#include "art_object_expr_param_ref.h"
#include "art_object_instance_param.h"
#include "art_object_assign.h"

#if 0
#include "multikey.h"			// extern template instantiations
#include "packed_array.h"		// extern template instantiations
#else
// experimental: suppressing automatic instantiation of template code
#include "art_object_extern_templates.h"
#endif

#include "memory/list_vector_pool.h"
#include "persistent_object_manager.tcc"

#include "art_object_type_hash.h"

#include "compose.h"
#include "conditional.h"		// for compare_if
#include "ptrs_functional.h"

//=============================================================================
namespace util {
// specializations of the class what, needed to override the default
// mangled names returned by type_info::name().

// These specializations need not be externally visible, unless
// the class template "what" is used directly.  
// Most of the time, the "what" member function interface is used.  

SPECIALIZE_UTIL_WHAT(ART::entity::pint_const_collection,
		"pint-const-collection")
SPECIALIZE_UTIL_WHAT(ART::entity::const_param_expr_list,
		"const-param-expr-list")
SPECIALIZE_UTIL_WHAT(ART::entity::dynamic_param_expr_list,
		"param-expr-list")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_instance_reference,
		"pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_instance_reference,
		"pint-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_const,
		"pint-const")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_const,
		"pbool-const")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_unary_expr,
		"pint-unary-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_unary_expr,
		"pbool-unary-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::arith_expr, 
		"arith-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::relational_expr, 
		"relational-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::logical_expr, 
		"logical-expr")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_range, 
		"pint-range")
SPECIALIZE_UTIL_WHAT(ART::entity::const_range, 
		"const-range")
SPECIALIZE_UTIL_WHAT(ART::entity::const_range_list, 
		"const-range-list")
SPECIALIZE_UTIL_WHAT(ART::entity::dynamic_range_list, 
		"dynamic_range_list")
SPECIALIZE_UTIL_WHAT(ART::entity::const_index_list, 
		"const-index-list")
SPECIALIZE_UTIL_WHAT(ART::entity::dynamic_index_list, 
		"dynamic-index-list")

}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
//=============================================================================
#include "using_ostream.h"
using namespace ADS;
using namespace util::memory;
USING_UTIL_OPERATIONS
using DISCRETE_INTERVAL_SET_NAMESPACE::discrete_interval_set;
using std::_Select1st;
using std::_Select2nd;
using std::mem_fun_ref;
using std::dereference;
using std::ostringstream;
USING_STACKTRACE

//=============================================================================
// local types (not externally visible)

/**
	Implementation type for range-checking, 
	used by const_range.
 */
typedef discrete_interval_set<pint_value_type>	interval_type;


//=============================================================================
// class param_expr method_definitions

#if 0
// inline
param_expr::param_expr() : object(), persistent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inline
param_expr::~param_expr() { }
#endif

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

#if 0
// inline
const_param::const_param() : param_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inline
const_param::~const_param() { }
#endif

//-----------------------------------------------------------------------------
// class pbool_expr method definitions

bool
pbool_expr::may_be_equivalent(const param_expr& p) const {
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_bool() ==
				b->static_constant_bool();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_expr::must_be_equivalent(const param_expr& p) const {
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_bool() ==
				b->static_constant_bool();
		else	return false;
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
		new pbool_const(static_constant_bool()));
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

bool
pint_expr::may_be_equivalent(const param_expr& p) const {
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
		if (is_static_constant() && i->is_static_constant())
			return static_constant_int() ==
				i->static_constant_int();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::must_be_equivalent(const param_expr& p) const {
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
		if (is_static_constant() && i->is_static_constant())
			return static_constant_int() ==
				i->static_constant_int();
		else	return false;
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
		new pint_const(static_constant_int()));
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
pint_expr::resolve_index(void) const {
	STACKTRACE("pint_expr::resolve_index()");
	typedef count_ptr<const_index> return_type;
	value_type i;
	return (resolve_value(i)) ? 
		return_type(new pint_const(i)) :
		return_type(NULL);
}

//=============================================================================
// class param_expr_list method definitions

#if 0
param_expr_list::param_expr_list() : object(), persistent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expr_list::~param_expr_list() { }
#endif

//=============================================================================
// class const_param_expr_list method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(const_param_expr_list, 
	CONST_PARAM_EXPR_LIST_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_param_expr_list::const_param_expr_list() :
		param_expr_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_param_expr_list::~const_param_expr_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_param_expr_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_param_expr_list::dump(ostream& o) const {
	if (empty()) return o;
	// else at least 1 item in list
	// hint: ostream_iterator
	const_iterator i = begin();
	NEVER_NULL(*i);
	(*i)->dump(o);
	for (i++; i!=end(); i++) {
		o << ", ";
		NEVER_NULL(*i);
		(*i)->dump(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
const_param_expr_list::size(void) const {
	return parent_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<param_expr_list>
const_param_expr_list::make_copy(void) const {
	return excl_ptr<param_expr_list>(
		new const_param_expr_list(*this));
	// use default copy constructor
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_param_expr_list::may_be_initialized(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_param_expr_list::must_be_initialized(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Precondition: all expressions must be non-NULL.  
 */
list<const param_expr&>
const_param_expr_list::get_const_ref_list(void) const {
	list<const param_expr&> ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		ret.push_back(**i);
	}
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_param_expr_list::may_be_equivalent(const param_expr_list& p) const {
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &p);
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const const_param> ip(*i);
		const count_ptr<const const_param> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == cpl->end());		// sanity
	return true;
} else {
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	dynamic_param_expr_list::const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const const_param> ip(*i);
		const count_ptr<const param_expr> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == dpl->end());		// sanity
	return true;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_param_expr_list::must_be_equivalent(const param_expr_list& p) const {
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &p);
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const const_param> ip(*i);
		const count_ptr<const const_param> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == cpl->end());		// sanity
	return true;
} else {
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	dynamic_param_expr_list::const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const const_param> ip(*i);
		const count_ptr<const param_expr> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == dpl->end());		// sanity
	return true;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return newly allocated copy of itself, always succeeds.  
	Eventually will add some context argument, though it is not needed
	because this is already constant.  
 */
excl_ptr<const_param_expr_list>
const_param_expr_list::unroll_resolve(const unroll_context& c) const {
	// safe to use default copy construction because
	// count_ptr's are copy-constructible
	return excl_ptr<const_param_expr_list>(
		new const_param_expr_list(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
const_param_expr_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CONST_PARAM_EXPR_LIST_TYPE_KEY)) {
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const const_param> ip(*i);
		ip->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty constructor / allocator for the first pass of 
	deserialization reconstruction.  
 */
persistent*
const_param_expr_list::construct_empty(const int i) {
	return new const_param_expr_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_param_expr_list::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const const_param> ip(*i);
		m.write_pointer(f, ip);
	}
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_param_expr_list::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<const_param> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			ip->load_object(m);
#endif
		push_back(ip);
	}
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class dynamic_param_expr_list method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(dynamic_param_expr_list, 
	DYNAMIC_PARAM_EXPR_LIST_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_param_expr_list::dynamic_param_expr_list() :
		param_expr_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_param_expr_list::~dynamic_param_expr_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_param_expr_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_param_expr_list::dump(ostream& o) const {
	if (empty()) return o;
	// else at least 1 item in list
	const_iterator i = begin();
	if (*i)	(*i)->dump(o);
	else	o << "(null)";
	for (i++; i!=end(); i++) {
		o << ", ";
		if (*i)	(*i)->dump(o);
		else	o << "(null)";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_param_expr_list::size(void) const {
	return parent_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<param_expr_list>
dynamic_param_expr_list::make_copy(void) const {
	return excl_ptr<param_expr_list>(
		new dynamic_param_expr_list(*this));
	// use default copy constructor
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::is_static_constant(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		const count_ptr<const param_expr> ip(*i);
		NEVER_NULL(ip);	// nothing may be NULL at this point!
		if (!ip->is_static_constant())
			return false;
		// else continue checking...
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::is_loop_independent(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		const count_ptr<const param_expr> ip(*i);
		NEVER_NULL(ip);	// nothing may be NULL at this point!
		if (!ip->is_loop_independent())
			return false;
		// else continue checking...
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::may_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		const count_ptr<const param_expr> ip(*i);
		NEVER_NULL(ip);	// nothing may be NULL at this point!
		if (!ip->may_be_initialized())
			return false;
		// else continue checking...
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::must_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		const count_ptr<const param_expr> ip(*i);
		NEVER_NULL(ip);	// nothing may be NULL at this point!
		if (!ip->must_be_initialized())
			return false;
		// else continue checking...
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Precondition: all expressions must be non-NULL.  
 */
list<const param_expr&>
dynamic_param_expr_list::get_const_ref_list(void) const {
	list<const param_expr&> ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++, j++) {
		count_ptr<const param_expr> ip(*i);
		NEVER_NULL(ip);
		ret.push_back(*ip);
	}
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::may_be_equivalent(const param_expr_list& p) const {
	const never_ptr<const const_param_expr_list>
		cpl(IS_A(const const_param_expr_list*, &p));
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_param_expr_list::const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const param_expr> ip(*i);
		const count_ptr<const const_param> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == cpl->end());		// sanity
	return true;
} else {
	const never_ptr<const dynamic_param_expr_list>
		dpl(IS_A(const dynamic_param_expr_list*, &p));
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const param_expr> ip(*i);
		const count_ptr<const param_expr> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == dpl->end());		// sanity
	return true;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::must_be_equivalent(const param_expr_list& p) const {
	const never_ptr<const const_param_expr_list>
		cpl(IS_A(const const_param_expr_list*, &p));
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_param_expr_list::const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const param_expr> ip(*i);
		const count_ptr<const const_param> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == cpl->end());		// sanity
	return true;
} else {
	const never_ptr<const dynamic_param_expr_list>
		dpl(IS_A(const dynamic_param_expr_list*, &p));
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_ptr<const param_expr> ip(*i);
		const count_ptr<const param_expr> jp(*j);
		INVARIANT(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == dpl->end());		// sanity
	return true;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return constant-resolved parameter list, 
		else NULL if resolution failed.
	TODO: add context argument for resolution.  
 */
excl_ptr<const_param_expr_list>
dynamic_param_expr_list::unroll_resolve(const unroll_context& c) const {
	typedef	excl_ptr<const_param_expr_list>		return_type;
	return_type ret(new const_param_expr_list);
	NEVER_NULL(ret);
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const param_expr> ip(*i);
		count_ptr<const_param> pc(ip->unroll_resolve(c));
		if (pc) {
			ret->push_back(pc);
		} else {
			cerr << "ERROR in dynamic_param_expr_list::unroll_resolve()" << endl;
			return return_type(NULL);
		}
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
dynamic_param_expr_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DYNAMIC_PARAM_EXPR_LIST_TYPE_KEY)) {
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const param_expr> ip(*i);
		ip->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty constructor / allocator for the first pass of 
	deserialization reconstruction.  
 */
persistent*
dynamic_param_expr_list::construct_empty(const int) {
	return new dynamic_param_expr_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_param_expr_list::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const param_expr> ip(*i);
		m.write_pointer(f, ip);
	}
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_param_expr_list::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<param_expr> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			ip->load_object(m);
#endif
		push_back(ip);
	}
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class index_expr method definitions

#if 0
index_expr::index_expr() : object(), persistent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
index_expr::~index_expr() { }
#endif

//-----------------------------------------------------------------------------
// class const_index method definitions

#if 0
const_index::const_index() : index_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index::~const_index() { }
#endif

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
	list<excl_ptr<param_expr> >::const_iterator i = elist.begin();
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
// class pbool_instance_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_instance_reference, 
	SIMPLE_PBOOL_INSTANCE_REFERENCE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pbool_instance_reference::pbool_instance_reference() :
		param_instance_reference(), pbool_expr(), pbool_inst_ref(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instance_reference::pbool_instance_reference(
		const never_ptr<pbool_instance_collection> pi) :
		param_instance_reference(pi->current_collection_state()),
		pbool_expr(), 
		pbool_inst_ref(pi) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
pbool_instance_reference::pbool_instance_reference(
		const never_ptr<pbool_instance_collection> pi,
		excl_ptr<index_list>& i) :
		param_instance_reference(i, pi->current_collection_state()),
		pbool_expr(), 
		pbool_inst_ref(pi) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
pbool_instance_reference::~pbool_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
pbool_instance_reference::get_inst_base(void) const {
	return pbool_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const param_instance_collection>
pbool_instance_reference::get_param_inst_base(void) const {
	return pbool_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pbool_instance_reference)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instance_reference::dump(ostream& o) const {
	return simple_instance_reference::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pbool_instance_reference::hash_string(void) const {
	return simple_instance_reference::hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
pbool_instance_reference::dimensions(void) const {
	return simple_instance_reference::dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_instance_reference::has_static_constant_dimensions(void) const {
	return simple_instance_reference::has_static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list
pbool_instance_reference::static_constant_dimensions(void) const {
	return simple_instance_reference::static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	IMPORTANT: This initialization is only for static analysis and is
	not the actual initialization that takes place during unrolling.  
	\return true if sucessfully initialized with valid expression.  
 */
bool
pbool_instance_reference::initialize(const init_arg_type& i) {
	return pbool_inst_ref->initialize(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_instance_reference::may_be_initialized(void) const {
	return param_instance_reference::may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_instance_reference::must_be_initialized(void) const {
	return param_instance_reference::must_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_instance_reference::is_static_constant(void) const {
	return param_instance_reference::is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_instance_reference::is_loop_independent(void) const {
	return param_instance_reference::is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_instance_reference::is_unconditional(void) const {
	return param_instance_reference::is_unconditional();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Better make sure that this is_static_constant before calling, 
	else will assert-fail.
 */
bool
pbool_instance_reference::static_constant_bool(void) const {
	INVARIANT(is_static_constant());
	return pbool_inst_ref->initial_value()->static_constant_bool();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version specifically asks for one integer value, 
	thus the array indices must be scalar (0-D).  
	\return true if resolution succeeds, else false.
 */
bool
pbool_instance_reference::resolve_value(value_type& i) const {
	// lookup pbool_instance_collection
	if (array_indices) {
		const const_index_list
			indices(array_indices->resolve_index_list());
		if (!indices.empty()) {
			const excl_ptr<multikey_index_type>
				lower = indices.lower_multikey();
			const excl_ptr<multikey_index_type>
				upper = indices.upper_multikey();
			NEVER_NULL(lower);
			NEVER_NULL(upper);
			if (*lower != *upper) {
				cerr << "ERROR: upper != lower" << endl;
				return false;
			}
			return pbool_inst_ref->lookup_value(i, *lower);
		} else {
			cerr << "Unable to resolve array_indices!" << endl;
			return false;
		}
	} else {
		const never_ptr<pbool_scalar>
			scalar_inst(pbool_inst_ref.is_a<pbool_scalar>());
		NEVER_NULL(scalar_inst);
		return scalar_inst->lookup_value(i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l the list in which to accumulate values.
	\return false if there was error.  
 */
bool
pbool_instance_reference::resolve_values_into_flat_list(
		list<value_type>& l) const {
	// base collection must be non-scalar
	INVARIANT(pbool_inst_ref->dimensions);
	const const_index_list
		ranges(resolve_dimensions());
	if (ranges.empty()) {
		cerr << "ERROR: could not unroll values with bad index."
			<< endl;
		return false;
	}
	else	return pbool_inst_ref->lookup_value_collection(
			l, const_range_list(ranges));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the dimensions of the collection in the current state, 
	ONLY IF, the indexed reference to the current state is all valid.  
	Otherwise, returns an empty list, which is interpreted as an error.  

	Really this should be independent of type?
	Except for checking implicit indices...
 */
const_index_list
pbool_instance_reference::resolve_dimensions(void) const {
	// criterion 1: indices (if any) must be resolved to constant values.  
	if (array_indices) {
		const const_index_list
			c_i(array_indices->resolve_index_list());
		if (c_i.empty()) {
			cerr << "ERROR: failed to resolve index list." << endl;
			return c_i;
		}
		// else let c_i remain empty, underspecified
		// check for implicit indices, that sub-arrays are
		// densely packed with the same dimensions.  
		const const_index_list
			r_i(pbool_inst_ref->resolve_indices(c_i));
		if (r_i.empty()) {
			cerr << "ERROR: implicitly resolving index list."
				<< endl;
		}
	}
	return const_index_list();
	// Elsewhere (during assign) check for initialization.  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves a scalar or collective instance reference into a 
	packed array of values.  
	\param c unrolling context.
	\return dense array of values, NULL if error.  
 */
count_ptr<const_param>
pbool_instance_reference::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	if (pbool_inst_ref->dimensions) {
#if 0
		// dimension resolution should depend on current 
		// state of instance collection, not static analysis
		// from compile phase.
		const const_index_list rdim(resolve_dimensions(/*c*/));
		if (rdim.empty())
			return return_type(NULL);
		// else we have fully specified dimensions

		const const_range_list crl(rdim.collapsed_dimension_ranges());
		INVARIANT(!crl.empty());
		// pbool_const_collection::array_type::key_type
		// is a multikey_generic<size_t>
		const count_ptr<pbool_const_collection>
			ret(new pbool_const_collection(crl.resolve_sizes()));
			// no index offset
		NEVER_NULL(ret);

		generic_index_generator_type key_gen(rdim.size());
		// automatic and temporarily allocated
		key_gen.get_lower_corner() = *rdim.lower_multikey();
		key_gen.get_upper_corner() = *rdim.upper_multikey();
		key_gen.initialize();
		bool lookup_err = false;
		pbool_const_collection::iterator coll_iter(ret->begin());
		do {
			// populate the collection with values
			// lookup_value returns true on success, false on error
			if (!pbool_inst_ref->lookup_value(
					*coll_iter, key_gen)) {
				cerr << "ERROR: looking up index " <<
					key_gen << " of pbool collection " <<
					pbool_inst_ref->get_qualified_name() <<
					"." << endl;
				lookup_err = true;
			}
			coll_iter++;			// unsafe, but checked
			key_gen++;
		} while (key_gen != key_gen.get_upper_corner());
		INVARIANT(coll_iter == ret->end());	// sanity check
		if (lookup_err) {
			// discard incomplete results
			cerr << "ERROR: in unroll_resolve-ing "
				"pbool_instance_reference." << endl;
			return return_type(NULL);
		} else {
			// safe up-cast
			return return_type(ret);
		}
#else
		STACKTRACE("pbool_instance_reference::unroll_resolve()");
		cerr << "FANG, write pbool_const_collection!!!" << endl;
		return return_type(NULL);
#endif
	} else {
		// is 0-dimensional, scalar
		value_type _val;
		const never_ptr<pbool_scalar>
			ps(pbool_inst_ref.is_a<pbool_scalar>());
		INVARIANT(ps);
		const bool valid(ps->lookup_value(_val));
		if (!valid) {
			cerr << "ERROR: in unroll_resolve-ing "
				"pbool_instance_reference, "
				"uninitialized value." << endl;
			return return_type(NULL);
		} else
			return return_type(new pbool_const(_val));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.
	\param m the persistent object manager.
 */
void
pbool_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, SIMPLE_PBOOL_INSTANCE_REFERENCE_TYPE_KEY))
{  
	collect_transient_info_base(m);
	pbool_inst_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}
// else already visited
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just allocates with bogus contents, first pass of reconstruction.
 */
persistent*
pbool_instance_reference::construct_empty(const int i) {
	return new pbool_instance_reference();
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.
	\param m the persistent object manager.
 */
void    
pbool_instance_reference::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, pbool_inst_ref);
	write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}
	
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Loads the instance reference from an input stream, translating
	indices to pointers.
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager.
 */
void
pbool_instance_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, pbool_inst_ref);
	NEVER_NULL(pbool_inst_ref);
	const_cast<pbool_instance_collection&>(*pbool_inst_ref).load_object(m);
	load_object_base(m, f);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//-----------------------------------------------------------------------------
// class pbool_instance_reference::assigner method definitions

/**
	Constructor caches the sequence of values for assigning to 
	an integer instance collection.  
 */
pbool_instance_reference::assigner::assigner(const pbool_expr& p) :
		src(p), ranges(), vals() {
	if (src.dimensions()) {
		ranges = src.resolve_dimensions();
		if (ranges.empty()) {
			// if empty list returned, there was an error,
			// because we know that the # dimensions is > 0.
			cerr << "ERROR: assignment unrolling expecting "
				"valid dimensions!" << endl;
			// or throw exception
			THROW_EXIT;
		}
		// load values into cache list as a sequence
		// pass list by reference to a virtual func?
		const bool err = src.resolve_values_into_flat_list(vals);
		if (err) {
			cerr << "ERROR: in flattening integer values." << endl;
			THROW_EXIT;
		}
	} else {	// is just scalar value
		// leave ranges empty
		value_type i;
		if (src.resolve_value(i)) {
			vals.push_back(i);
		} else {
			cerr << "ERROR: resolving scalar integer value!"
				<< endl;
			THROW_EXIT;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns cached list of unrolled values to the destination
	instance collection.  
	\param b the cumulative error status.
	\param p the destination instance reference.  
	\return error (true) if anything goes wrong, or has gone wrong before.  
 */
bool
pbool_instance_reference::assigner::operator() (const value_type b, 
		const pbool_instance_reference& p) const {
	// check dimensions for match first
	if (ranges.empty()) {
		INVARIANT(vals.size() == 1);
		// is scalar assignment, but may be indexed
		const never_ptr<pbool_scalar> 
			scalar_inst(p.pbool_inst_ref.is_a<pbool_scalar>());
		if (scalar_inst) {
			return scalar_inst->assign(vals.front()) || b;
		}
	}
	// else is scalar or array, but must resolve indices
	const const_index_list dim(p.resolve_dimensions());
	if (dim.empty()) {
		cerr << "ERROR: unable to resolve constant dimensions."
			<< endl;
		THROW_EXIT;
		// return true;
	}
	// We are assured that the dimensions of the references
	// are equal, b/c dimensionality is statically checked.  
	// However, ranges may be of different length because
	// of collapsible dimensions.  
	// Compare dim against ranges: sizes of each dimension...
	// but what about collapsed dimensions?
	if (!ranges.empty() && !ranges.equal_dimensions(dim)) {
		// if range.empty(), then there is no need to match dimensions,
		// dimensions must be equal because both src/dest are scalar.
		cerr << "ERROR: resolved indices are not "
			"dimension-equivalent!" << endl;
		ranges.dump(cerr << "got: ");
		dim.dump(cerr << " and: ") << endl;
		THROW_EXIT;
		// return true;
	}
	// else good to continue

	const excl_ptr<index_generator_type>
		key_gen(index_generator_type::
			make_multikey_generator(dim.size()));
	NEVER_NULL(key_gen);
	// automatic and temporarily allocated
	key_gen->get_lower_corner() = *dim.lower_multikey();
	key_gen->get_upper_corner() = *dim.upper_multikey();
	key_gen->initialize();
	list<value_type>::const_iterator list_iter = vals.begin();
	bool assign_err = false;
	// alias for key_gen
	index_generator_type& key_gen_ref = *key_gen;
	do {
		if (p.pbool_inst_ref->assign(key_gen_ref, *list_iter)) {
			cerr << "ERROR: assigning index " << key_gen_ref << 
				" of pbool collection " <<
				p.pbool_inst_ref->get_qualified_name() <<
				"." << endl;
#if 0
			cerr << "\tlower_corner = " <<
				key_gen->get_lower_corner();
			cerr << ", upper_corner = " <<
				key_gen->get_upper_corner() << endl;
			THROW_EXIT;
#endif
			assign_err = true;
		}
		list_iter++;			// unsafe, but checked
		key_gen_ref++;
	} while (key_gen_ref != key_gen_ref.get_upper_corner());
	INVARIANT(list_iter == vals.end());	// sanity check
	return assign_err || b;
}

//=============================================================================
// class pint_instance_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_instance_reference, 
	SIMPLE_PINT_INSTANCE_REFERENCE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pint_instance_reference::pint_instance_reference() :
		param_instance_reference(), pint_expr(), pint_inst_ref(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_reference::pint_instance_reference(
		const never_ptr<pint_instance_collection> pi) :
		param_instance_reference(pi->current_collection_state()),
		pint_expr(), 
		pint_inst_ref(pi) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
pint_instance_reference::pint_instance_reference(
		const never_ptr<pint_instance_collection> pi,
		excl_ptr<index_list>& i) :
		param_instance_reference(i, pi->current_collection_state()),
		pint_expr(), 
		pint_inst_ref(pi) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
pint_instance_reference::~pint_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
pint_instance_reference::get_inst_base(void) const {
	return pint_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const param_instance_collection>
pint_instance_reference::get_param_inst_base(void) const {
	return pint_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_instance_reference)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instance_reference::dump(ostream& o) const {
	return simple_instance_reference::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pint_instance_reference::hash_string(void) const {
	return simple_instance_reference::hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
pint_instance_reference::dimensions(void) const {
	return simple_instance_reference::dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_instance_reference::has_static_constant_dimensions(void) const {
	return simple_instance_reference::has_static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list
pint_instance_reference::static_constant_dimensions(void) const {
	return simple_instance_reference::static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	IMPORTANT: This initialization is only for static analysis and is
	not the actual initialization that takes place during unrolling.  
	\return true if successfully initialized with valid expression.  
 */
bool
pint_instance_reference::initialize(const init_arg_type& i) {
	return pint_inst_ref->initialize(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_instance_reference::may_be_initialized(void) const {
	return param_instance_reference::may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_instance_reference::must_be_initialized(void) const {
	return param_instance_reference::must_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_instance_reference::is_static_constant(void) const {
	return param_instance_reference::is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_instance_reference::is_loop_independent(void) const {
	return param_instance_reference::is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_instance_reference::is_unconditional(void) const {
	return param_instance_reference::is_unconditional();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Better make sure that this is_static_constant before calling, 
	else will assert-fail.
 */
pint_instance_reference::value_type
pint_instance_reference::static_constant_int(void) const {
	INVARIANT(is_static_constant());
	return pint_inst_ref->initial_value()->static_constant_int();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version specifically asks for one integer value, 
	thus the array indices must be scalar (0-D).  
	\return true if resolution succeeds, else false.
 */
bool
pint_instance_reference::resolve_value(value_type& i) const {
	// lookup pint_instance_collection
	if (array_indices) {
		const const_index_list
			indices(array_indices->resolve_index_list());
		if (!indices.empty()) {
			// really should pass indices into ->lookup_values();
			// fix this later...
			const excl_ptr<multikey_index_type>
				lower = indices.lower_multikey();
			const excl_ptr<multikey_index_type>
				upper = indices.upper_multikey();
			NEVER_NULL(lower);
			NEVER_NULL(upper);
			if (*lower != *upper) {
				cerr << "ERROR: upper != lower" << endl;
				return false;
			}
			return pint_inst_ref->lookup_value(i, *lower);
		} else {
			cerr << "Unable to resolve array_indices!" << endl;
			return false;
		}
	} else {
		const never_ptr<pint_scalar>
			scalar_inst(pint_inst_ref.is_a<pint_scalar>());
		NEVER_NULL(scalar_inst);
		return scalar_inst->lookup_value(i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre This is called only if is an indexed (implicit or explicit)
		instance reference, and under no circumstances
		should this be invoked for scalars for which 
		resolve_dimensions() always returns an empty list.  
	\param l the list in which to accumulate values.
	\return false if there was error.  
 */
bool
pint_instance_reference::resolve_values_into_flat_list(
		list<value_type>& l) const {
	// base collection must be non-scalar
	INVARIANT(pint_inst_ref->dimensions);
	const const_index_list
		ranges(resolve_dimensions());
	if (ranges.empty()) {
		cerr << "ERROR: could not unroll values with bad index."
			<< endl;
		return false;
	}
	else	return pint_inst_ref->lookup_value_collection(
			l, const_range_list(ranges));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the dimensions of the collection in the current state, 
	ONLY IF, the indexed reference to the current state is all valid.  
	Otherwise, returns an empty list, which is interpreted as an error.  

	Really this should be independent of type?
	Except for checking implicit indices...

	TODO: add unroll_context argument
 */
const_index_list
pint_instance_reference::resolve_dimensions(void) const {
	// criterion 1: indices (if any) must be resolved to constant values.  
	if (array_indices) {
		const const_index_list
			c_i(array_indices->resolve_index_list());
		if (c_i.empty()) {
			cerr << "ERROR: failed to resolve index list." << endl;
			return c_i;
		}
		// but indices may be underspecified... 
		// check for implicit indices, that sub-arrays are
		// densely packed with the same dimensions.  
		const const_index_list
			r_i(pint_inst_ref->resolve_indices(c_i));
		if (r_i.empty()) {
			cerr << "ERROR: implicitly resolving index list."
				<< endl;
		}
		return r_i;
		// Elsewhere (during assign) check for initialization.  
	}
	else return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves a scalar or collective instance reference into a 
	packed array of values.  
	\param c unrolling context.
	\return dense array of values, NULL if error.  
 */
count_ptr<const_param>
pint_instance_reference::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	if (pint_inst_ref->dimensions) {
		// dimension resolution should depend on current 
		// state of instance collection, not static analysis
		// from compile phase.
		const const_index_list rdim(resolve_dimensions(/*c*/));
		if (rdim.empty())
			return return_type(NULL);
		// else we have fully specified dimensions

		const const_range_list crl(rdim.collapsed_dimension_ranges());
		INVARIANT(!crl.empty());
		// pint_const_collection::array_type::key_type
		// is a multikey_generic<size_t>
		const count_ptr<pint_const_collection>
			ret(new pint_const_collection(crl.resolve_sizes()));
			// no index offset
		NEVER_NULL(ret);

		generic_index_generator_type key_gen(rdim.size());
		// automatic and temporarily allocated
		key_gen.get_lower_corner() = *rdim.lower_multikey();
		key_gen.get_upper_corner() = *rdim.upper_multikey();
		key_gen.initialize();
		bool lookup_err = false;
		pint_const_collection::iterator coll_iter(ret->begin());
		do {
			// populate the collection with values
			// lookup_value returns true on success, false on error
			if (!pint_inst_ref->lookup_value(*coll_iter, key_gen)) {
				cerr << "ERROR: looking up index " <<
					key_gen << " of pint collection " <<
					pint_inst_ref->get_qualified_name() <<
					"." << endl;
				lookup_err = true;
			}
			coll_iter++;			// unsafe, but checked
			key_gen++;
		} while (key_gen != key_gen.get_upper_corner());
		INVARIANT(coll_iter == ret->end());	// sanity check
		if (lookup_err) {
			// discard incomplete results
			cerr << "ERROR: in unroll_resolve-ing "
				"pint_instance_reference." << endl;
			return return_type(NULL);
		} else {
			// safe up-cast
			return return_type(ret);
		}
	} else {
		// is 0-dimensional, scalar
		value_type _val;
		const never_ptr<pint_scalar>
			ps(pint_inst_ref.is_a<pint_scalar>());
		INVARIANT(ps);
		const bool valid(ps->lookup_value(_val));
		if (!valid) {
			cerr << "ERROR: in unroll_resolve-ing "
				"pint_instance_reference, "
				"uninitialized value." << endl;
			return return_type(NULL);
		} else
			return return_type(new pint_const(_val));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Assigns a flat list if integer values to a possibly multidimensional
	slice of integer references.  
	This is considered unsafe, so call this only if dimensions and 
	sizes have been checked.  
 */
bool
pint_instance_reference::assign(const list<value_type>& l) const {
	return pint_inst_ref->unroll_assign(l);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.
	\param m the persistent object manager.
 */
void
pint_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		SIMPLE_PINT_INSTANCE_REFERENCE_TYPE_KEY)) {  
	collect_transient_info_base(m);
	pint_inst_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}
// else already visited
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just allocates with bogus contents, first pass of reconstruction.
 */
persistent*
pint_instance_reference::construct_empty(const int i) {
	return new pint_instance_reference();
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.
	\param m the persistent object manager.
 */
void    
pint_instance_reference::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, pint_inst_ref);
	write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}
	
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Loads the instance reference from an input stream, translating
	indices to pointers.
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager.
 */
void
pint_instance_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, pint_inst_ref);
	NEVER_NULL(pint_inst_ref);
	const_cast<pint_instance_collection&>(*pint_inst_ref).load_object(m);
	load_object_base(m, f);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//-----------------------------------------------------------------------------
// class pint_instance_reference::assigner method definitions

/**
	Constructor caches the sequence of values for assigning to 
	an integer instance collection.  
 */
pint_instance_reference::assigner::assigner(const pint_expr& p) :
		src(p), ranges(), vals() {
	if (src.dimensions()) {
		ranges = src.resolve_dimensions();
		if (ranges.empty()) {
			// if empty list returned, there was an error,
			// because we know that the # dimensions is > 0.
			cerr << "ERROR: assignment unrolling expecting "
				"valid dimensions!" << endl;
			// or throw exception
			THROW_EXIT;
		}
		// load values into cache list as a sequence
		// pass list by reference to a virtual func?
		const bool err = src.resolve_values_into_flat_list(vals);
		if (err) {
			cerr << "ERROR: in flattening integer values." << endl;
			THROW_EXIT;
		}
	} else {	// is just scalar value
		// leave ranges empty
		value_type i;
		if (src.resolve_value(i)) {
			vals.push_back(i);
		} else {
			cerr << "ERROR: resolving scalar integer value!"
				<< endl;
			THROW_EXIT;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns cached list of unrolled values to the destination
	instance collection.  
	\param b the cumulative error status.
	\param p the destination instance reference.  
	\return error (true) if anything goes wrong, or has gone wrong before.  
 */
bool
pint_instance_reference::assigner::operator() (const bool b, 
		const pint_instance_reference& p) const {
	// check dimensions for match first
	if (ranges.empty()) {
		INVARIANT(vals.size() == 1);
		// is scalar assignment, but may be indexed
		const never_ptr<pint_scalar> 
			scalar_inst(p.pint_inst_ref.is_a<pint_scalar>());
		if (scalar_inst) {
			return scalar_inst->assign(vals.front()) || b;
		}
	}
	// else is scalar or array, but must resolve indices
	const const_index_list dim(p.resolve_dimensions());
	if (dim.empty()) {
		cerr << "ERROR: unable to resolve constant dimensions."
			<< endl;
		THROW_EXIT;
		// return true;
	}
	// We are assured that the dimensions of the references
	// are equal, b/c dimensionality is statically checked.  
	// However, ranges may be of different length because
	// of collapsible dimensions.  
	// Compare dim against ranges: sizes of each dimension...
	// but what about collapsed dimensions?
	if (!ranges.empty() && !ranges.equal_dimensions(dim)) {
		// if range.empty(), then there is no need to match dimensions,
		// dimensions must be equal because both src/dest are scalar.
		cerr << "ERROR: resolved indices are not "
			"dimension-equivalent!" << endl;
		ranges.dump(cerr << "got: ");
		dim.dump(cerr << " and: ") << endl;
		THROW_EXIT;
		// return true;
	}
	// else good to continue

	const excl_ptr<index_generator_type>
		key_gen(index_generator_type::
			make_multikey_generator(dim.size()));
	NEVER_NULL(key_gen);
	// automatic and temporarily allocated
	key_gen->get_lower_corner() = *dim.lower_multikey();
	key_gen->get_upper_corner() = *dim.upper_multikey();
	key_gen->initialize();
	list<value_type>::const_iterator list_iter = vals.begin();
	bool assign_err = false;
	// alias for key_gen
	index_generator_type& key_gen_ref = *key_gen;
	do {
		if (p.pint_inst_ref->assign(key_gen_ref, *list_iter)) {
			cerr << "ERROR: assigning index " << key_gen_ref << 
				" of pint collection " <<
				p.pint_inst_ref->get_qualified_name() <<
				"." << endl;
#if 0
			cerr << "\tlower_corner = " <<
				key_gen->get_lower_corner();
			cerr << ", upper_corner = " <<
				key_gen->get_upper_corner() << endl;
			THROW_EXIT;
#endif
			assign_err = true;
		}
		list_iter++;			// unsafe, but checked
		key_gen_ref++;
	} while (key_gen_ref != key_gen_ref.get_upper_corner());
	INVARIANT(list_iter == vals.end());	// sanity check
	return assign_err || b;
}

//=============================================================================
// class pint_const method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_const, CONST_PINT_TYPE_KEY)

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
ostream&
pint_const::dump(ostream& o) const {
	return o << hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pint_const::hash_string(void) const {
	ostringstream o;
	o << val;
	return o.str();
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
bool
pint_const::resolve_value(value_type& i) const {
	i = val;
	return true;
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
bool
pint_const::resolve_values_into_flat_list(list<value_type>& l) const {
	l.push_back(val);
	return true;
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
count_ptr<const_param>
pint_const::unroll_resolve(const unroll_context& c) const {
	return count_ptr<const_param>(new pint_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does nothing, no pointers to visit.  
 */
void
pint_const::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, CONST_PINT_TYPE_KEY);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_const::construct_empty(const int i) {
	return new pint_const(0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);		// wasteful
	write_value(f, val);
	WRITE_OBJECT_FOOTER(f);			// wasteful
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);		// wasteful
	read_value(f, val);
	STRIP_OBJECT_FOOTER(f);			// wasteful
}
// else already visited
}

//=============================================================================
// class pint_const_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_const_collection, 
		CONST_PINT_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const_collection::pint_const_collection(const size_t d) :
		pint_expr(), const_param(), values(d) {
	INVARIANT(d <= 4);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const_collection::pint_const_collection(const array_type::key_type& k) :
		pint_expr(), const_param(), values(k) {
	INVARIANT(k.size() <= 4);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const_collection::~pint_const_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_const_collection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_const_collection::dump(ostream& o) const {
	return values.dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pint_const_collection::hash_string(void) const {
	cerr << "FANG, write pint_const_collection::hash_string()!" << endl;
	THROW_EXIT;
	return string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
pint_const_collection::dimensions(void) const {
	return values.dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return heap-allocated copy.  
 */
count_ptr<const const_param>
pint_const_collection::static_constant_param(void) const {
	typedef	count_ptr<const const_param>	return_type;
	return return_type(new pint_const_collection(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A packed array of constants always has constant dimensions.  
 */
bool
pint_const_collection::has_static_constant_dimensions(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return dense range list representation of the instance
		collection if it is indeed compact, else an empty list.  
	No dimensions or indices are implicit.  
 */
const_range_list
pint_const_collection::static_constant_dimensions(void) const {
	const_range_list ret;
	const array_type::key_type first(values.first_key());
	const array_type::key_type last(values.last_key());
	array_type::key_type::const_iterator f_iter = first.begin();
	array_type::key_type::const_iterator l_iter = last.begin();
	for ( ; f_iter != first.end(); f_iter++, l_iter++) {
		ret.push_back(const_range(*f_iter, *l_iter));
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if expressions may be equivalent (conservative).
 */
bool
pint_const_collection::may_be_equivalent(const param_expr& e) const {
	const never_ptr<const pint_const_collection>
		p(IS_A(const pint_const_collection*, &e));
	if (p) {
		// precisely
		return (values.dimensions() == p->values.dimensions() &&
			values.size() == p->values.size() &&
			values == p->values);
	} else {
		// conservatively
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if the expressions must be equivalent, 
		conservatively false.
 */
bool
pint_const_collection::must_be_equivalent(const param_expr& e) const {
	const never_ptr<const pint_const_collection>
		p(IS_A(const pint_const_collection*, &e));
	if (p) {
		// precisely
		return (values.dimensions() == p->values.dimensions() &&
			values.size() == p->values.size() &&
			values == p->values);
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_const_collection::resolve_value(value_type& ) const {
	cerr << "Never supposed to call pint_const_collection::resolve_value()."
		<< endl;
	THROW_EXIT;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const_collection::value_type
pint_const_collection::static_constant_int(void) const {
	cerr << "Never supposed to call pint_const_collection::static_constant_int()." << endl;
	THROW_EXIT;
	return -1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Straight copy of constant values to list.  
	May become obsolete in future.  
 */
bool
pint_const_collection::resolve_values_into_flat_list(
		list<value_type>& l) const {
	copy(values.begin(), values.end(), back_inserter(l));
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
pint_const_collection::resolve_dimensions(void) const {
	const_index_list ret;
	const array_type::key_type first(values.first_key());
	const array_type::key_type last(values.last_key());
	array_type::key_type::const_iterator f_iter = first.begin();
	array_type::key_type::const_iterator l_iter = last.begin();
	for ( ; f_iter != first.end(); f_iter++, l_iter++) {
		ret.push_back(
			// is reference-counted pointer type
			const_index_list::const_index_ptr_type(
				new const_range(*f_iter, *l_iter)));
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const_param>
pint_const_collection::unroll_resolve(const unroll_context& c) const {
	return count_ptr<const_param>(new pint_const_collection(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does nothing because there are no pointers to collect and visit.  
 */
void
pint_const_collection::collect_transient_info(
		persistent_object_manager& m) const {
	const char s = values.dimensions();
	INVARIANT(s >= 0);
	INVARIANT(s <= 4);
	m.register_transient_object(this, CONST_PINT_COLLECTION_TYPE_KEY, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_const_collection::construct_empty(const int d) {
	INVARIANT(d >= 0);
	INVARIANT(d <= 4);
	return new pint_const_collection(d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const_collection::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);		// wasteful
	values.write(f);
	WRITE_OBJECT_FOOTER(f);			// wasteful
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const_collection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);		// wasteful
	values.read(f);
	STRIP_OBJECT_FOOTER(f);			// wasteful
}
// else already loaded
}

//=============================================================================
// class pbool_const method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_const, CONST_PBOOL_TYPE_KEY)

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
pbool_const::dump(ostream& o) const {
	return o << hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pbool_const::hash_string(void) const {
	return (val) ? "true" : "false";
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
bool
pbool_const::resolve_value(value_type& i) const {
	i = val;
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_const::resolve_values_into_flat_list(list<value_type>& l) const {
	l.push_back(val);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const_param>
pbool_const::unroll_resolve(const unroll_context& c) const {
	return count_ptr<const_param>(new pbool_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, CONST_PBOOL_TYPE_KEY);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pbool_const::construct_empty(const int i) {
	return new pbool_const(0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);		// wasteful
	write_value(f, val);
	WRITE_OBJECT_FOOTER(f);			// wasteful
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);		// wasteful
	read_value(f, val);
	STRIP_OBJECT_FOOTER(f);			// wasteful
}
// else already visited
}

//=============================================================================
// class pint_unary_expr method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_unary_expr, PINT_UNARY_EXPR_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
pint_unary_expr::dump(ostream& o) const {
	// parentheses? check operator precedence
	return ex->dump(o << op);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pint_unary_expr::hash_string(void) const {
//	return op +ex->hash_string();
	return ex->hash_string() +op;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_unary_expr::is_static_constant(void) const {
	return ex->is_static_constant();
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
pint_unary_expr::static_constant_int(void) const {
	// depends on op
	return - ex->static_constant_int();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns resolved value of negation expression.  
 */
bool
pint_unary_expr::resolve_value(value_type& i) const {
	value_type j;
	NEVER_NULL(ex);
	const bool ret = ex->resolve_value(j);
	i = -j;		// regardless of ret
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false if there is error in resolving.
 */
bool
pint_unary_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type i = 0;
	const bool ret = resolve_value(i);
	l.push_back(i);		// regardless of validity
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
		count_ptr<pint_const> pc(ret.is_a<pint_const>());
		INVARIANT(pc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(
			new pint_const(- pc->static_constant_int()));
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
if (!m.register_transient_object(this, PINT_UNARY_EXPR_TYPE_KEY)) {
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_unary_expr::construct_empty(const int i) {
	return new pint_unary_expr();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_unary_expr::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, op);
	m.write_pointer(f, ex);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_unary_expr::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	read_value(f, const_cast<op_type&>(op));
	m.read_pointer(f, ex);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class pbool_unary_expr method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_unary_expr, 
	PBOOL_UNARY_EXPR_TYPE_KEY)

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
pbool_unary_expr::dump(ostream& o) const {
	// parentheses?
	return ex->dump(o << op);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pbool_unary_expr::hash_string(void) const {
	return ex->hash_string() +op;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::is_static_constant(void) const {
	return ex->is_static_constant();
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
pbool_unary_expr::static_constant_bool(void) const {
	return !ex->static_constant_bool();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
pbool_unary_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::resolve_value(value_type& i) const {
	value_type b;
	const bool ret = ex->resolve_value(b);
	i = !b;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_unary_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type b;
	const bool ret = resolve_value(b);
	l.push_back(b);
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
		count_ptr<pbool_const> pc(ret.is_a<pbool_const>());
		INVARIANT(pc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(
			new pbool_const(!pc->static_constant_bool()));
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
if (!m.register_transient_object(this, PBOOL_UNARY_EXPR_TYPE_KEY)) {
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pbool_unary_expr::construct_empty(const int i) {
	return new pbool_unary_expr();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_unary_expr::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, op);
	m.write_pointer(f, ex);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_unary_expr::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	read_value(f, const_cast<op_type&>(op));
	m.read_pointer(f, ex);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class arith_expr method definitions

// static member initializations (order matters!)

DEFAULT_PERSISTENT_TYPE_REGISTRATION(arith_expr, ARITH_EXPR_TYPE_KEY)

const plus<pint_value_type, pint_value_type>		arith_expr::adder;
const minus<pint_value_type, pint_value_type>		arith_expr::subtractor;
const multiplies<pint_value_type, pint_value_type>	arith_expr::multiplier;
const divides<pint_value_type, pint_value_type>		arith_expr::divider;
const modulus<pint_value_type, pint_value_type>		arith_expr::remainder;

const arith_expr::op_map_type
arith_expr::op_map;

const arith_expr::reverse_op_map_type
arith_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
arith_expr::op_map_size = arith_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
arith_expr::op_map_register(const char c, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[c] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered arithmetic operators.  
 */
size_t
arith_expr::op_map_init(void) {
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
arith_expr::arith_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
arith_expr::~arith_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
arith_expr::arith_expr(const count_ptr<const pint_expr>& l, const char o,
		const count_ptr<const pint_expr>& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(arith_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
arith_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
arith_expr::hash_string(void) const {
	return lx->hash_string() +reverse_op_map[op] +rx->hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
arith_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
arith_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
arith_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
arith_expr::value_type
arith_expr::static_constant_int(void) const {
	const arg_type a = lx->static_constant_int();
	const arg_type b = rx->static_constant_int();
#if 0
	switch(op) {
		case '+':	return a + b;
		case '-':	return a - b;
		case '*':	return a / b;
		case '/':	return a * b;
		case '%':	return a % b;
		default:
			cerr << "FATAL: Unexpected operator \'" << op <<
				"\', aborting." << endl;
			DIE; return 0;
	}
#else
	// Oooooh, virtual operator dispatch!
	return (*op)(a,b);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
arith_expr::resolve_value(value_type& i) const {
	arg_type a, b;
	NEVER_NULL(lx);	NEVER_NULL(rx);
	const bool lret = lx->resolve_value(a);
	const bool rret = rx->resolve_value(b);
	if (!lret) {
		cerr << "ERROR: resolving left operand of: ";
		dump(cerr) << endl;
		return false;
	} else if (!rret) {
		cerr << "ERROR: resolving right operand of: ";
		dump(cerr) << endl;
		return false;
	}
#if 0
	switch(op) {
		case '+':	i = a + b;	break;
		case '-':	i = a - b;	break;
		case '*':	i = a / b;	break;
		case '/':	i = a * b;	break;
		case '%':	i = a % b;	break;
		default:	
			cerr << "FATAL: Unexpected operator \'" << op <<
				"\', aborting." << endl;
			DIE; return false;
	}
#else
	// Oooooh, virtual operator dispatch!
	i = (*op)(a,b);
#endif
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false if there is error in resolving.
 */
bool
arith_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type i = 0;
	const bool ret = resolve_value(i);
	l.push_back(i);		// regardless of validity
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty list, signifying a scalar value, because all 
		expressions operations only work on scalars.  
 */
const_index_list
arith_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pint_const of the resolved value.
 */
count_ptr<const_param>
arith_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type lex(lx->unroll_resolve(c));
	const return_type rex(rx->unroll_resolve(c));
	if (lex && rex) {
		const count_ptr<pint_const> lpc(lex.is_a<pint_const>());
		const count_ptr<pint_const> rpc(rex.is_a<pint_const>());
		INVARIANT(lpc);
		INVARIANT(rpc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(new pint_const(
			(*op)(lpc->static_constant_int(), 
				rpc->static_constant_int())));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
arith_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, ARITH_EXPR_TYPE_KEY)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
arith_expr::construct_empty(const int i) {
	return new arith_expr();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
arith_expr::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
//	write_value(f, op);
	write_value(f, reverse_op_map[op]);	// writes a character
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
arith_expr::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	{
	char o;
	read_value(f, o);
	op = op_map[o];
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class relational_expr method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(relational_expr, RELATIONAL_EXPR_TYPE_KEY)

// static member initializations (order matters!)

const equal_to<pbool_value_type, pint_value_type>
relational_expr::op_equal_to;

const not_equal_to<pbool_value_type, pint_value_type>
relational_expr::op_not_equal_to;

const less<pbool_value_type, pint_value_type>
relational_expr::op_less;

const greater<pbool_value_type, pint_value_type>
relational_expr::op_greater;

const less_equal<pbool_value_type, pint_value_type>
relational_expr::op_less_equal;

const greater_equal<pbool_value_type, pint_value_type>
relational_expr::op_greater_equal;

const relational_expr::op_map_type
relational_expr::op_map;

const relational_expr::reverse_op_map_type
relational_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
relational_expr::op_map_size = relational_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
relational_expr::op_map_register(const string& s, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[s] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered relationalmetic operators.  
 */
size_t
relational_expr::op_map_init(void) {
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
relational_expr::relational_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
relational_expr::~relational_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
relational_expr::relational_expr(const count_ptr<const pint_expr>& l,
		const string& o, const count_ptr<const pint_expr>& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(relational_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
relational_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
relational_expr::hash_string(void) const {
	return lx->hash_string() +reverse_op_map[op] +rx->hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
relational_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
relational_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
relational_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return result of resolved comparison.  
 */
relational_expr::value_type
relational_expr::static_constant_bool(void) const {
	const arg_type a = lx->static_constant_int();
	const arg_type b = rx->static_constant_int();
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
relational_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: switch on relational expression operator.  
 */
bool
relational_expr::resolve_value(value_type& i) const {
	arg_type li, ri;
	const bool l_ret = lx->resolve_value(li);
	const bool r_ret = rx->resolve_value(ri);
	// SWITCH
	i = (*op)(li, ri);
	return l_ret && r_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Appends the value to the list, even if there was an error.
	\param l the cumulative list of values.
	\return error status
 */
bool
relational_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type b;
	const bool ret = resolve_value(b);
	l.push_back(b);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pbool_const of the resolved value.
 */
count_ptr<const_param>
relational_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type lex(lx->unroll_resolve(c));
	const return_type rex(rx->unroll_resolve(c));
	if (lex && rex) {
		const count_ptr<pint_const> lpc(lex.is_a<pint_const>());
		const count_ptr<pint_const> rpc(rex.is_a<pint_const>());
		INVARIANT(lpc);
		INVARIANT(rpc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(new pbool_const(
			(*op)(lpc->static_constant_int(), 
				rpc->static_constant_int())));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
relational_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, RELATIONAL_EXPR_TYPE_KEY)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
relational_expr::construct_empty(const int i) {
	return new relational_expr();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
relational_expr::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
//	write_value(f, op);
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
relational_expr::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
//	read_value(f, op);
	{
	string s;
	read_value(f, s);
	op = op_map[s];
	NEVER_NULL(op);
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class logical_expr method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(logical_expr, LOGICAL_EXPR_TYPE_KEY)

// static member initializations (order matters!)

const util::logical_and<pbool_value_type, pbool_value_type>
logical_expr::op_and;

const util::logical_or<pbool_value_type, pbool_value_type>
logical_expr::op_or;

const util::logical_xor<pbool_value_type, pbool_value_type>
logical_expr::op_xor;

const logical_expr::op_map_type
logical_expr::op_map;

const logical_expr::reverse_op_map_type
logical_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
logical_expr::op_map_size = logical_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
logical_expr::op_map_register(const string& s, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[s] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered logicalmetic operators.  
 */
size_t
logical_expr::op_map_init(void) {
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
logical_expr::logical_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
logical_expr::~logical_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
logical_expr::logical_expr(const count_ptr<const pbool_expr>& l,
		const string& o, const count_ptr<const pbool_expr>& r) :
		lx(l), rx(r), op(op_map[o]) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(logical_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
logical_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
logical_expr::hash_string(void) const {
	return lx->hash_string() +reverse_op_map[op] +rx->hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
logical_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
logical_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
logical_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must be truly compile-time constant.
 */
logical_expr::value_type
logical_expr::static_constant_bool(void) const {
	const arg_type a = lx->static_constant_bool();
	const arg_type b = rx->static_constant_bool();
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
logical_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: switch on logical expression operator.  
 */
bool
logical_expr::resolve_value(value_type& i) const {
	arg_type lb, rb;
	const bool l_ret = lx->resolve_value(lb);
	const bool r_ret = rx->resolve_value(rb);
	i = (*op)(lb, rb);
	return l_ret && r_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
logical_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	arg_type b;
	const bool ret = resolve_value(b);
	l.push_back(b);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pbool_const of the resolved value.
 */
count_ptr<const_param>
logical_expr::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	// should return a pint_const
	// maybe make a pint_const version to avoid casting
	const return_type lex(lx->unroll_resolve(c));
	const return_type rex(rx->unroll_resolve(c));
	if (lex && rex) {
		const count_ptr<pbool_const> lpc(lex.is_a<pbool_const>());
		const count_ptr<pbool_const> rpc(rex.is_a<pbool_const>());
		INVARIANT(lpc);
		INVARIANT(rpc);
		// would like to just modify pc, but pint_const's 
		// value_type is const :( consider un-const-ing it...
		return return_type(new pbool_const(
			(*op)(lpc->static_constant_bool(), 
				rpc->static_constant_bool())));
	} else {
		// there is an error in at least one sub-expression
		// discard intermediate result
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
logical_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, LOGICAL_EXPR_TYPE_KEY)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
logical_expr::construct_empty(const int i) {
	return new logical_expr();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
logical_expr::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
//	write_value(f, op);
//	write_string(f, reverse_op_map[op]);
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
logical_expr::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
//	read_value(f, op);
	{
	string s;
//	read_string(f, s);
	read_value(f, s);
	op = op_map[s];
	NEVER_NULL(op);
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class pint_range method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_range, DYNAMIC_RANGE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pint_range::pint_range() :
		range_expr(), lower(NULL), upper(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// Default destructor.  
pint_range::~pint_range() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct a range given one integer expression, 
	implicitly from 0 too expr -1, inclusive.
 */
pint_range::pint_range(const count_ptr<const pint_expr>& n) :
		range_expr(),
		lower(new pint_const(0)),
		upper(new arith_expr(n, '-', 
			count_ptr<const pint_expr>(new pint_const(1)))) {
	NEVER_NULL(n);
	NEVER_NULL(lower);
	NEVER_NULL(upper);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_range::pint_range(const count_ptr<const pint_expr>& l, 
		const count_ptr<const pint_expr>& u) :
		range_expr(), lower(l), upper(u) {
	NEVER_NULL(lower);
	NEVER_NULL(upper);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_range::pint_range(const pint_range& pr) :
		object(), persistent(), index_expr(), range_expr(), 
		// virtual base object() needs to be explicitly invoked
		// in this copy constructor
		lower(pr.lower), upper(pr.upper) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_range)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_range::dump(ostream& o) const {
	return o << '[' << hash_string() << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
pint_range::hash_string(void) const {
	return lower->hash_string() + ".." +upper->hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Range is sane if lower <= upper.
	If expressions are not constant, then conservatively return true.  
 */
bool
pint_range::is_sane(void) const {
	if (is_static_constant()) {
		return lower->static_constant_int() <=
			upper->static_constant_int();
	}
	else return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_range::is_static_constant(void) const {
	return lower->is_static_constant() && upper->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range
pint_range::static_constant_range(void) const {
	return const_range(lower->static_constant_int(), 
		upper->static_constant_int());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_range::resolve_range(const_range& r) const {
	if (!lower->resolve_value(r.first))	return false;
	if (!upper->resolve_value(r.second))	return false;
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_range::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DYNAMIC_RANGE_TYPE_KEY)) {
	lower->collect_transient_info(m);
	upper->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_range::construct_empty(const int i) {
	return new pint_range();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_range::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, lower);
	m.write_pointer(f, upper);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_range::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, lower);
	m.read_pointer(f, upper);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class const_range method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(const_range, CONST_RANGE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default empty constructor. 
	Makes an invalid range.  
 */
const_range::const_range() : range_expr(), const_index(), parent_type(0,-1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/** Protected internal constructor. */
const_range::const_range(const interval_type& i) :
		range_expr(), const_index(), 
		parent(i.empty() ? 0 : i.begin()->first,
			i.empty() ? -1 : i.begin()->second) {
	if (!i.empty())
		INVARIANT(upper() >= lower());		// else what!?!?
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit constructor of a dense range from 0 to N-1.  
	\param n must be > 0, else assertion will fail.
 */
const_range::const_range(const pint_value_type n) :
		range_expr(), const_index(), 
		parent_type(0, n-1) {
	INVARIANT(upper() >= lower());		// else what!?!?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit constructor of a dense range from 0 to N-1.  
	\param n must be > 0, else assertion will fail.
 */
const_range::const_range(const pint_const& n) :
		range_expr(), const_index(), 
		parent_type(0, n.static_constant_int() -1) {
	INVARIANT(upper() >= lower());		// else what!?!?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should both l and u be non-negative too?
	\param l is lower bound, inclusive.  
	\param u is upper bound, inclusive, and must be >= l.  
 */
const_range::const_range(const pint_value_type l, const pint_value_type u) :
		range_expr(), const_index(), 
		parent_type(l, u) {
	INVARIANT(upper() >= lower());		// else what!?!?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private unsafe constructor, for internal use only.  
	Should both l and u be non-negative too?
	\param l is lower bound, inclusive.  
	\param u is upper bound, inclusive, and must be >= l.  
	\param b is unused bogus parameter to distinguish from safe version.  
 */
const_range::const_range(const pint_value_type l, const pint_value_type u,
		const bool b) :
		range_expr(), const_index(), 
		parent_type(l, u) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** standard copy constructor */
const_range::const_range(const const_range& r) :
		object(), 
		persistent(), 
		index_expr(),
		range_expr(), 
		const_index(), 
		parent_type(r) {
	// assert check range?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range::const_range(const parent_type& r) :
		object(), 
		persistent(), 
		index_expr(),
		range_expr(), 
		const_index(), 
		parent_type(r) {
	// assert check range?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_range)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_range::dump(ostream& o) const {
	if (empty())
		return o << "[]";
	else
		return o << "[" << lower() << ".." << upper() << "]";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
const_range::hash_string(void) const {
	ostringstream o;
	dump(o);		// will include braces?
	return o.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns whether or not two intervals overlap.  
	\return overlap range.
 */
const_range
const_range::static_overlap(const const_range& r) const {
#if 0
	// DEBUG
	cerr << "In const_range::static_overlap with this = "
		<< this << " .interval = " << interval << endl;
	r.dump(cerr << "const const_range& r = ") << endl;
#endif

	interval_type temp(first, second);
	interval_type temp2(r.first, r.second);
	temp.intersect(temp2);

	const const_range ret(temp.empty() ? 0 : temp.begin()->first,
		temp.empty() ? -1 : temp.begin()->second, true);
	if (!temp.empty())
		INVARIANT(ret.upper() >= ret.lower());		// else what!?!?
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_range::operator == (const const_range& c) const {
	return (first == c.first) && (second == c.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Presumably if this was successfully constructed, then it
	passed the assertion.  No need to recheck.  
 */
bool
const_range::is_sane(void) const {
	return !empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_value_type
const_range::lower_bound(void) const {
	return first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_value_type
const_range::upper_bound(void) const {
	return second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_range::resolve_range(const_range& r) const {
	r = *this;
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return deep copy of this constant range, always succeeds.  
 */
count_ptr<const_index>
const_range::resolve_index(void) const {
	return count_ptr<const_index>(new const_range(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the index to compare against, may be a range or int.  
	\return true if the ranges spanned are equal in size, 
		no necessarily equal in index.  
 */
bool
const_range::range_size_equivalent(const const_index& i) const {
	const never_ptr<const const_range> r(IS_A(const const_range*, &i));
	return r && (r->second - r->first == second -first);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_range::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, CONST_RANGE_TYPE_KEY);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
const_range::construct_empty(const int i) {
	return new const_range();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_range::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, first);
	write_value(f, second);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_range::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	read_value(f, first);
	read_value(f, second);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class range_expr method definitions

#if 0
range_expr::range_expr() : index_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
range_expr::~range_expr() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
range_expr::dump(ostream& o) const {
	return o << hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a resolve constant range, or NULL if resolution fails.  
 */
count_ptr<const_index>
range_expr::resolve_index(void) const {
	typedef	count_ptr<const_index>	return_type;
	const_range tmp;
	return (resolve_range(tmp)) ?
		return_type(new const_range(tmp)) :
		return_type(NULL);
}

//=============================================================================
// class range_expr_list method definitions

#if 0
range_expr_list::range_expr_list() : object(), persistent() {
}
#endif

//=============================================================================
// class const_range_list method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(const_range_list, 
	CONST_RANGE_LIST_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list::const_range_list() : range_expr_list(), list_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implicit conversion.  
 */
const_range_list::const_range_list(const list_type& l) :
		range_expr_list(), list_type(l) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit conversion from an index list to a range list.  
	Only available for constants, of course.
	Converts x[n] to x[n..n].
	Useful for static range-checking of indices.  
 */
const_range_list::const_range_list(const const_index_list& i) :
		range_expr_list(), list_type() {
	const_index_list::const_iterator j = i.begin();
	for ( ; j!=i.end(); j++) {
		const count_ptr<const_index> k(*j);
		NEVER_NULL(k);
		const count_ptr<pint_const> p(k.is_a<pint_const>());
		const count_ptr<const_range> r(k.is_a<const_range>());
		if (p) {
			const int min_max = p->static_constant_int();
			push_back(const_range(min_max, min_max));	// copy
		} else {
			NEVER_NULL(r);
			push_back(*r);		// deep copy
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list::~const_range_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_range_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_range_list::dump(ostream& o) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++)
		i->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
const_range_list::size(void) const {
	return list_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for overlap between two static multidimensional index ranges.  
	If argument is actually dynamic, can only conservatively return false.  
	\return a const_range_list which is either false if there's no overlap, 
		or the valid overlap range if there is overlap.  
 */
const_range_list
const_range_list::static_overlap(const range_expr_list& r) const {
#if 0
	// DEBUG
	cerr << "In const_range_list::static_overlap with this = "
		<< this << endl;
	r.dump(cerr << "const range_expr_list& r = ") << endl;
#endif
	const const_range_list* s = IS_A(const const_range_list*, &r);
	const_range_list ret;	// initially empty
	if (s) {
		// dimensionality should match, or else!
		INVARIANT(size() == s->size());    
		const_iterator i = begin();
		const_iterator j = s->begin();
		for ( ; i!=end(); i++, j++) {
			// check for index overlap in ALL dimensions
			const const_range& ir = *i;
			const const_range& jr = *j;
			const_range o(ir.static_overlap(jr));
			ret.push_back(o);
			if (!o.empty())
				continue;
			else return const_range_list();
			// if there is any dimension without overlap, false
		}
		// else there is some overlap in all dimensions
		return ret;
	} else {	// argument is dynamic, not static
		// conservatively return false
		// overlap is possible but not definite, can't reject now.  
		return const_range_list();		// empty list
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Takes a list of ranges and converts back to indices, using a 
	reference index list, resembling the original index list.  
	Useful for converting results from multidimensional_sparse_set
	queries (range lists) back into index form where needed.  
	\param il the reference index list.
 */
const_index_list
const_range_list::revert_to_indices(const const_index_list& il) const {
	// let's actually bother to check consistency of the reference list
	const_index_list ret;
	// have a better idea... punt this for now
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
UNNECESSARY
/**
	\param il should be a reference index list for this range list.  
	\return copy of this range list with fewer dimensions, 
		collapsing the dimensions that were indexed by single ints.  
 */
const_range_list
const_range_list::collapsed_dimension_ranges(
		const const_index_list& il) const {
	const_index_list ret;
	INVARIANT(size() == il.size());
	const_iterator i = begin();
	const_index_list::const_iterator j = il.begin();
	for ( ; j!=il.end(); i++, j++) {
		const count_ptr<const pint_const>	// or pint_const
			pi(j->is_a<pint_const>());
		if (pi) {
			INVARIANT(i != end());
			INVARIANT(i->first == pi->static_constant_int());
			INVARIANT(i->first == i->second);
		} else {
			const count_ptr<const const_range>
				pr(j->is_a<const_range>());
			NEVER_NULL(pr);
			INVARIANT(pr->first == i->first);
			INVARIANT(pr->second == i->second);
			ret.push_back(*pr);
		}
	}
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ABANDONING, see comments within
/**
	Collapses the multidimensional range list using an
	reference index list as an argument.  
	For each index element of the argument that is an int, 
	not a range, shorten the range list in that dimension.  
	Make sure that the range is unity, as a sanity check.  
	Warning: list modifies itself by deleting elements, 
		may break with old shitty compiler libraries, 
		depending on the implementation.  
	\param il is the reference index list.  
 */
void
const_range_list::collapse_dimensions_wrt_indices(const const_index_list& il) {
	INVARIANT(size() >= il.size());
	iterator i = begin();
	const_index_list::const_iterator j = il.begin();
	for ( ; j!=il.end(); i++, j++) {
		const count_ptr<const pint_const>	// or pint_const
			pi(j->is_a<pint_const>());
		if (pi) {
			INVARIANT(i != end());
			INVARIANT(i->first == i->second);
#if 1
			i = erase(i);	// potential libstdc++ bug? don't trust

//			iterator ipp(i);
//			ipp++;
//			erase(i, ipp);	// doesn't work either...
#else
			// shit code here... virtual erasing:
			iterator iw(i);
			const_iterator ir(iw);
			ir++;		// read one-ahead
			for ( ; ir!=end(); iw++, ir++) {
				iw->first = ir->first;
				iw->second = ir->second;
			}
			iw->first = iw->second = -666;	// garbage
			erase(iw);	// be-tail
#endif
		} else {
			const count_ptr<const const_range>
				pr(j->is_a<const_range>());
			NEVER_NULL(pr);
			INVARIANT(pr->first == i->first);
#if 1
			if (pr->second != i->second) {
				cerr << "pr->second = " << pr->second << endl;
				cerr << "i->second = " << i->second << endl;
			}
			// following assertion produces different results
			// on gcc-3.2,3.3 vs 3.4
#endif
			INVARIANT(pr->second == i->second);
		}
	}
	// any remaining indices are kept
}
ABANDONING
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Size equality of two multidimensional ranges.  
	Also reports error in size mismatch to stderr.  
 */
bool
const_range_list::is_size_equivalent(const const_range_list& c) const {
	if (size() != c.size())
		return false;
	// else check
	int dim = 1;
	const_iterator i = begin();
	const_iterator j = c.begin();
	for ( ; i!=end(); i++, j++, dim++) {
		const int ldiff = i->second -i->first;
		const int rdiff = j->second -j->first;
		if (ldiff != rdiff) {
			cerr << "Size of dimension " << dim <<
				" does not match!  got: " << ldiff+1 <<
				" and " << rdiff+1 << "." << endl;
			return false;
		}
	}
	INVARIANT(j == c.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether two multidimensional range lists are identical.  
	Not just whether or not the size of the spanned ranges are equal.  
 */
bool
const_range_list::operator == (const const_range_list& c) const {
	if (size() != c.size())
		return false;
	// else check
	const_iterator i = begin();
	const_iterator j = c.begin();
	for ( ; i!=end(); i++, j++) {
		if (*i != *j)
			return false;
	}
	INVARIANT(j == c.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_range_list::resolve_ranges(const_range_list& r) const {
	r = *this;
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<multikey_index_type>
const_range_list::lower_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), _Select1st<const_range>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<multikey_index_type>
const_range_list::upper_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), _Select2nd<const_range>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param k the multikey generator to assign and fill.  
 */
template <size_t D>
void
const_range_list::make_multikey_generator(
		multikey_generator<D, pint_value_type>& k) const {
	typedef multikey_generator<D, pint_value_type>	arg_type;
	INVARIANT(size() <= D);  // else error on user!
	typename arg_type::base_type::iterator li = k.lower_corner.begin();
	typename arg_type::base_type::iterator ui = k.upper_corner.begin();
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

// explicit instantiation thereof
// I know, this could be a pain to maintain, 
// this is only temporary, bear with me.
#define	INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(D)	\
template void							\
const_range_list::make_multikey_generator(			\
	multikey_generator<D, pint_value_type>& ) const;

// INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(0)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(1)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(2)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(3)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(4)

#undef	INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the sizes of the ranges spanned.  
	Return type should be pint_const_collection::array_type::key_type
 */
multikey_generic<size_t>
const_range_list::resolve_sizes(void) const {
	typedef	multikey_generic<size_t>	return_type;
	return_type ret(size());
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		ret[j] = i->size();
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_range_list::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, CONST_RANGE_LIST_TYPE_KEY);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty constructor / allocator for the first pass of 
	deserialization reconstruction.  
 */
persistent*
const_range_list::construct_empty(const int i) {
	return new const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_range_list::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const const_range& cr(*i);
		write_value(f, cr.first);
		write_value(f, cr.second);
	}
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_range_list::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		const_range cr;
		read_value(f, cr.first);
		read_value(f, cr.second);
		push_back(cr);
	}
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class dynamic_range_list method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(dynamic_range_list, 
	DYNAMIC_RANGE_LIST_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_range_list::dynamic_range_list() : range_expr_list(), list_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_range_list::~dynamic_range_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_range_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_range_list::dump(ostream& o) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		(*i)->dump(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_range_list::size(void) const {
	return list_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_range_list::is_static_constant(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		const count_ptr<const pint_range> pr(*i);
		NEVER_NULL(pr);
		if (!pr->is_static_constant())
			return false;
		// else continue checking
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overlap is indefinite with dynamic ranges, conservatively.  
 */
const_range_list
dynamic_range_list::static_overlap(const range_expr_list& r) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param r reference to return value of const_range_list, 
		where final range bounds are saved.  
	\return true if success, false if fail.  
 */
bool
dynamic_range_list::resolve_ranges(const_range_list& r) const {
	// resolve ranges each step, each dynamic_range
	r.clear();
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const_range c;
		const count_ptr<const pint_range> ip(*i);
		if (ip->resolve_range(c)) {
			r.push_back(c);
		} else {
			return false;
		}
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
dynamic_range_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DYNAMIC_RANGE_LIST_TYPE_KEY)) {
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const pint_range> ip(*i);
		ip->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty constructor / allocator for the first pass of 
	deserialization reconstruction.  
 */
persistent*
dynamic_range_list::construct_empty(const int i) {
	return new dynamic_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_range_list::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const pint_range> ip(*i);
		m.write_pointer(f, ip);
	}
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_range_list::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<pint_range> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			ip->load_object(m);
#endif
		push_back(ip);
	}
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}


//=============================================================================
// class index_list method definitions

#if 0
index_list::index_list() : object(), persistent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
index_list::~index_list() { }
#endif

//=============================================================================
// class const_index_list method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(const_index_list, 
	CONST_INDEX_LIST_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list::const_index_list() : index_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Special constructor: copies index list from first argument, 
	and the second argument is a pair of lists that explicitly
	fills in the remaining implicit indices, as resolved from
	multikey_map::is_compact_slice.
	\param l the base index list, may be under-specified.
	\param f the fill lists of explicit indices (upper and lower), 
		or empty if the set's subslice was not densely populated.
 */
const_index_list::const_index_list(const const_index_list& l, 
		const pair<list<pint_value_type>, list<pint_value_type> >& f) :
		index_list(), parent_type(l) {
	if (f.first.empty()) {
		INVARIANT(f.second.empty());
		clear();
	} else {
		INVARIANT(!f.second.empty());
		const size_t f_size = f.first.size();
		const size_t s_size = f.second.size();
		INVARIANT(f_size == s_size);
		const size_t skip = size();
		INVARIANT(skip <= f_size);
		size_t i = 0;
		const_iterator this_iter = begin();
		list<pint_value_type>::const_iterator f_iter = f.first.begin();
		list<pint_value_type>::const_iterator s_iter = f.second.begin();
		for ( ; i<skip; i++, this_iter++, f_iter++, s_iter++) {
			// sanity check against arguments
			NEVER_NULL(*this_iter);
			INVARIANT((*this_iter)->lower_bound() == *f_iter);
			INVARIANT((*this_iter)->upper_bound() == *s_iter);
		}
		for ( ; i<f_size; i++, f_iter++, s_iter++) {
			INVARIANT(*f_iter <= *s_iter);
			push_back(count_ptr<const_range>(
				new const_range(*f_iter, *s_iter)));
		}
		INVARIANT(size() == f_size);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list::~const_index_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_index_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_index_list::dump(ostream& o) const {
	return o << hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
const_index_list::hash_string(void) const {
	string ret;
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const bool b = (i->is_a<const pint_expr>());
		if (b) ret += '[';
		ret += (*i)->hash_string();
		if (b) ret += ']';
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
const_index_list::size(void) const {
	return parent_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A dimension is collapsed if the index is a single integer, 
	and otherwise not collapsed if it's a range.  
	A non-collapsed dimension of size 1 is indicated
	as a range [i..i].  
	\return the number of dimensions are collapsed.  
 */
size_t
const_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		if (i->is_a<const pint_const>())
			ret++;
		else INVARIANT(i->is_a<const const_range>());
			// sanity check
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return list of ranges without the collapsed dimensions.  
		No error is possible, since indices are resolved as constants.  

	Useful for finding actual dimensions and sizes of the reference.  
 */
const_range_list
const_index_list::collapsed_dimension_ranges(void) const {
	const_range_list ret;
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const const_range>
			cr(i->is_a<const const_range>());
		if (cr)
			ret.push_back(*cr);	// will copy
		else INVARIANT(i->is_a<const pint_const>());
		// continue
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper to list parent's push_back that checks that
	expression is a 0-dimensional pint_inst reference.  
 */
void
const_index_list::push_back(const count_ptr<const_index>& i) {
	// check dimensionality
	NEVER_NULL(i);
	INVARIANT(i->dimensions() == 0);
	parent_type::push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::may_be_initialized(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::must_be_initialized(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::is_static_constant(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::is_loop_independent(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::is_unconditional(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
const_index_list::resolve_index_list(void) const {
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Each index should be a scalar integer.  
	Error otherwise.
	\param k placeholder for returning allocated and assigned key.  
	\return true if resolved successfully.
 */
bool
const_index_list::resolve_multikey(
		excl_ptr<multikey_index_type>& k) const {
	k = excl_ptr<multikey_index_type>(
		multikey_index_type::make_multikey(size()));
	NEVER_NULL(k);
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j=0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<const const_index> ip(*i);
		const count_ptr<const pint_const> pc(ip.is_a<pint_const>());
		if (pc)
			(*k)[j] = pc->static_constant_int();
		else 	return false;
	}
	return true;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<multikey_index_type>
const_index_list::lower_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			dereference<count_ptr, const const_index>()
		)
	);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<multikey_index_type>
const_index_list::upper_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			dereference<count_ptr, const const_index>()
		)
	);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given two resolved lists of constant indices, determine
	whether they are dimensionally equal.  
	Bear in mind that collapsed dimensions are to be ignored. 
 */
bool
const_index_list::equal_dimensions(const const_index_list& l) const {
	// compare_if defined in "util/conditional.h"
	return compare_if(begin(), end(), l.begin(), l.end(), 
		mem_fun_ref(&count_ptr<const_index>::is_a<const const_range>), 
		mem_fun_ref(&count_ptr<const_index>::is_a<const const_range>), 
		binary_compose(
			mem_fun_ref(&const_index::range_size_equivalent), 
			dereference<count_ptr, const const_index>(), 
			dereference<count_ptr, const const_index>()
		)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
const_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CONST_INDEX_LIST_TYPE_KEY)) {
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const const_index> ip(*i);
		ip->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty constructor / allocator for the first pass of 
	deserialization reconstruction.  
 */
persistent*
const_index_list::construct_empty(const int i) {
	return new const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_index_list::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const const_index> ip(*i);
		m.write_pointer(f, ip);
	}
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_index_list::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<const_index> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			ip->load_object(m);
#endif
		push_back(ip);
	}
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class dynamic_index_list method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(dynamic_index_list, 
	DYNAMIC_INDEX_LIST_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_index_list::dynamic_index_list() : index_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_index_list::~dynamic_index_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_index_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_index_list::dump(ostream& o) const {
	return o << hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
dynamic_index_list::hash_string(void) const {
	string ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		const bool b = (i->is_a<const pint_expr>());
		if (b) ret += '[';
		ret += (*i)->hash_string();
		if (b) ret += ']';
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
dynamic_index_list::push_back(const count_ptr<index_expr>& i) {
	NEVER_NULL(i);
	if (i->dimensions() != 0) {
		cerr << "i->dimensions = " << i->dimensions() << endl;
		INVARIANT(i->dimensions() == 0);
	}
	parent_type::push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_index_list::size(void) const {
	return parent_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Counts which dimensions are collapsed.  
	See description in const_index_list::dimensions_collapsed().  
 */
size_t
dynamic_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		if (i->is_a<const pint_expr>())
			ret++;
		else INVARIANT(i->is_a<const range_expr>());
			// sanity check
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_index_list::may_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		if (!(*i)->may_be_initialized())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_index_list::must_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		if (!(*i)->must_be_initialized())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_index_list::is_static_constant(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		if (!(*i)->is_static_constant())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_index_list::is_loop_independent(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		if (!(*i)->is_loop_independent())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_index_list::is_unconditional(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		if (!(*i)->is_unconditional())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now const_index_list constains count_ptr<const_index>, 
	not count_const_ptr, so we have to make deep copies, 
	until it is changed?
	\return resolve list of constant indices if every term can be resolved, 
		otherwise, returns an empty list.  
 */
const_index_list
dynamic_index_list::resolve_index_list(void) const {
	const_index_list ret;
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<index_expr> ind(*i);
		const count_ptr<const_index> c_ind(ind.is_a<const_index>());
		if (c_ind) {
			// direct reference copy
			ret.push_back(c_ind);
		} else {
			const count_ptr<const_index>
				r_ind(ind->resolve_index());
			if (r_ind) {
				ret.push_back(r_ind);
			} else {
				// failed to resolve as constant!
				return const_index_list();
			}
		}
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
bool
dynamic_index_list::resolve_multikey(
		excl_ptr<multikey_index_type>& k) const {
	k = excl_ptr<multikey_index_type>(
		multikey_index_type::make_multikey(size()));
	NEVER_NULL(k);
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<const index_expr> ip(*i);
		const count_ptr<const pint_expr> pi(ip.is_a<pint_expr>());
		// assert(pi); ?
		if (pi) {
			if (!pi->resolve_value((*k)[j])) {
				cerr << "Unable to resolve pint_expr at "
					"index position " << j << endl;
				return false;
			}
		} else {
			cerr << "Index at position " << j << 
				"is not a pint_expr!" << endl;
			return false;
		}
	}
	// nothing went wrong
	return true;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
dynamic_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DYNAMIC_INDEX_LIST_TYPE_KEY)) {
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const index_expr> ip(*i);
		ip->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty constructor / allocator for the first pass of 
	deserialization reconstruction.  
 */
persistent*
dynamic_index_list::construct_empty(const int i) {
	return new dynamic_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_index_list::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const index_expr> ip(*i);
		m.write_pointer(f, ip);
	}
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_index_list::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<index_expr> ip;
		m.read_pointer(f, ip);
		if (ip)
			ip->load_object(m);
		// need to load to know dimensions
		push_back(ip);
	}
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
}	// end namepace entity
}	// end namepace ART

#endif	// __ART_OBJECT_EXPR_CC__

