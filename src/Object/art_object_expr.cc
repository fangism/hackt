/**
	\file "Object/art_object_expr.cc"
	Class method definitions for semantic expression.  
 	$Id: art_object_expr.cc,v 1.47.4.4 2005/06/06 21:18:44 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_EXPR_CC__
#define	__OBJECT_ART_OBJECT_EXPR_CC__

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

#include <exception>
#include <algorithm>

// consider: (for reducing expression storage overhead)
// #define NO_OBJECT_SANITY	1
// this will override the definition in "art_object_base.h"

#include "Object/art_object_index.h"
#include "Object/art_object_expr.h"	// includes "art_object_expr_const.h"
// #include "Object/art_object_expr_param_ref.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_classification_details.h"
#include "Object/art_object_value_collection.h"
#include "Object/art_object_const_collection.tcc"
#include "Object/art_object_value_reference.tcc"
#include "Object/art_object_assign.h"
#include "Object/art_object_connect.h"	// for ~aliases_connection_base
#include "Object/art_object_type_hash.h"

#if 0
#include "util/multikey.h"		// extern template instantiations
#include "util/packed_array.h"		// extern template instantiations
#else
// experimental: suppressing automatic instantiation of template code
#include "Object/art_object_extern_templates.h"
#endif

#include "util/what.tcc"
#include "util/STL/list.tcc"
#include "util/qmap.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/sstream.h"		// for ostringstring, used by dump
#include "util/discrete_interval_set.tcc"
#include "util/compose.h"
#include "util/conditional.h"		// for compare_if
#include "util/ptrs_functional.h"
#include "util/dereference.h"

// these conditional definitions must appear after inclusion of "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif


//=============================================================================
namespace util {
// specializations of the class what, needed to override the default
// mangled names returned by type_info::name().

// These specializations need not be externally visible, unless
// the class template "what" is used directly.  
// Most of the time, the "what" member function interface is used.  

SPECIALIZE_UTIL_WHAT(ART::entity::pint_const_collection,
		"pint-const-collection")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_const_collection,
		"pbool-const-collection")
SPECIALIZE_UTIL_WHAT(ART::entity::const_param_expr_list,
		"const-param-expr-list")
SPECIALIZE_UTIL_WHAT(ART::entity::dynamic_param_expr_list,
		"param-expr-list")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_meta_instance_reference,
		"pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_meta_instance_reference,
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
SPECIALIZE_UTIL_WHAT(ART::entity::dynamic_meta_range_list, 
		"dynamic_meta_range_list")
SPECIALIZE_UTIL_WHAT(ART::entity::const_index_list, 
		"const-index-list")
SPECIALIZE_UTIL_WHAT(ART::entity::dynamic_meta_index_list, 
		"dynamic-index-list")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::const_param_expr_list,
		CONST_PARAM_EXPR_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::dynamic_param_expr_list,
		DYNAMIC_PARAM_EXPR_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_meta_instance_reference, 
		SIMPLE_PBOOL_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_meta_instance_reference, 
		SIMPLE_PINT_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_const, CONST_PINT_TYPE_KEY, 0)

// pint_const_collection requires special treatment:
// it has no empty constructor and requires an int argument
// this example shows how we can register various bound constructor
// functors with the persistent_object_manager type registry.  
using ART::entity::const_collection;
// macros defined in "art_object_const_collection.tcc"
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	ART::entity::pint_tag, CONST_PINT_COLLECTION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(
	ART::entity::pbool_tag, CONST_PBOOL_COLLECTION_TYPE_KEY)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_const, CONST_PBOOL_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_unary_expr, PINT_UNARY_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_unary_expr, PBOOL_UNARY_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::arith_expr, ARITH_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::relational_expr, RELATIONAL_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::logical_expr, LOGICAL_EXPR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_range, DYNAMIC_RANGE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::const_range, CONST_RANGE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::const_range_list, CONST_RANGE_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::dynamic_meta_range_list, DYNAMIC_RANGE_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::const_index_list, CONST_INDEX_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::dynamic_meta_index_list, DYNAMIC_INDEX_LIST_TYPE_KEY, 0)

namespace memory {
	// pool-allocator managed types that are safe to destroy lazily
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::pbool_const)
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::pint_const)
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::const_range)
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
//=============================================================================
// #include "util/using_ostream.h"
using namespace util::memory;
USING_UTIL_COMPOSE
USING_UTIL_OPERATIONS
using util::discrete_interval_set;
using std::_Select1st;
using std::_Select2nd;
using std::mem_fun_ref;
using util::dereference;
using std::ostringstream;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
USING_STACKTRACE
using util::persistent_traits;

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
// the robust list_vector_pool requires this.  
#endif

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
param_expr::param_expr() : persistent() { }

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

pbool_expr::~pbool_expr() {
	STACKTRACE_DTOR("~pbool_expr()");
}

bool
pbool_expr::may_be_equivalent_generic(const param_expr& p) const {
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

pint_expr::~pint_expr() {
	STACKTRACE_DTOR("~pint_expr()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::may_be_equivalent_generic(const param_expr& p) const {
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
// class param_expr_list method definitions

#if 0
param_expr_list::param_expr_list() : persistent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expr_list::~param_expr_list() { }
#endif

//=============================================================================
// class const_param_expr_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_param_expr_list::const_param_expr_list() :
		param_expr_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_param_expr_list::~const_param_expr_list() {
	STACKTRACE_DTOR("~const_param_expr_list()");
}

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
		if (!ip->may_be_equivalent_generic(*jp))
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
		if (!ip->may_be_equivalent_generic(*jp))
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
		if (!ip->must_be_equivalent_generic(*jp))
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
		if (!ip->must_be_equivalent_generic(*jp))
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
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
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
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_param_expr_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const const_param> ip(*i);
		m.write_pointer(f, ip);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_param_expr_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	STACKTRACE_PERSISTENT("const_param_expr_list::load_object()");
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<const_param> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			m.load_object_once(ip);
#endif
		push_back(ip);
	}
}

//=============================================================================
// class dynamic_param_expr_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_param_expr_list::dynamic_param_expr_list() :
		param_expr_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_param_expr_list::~dynamic_param_expr_list() {
	STACKTRACE_DTOR("~dynamic_param_expr_list()");
#if 0
	cerr << "list contains " << size() << " pointers." << endl;
	dump(cerr) << endl;
	cerr << "reference counts in list:" << endl;
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		cerr << "\t" << i->refs() << " @ " << 
			((*i) ? &**i : NULL) << endl;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_param_expr_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_param_expr_list::dump(ostream& o) const {
	if (empty()) return o;
	// else at least 1 item in list
	const_iterator i = begin();
	const const_iterator e = end();
	if (*i)	(*i)->dump(o);
	else	o << "(null)";
	for (i++; i!=e; i++) {
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
		if (!ip->may_be_equivalent_generic(*jp))
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
		if (!ip->may_be_equivalent_generic(*jp))
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
		if (!ip->must_be_equivalent_generic(*jp))
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
		if (!ip->must_be_equivalent_generic(*jp))
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
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
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
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_param_expr_list::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const param_expr> ip(*i);
		m.write_pointer(f, ip);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_param_expr_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	STACKTRACE_PERSISTENT("dyn_param_expr_list::load_object()");
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<param_expr> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			m.load_object_once(ip);
#endif
		push_back(ip);
	}
}

//=============================================================================
// class meta_index_expr method definitions

#if 0
meta_index_expr::meta_index_expr() : persistent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_index_expr::~meta_index_expr() { }
#endif

//-----------------------------------------------------------------------------
// class const_index method definitions

#if 0
const_index::const_index() : meta_index_expr() { }

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
good_bool
pint_const::resolve_values_into_flat_list(list<value_type>& l) const {
	l.push_back(val);
	return good_bool(true);
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
good_bool
pbool_const::resolve_values_into_flat_list(list<value_type>& l) const {
	l.push_back(val);
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
// class pint_unary_expr method definitions

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
	\return false if there is error in resolving.
 */
good_bool
pint_unary_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type i = 0;
	const good_bool ret(resolve_value(i));
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
			new pint_const(- pc->static_constant_value()));
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
good_bool
pbool_unary_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type b;
	const good_bool ret(resolve_value(b));
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
			new pbool_const(!pc->static_constant_value()));
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
// class arith_expr method definitions

// static member initializations (order matters!)

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
ostream&
arith_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
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
arith_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	// Oooooh, virtual operator dispatch!
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
arith_expr::must_be_equivalent(const pint_expr& p) const {
	const arith_expr* const ae = IS_A(const arith_expr*, &p);
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
arith_expr::resolve_value(value_type& i) const {
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
	\return false if there is error in resolving.
 */
good_bool
arith_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type i = 0;
	const good_bool ret(resolve_value(i));
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
	\return true if resolved.
 */
good_bool
arith_expr::unroll_resolve_value(const unroll_context& c, value_type& i) const {
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
			(*op)(lpc->static_constant_value(), 
				rpc->static_constant_value())));
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
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
arith_expr::write_object(const persistent_object_manager& m, ostream& f) const {
	write_value(f, reverse_op_map[op]);	// writes a character
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
arith_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	char o;
	read_value(f, o);
	op = op_map[o];
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class relational_expr method definitions

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
relational_expr::relational_expr(const count_ptr<const pint_expr>& l,
		const op_type* o, const count_ptr<const pint_expr>& r) :
		lx(l), rx(r), op(o) {
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
relational_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
relational_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
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
relational_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
relational_expr::must_be_equivalent(const pbool_expr& b) const {
	const relational_expr* const re = IS_A(const relational_expr*, &b);
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
relational_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: switch on relational expression operator.  
 */
good_bool
relational_expr::resolve_value(value_type& i) const {
	arg_type li, ri;
	const good_bool l_ret(lx->resolve_value(li));
	const good_bool r_ret(rx->resolve_value(ri));
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
good_bool
relational_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	value_type b;
	const good_bool ret(resolve_value(b));
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
			(*op)(lpc->static_constant_value(), 
				rpc->static_constant_value())));
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
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
relational_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
relational_expr::load_object(const persistent_object_manager& m, istream& f) {
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
// class logical_expr method definitions

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
logical_expr::logical_expr(const count_ptr<const pbool_expr>& l,
		const op_type* o, const count_ptr<const pbool_expr>& r) :
		lx(l), rx(r), op(o) {
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
logical_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
logical_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
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
logical_expr::static_constant_value(void) const {
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	return (*op)(a,b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
logical_expr::must_be_equivalent(const pbool_expr& b) const {
	const logical_expr* const re = IS_A(const logical_expr*, &b);
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
logical_expr::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: switch on logical expression operator.  
 */
good_bool
logical_expr::resolve_value(value_type& i) const {
	arg_type lb, rb;
	const good_bool l_ret(lx->resolve_value(lb));
	const good_bool r_ret(rx->resolve_value(rb));
	i = (*op)(lb, rb);
	return l_ret && r_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
logical_expr::resolve_values_into_flat_list(list<value_type>& l) const {
	arg_type b;
	const good_bool ret(resolve_value(b));
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
			(*op)(lpc->static_constant_value(), 
				rpc->static_constant_value())));
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
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
logical_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
logical_expr::load_object(const persistent_object_manager& m, istream& f) {
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
// class pint_range method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pint_range::pint_range() :
		meta_range_expr(), lower(NULL), upper(NULL) {
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
		meta_range_expr(),
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
		meta_range_expr(), lower(l), upper(u) {
	NEVER_NULL(lower);
	NEVER_NULL(upper);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/// default copy-constructor
pint_range::pint_range(const pint_range& pr) :
		persistent(), meta_index_expr(), meta_range_expr(), 
		// need explicit virtual base constructors?
		lower(pr.lower), upper(pr.upper) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_range)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
pint_range::dump_brief(ostream& o) const {
	return dump(o);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_range::dump(ostream& o) const {
	return upper->dump_brief(
		lower->dump_brief(o << '[') << "..") << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Range is sane if lower <= upper.
	If expressions are not constant, then conservatively return true.  
 */
bool
pint_range::is_sane(void) const {
	if (is_static_constant()) {
		return lower->static_constant_value() <=
			upper->static_constant_value();
	}
	else return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_range::is_static_constant(void) const {
	return lower->is_static_constant() && upper->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre both upper and lower MUST be is_static_constant().
 */
const_range
pint_range::static_constant_range(void) const {
	return const_range(lower->static_constant_value(), 
		upper->static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if successfully resolved at unroll-time.
 */
good_bool
pint_range::unroll_resolve_range(const unroll_context& c, 
		const_range& r) const {
	if (!lower->unroll_resolve_value(c, r.first).good)
		return good_bool(false);
	if (!upper->unroll_resolve_value(c, r.second).good)
		return good_bool(false);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pint_range::resolve_range(const_range& r) const {
	if (!lower->resolve_value(r.first).good)
		return good_bool(false);
	if (!upper->resolve_value(r.second).good)
		return good_bool(false);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_range::must_be_formal_size_equivalent(const meta_range_expr& re) const {
	const const_range* const cr = IS_A(const const_range*, &re);
	if (cr) {
		if (!is_static_constant())
			return false;
		else return lower->static_constant_value() == cr->first &&
			upper->static_constant_value() == cr->second;
	} else  {
		const pint_range* const pr = IS_A(const pint_range*, &re);
		INVARIANT(pr);
		return lower->must_be_equivalent(*pr->lower) &&
			upper->must_be_equivalent(*pr->upper);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_range::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lower->collect_transient_info(m);
	upper->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_range::write_object(const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, lower);
	m.write_pointer(f, upper);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_range::load_object(const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, lower);
	m.read_pointer(f, upper);
}

//=============================================================================
// class const_range method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(const_range, 64)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Default empty constructor. 
	Makes an invalid range.  
 */
const_range::const_range() :
		meta_range_expr(), const_index(), parent_type(0,-1) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/** Protected internal constructor. */
const_range::const_range(const interval_type& i) :
		meta_range_expr(), const_index(), 
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
		meta_range_expr(), const_index(), 
		parent_type(0, n-1) {
	INVARIANT(upper() >= lower());		// else what!?!?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit constructor of a dense range from 0 to N-1.  
	\param n must be > 0, else assertion will fail.
 */
const_range::const_range(const pint_const& n) :
		meta_range_expr(), const_index(), 
		parent_type(0, n.static_constant_value() -1) {
	INVARIANT(upper() >= lower());		// else what!?!?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should both l and u be non-negative too?
	\param l is lower bound, inclusive.  
	\param u is upper bound, inclusive, and must be >= l.  
 */
const_range::const_range(const pint_value_type l, const pint_value_type u) :
		meta_range_expr(), const_index(), 
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
		meta_range_expr(), const_index(), 
		parent_type(l, u) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/** standard copy constructor */
const_range::const_range(const const_range& r) :
		persistent(), 
		meta_index_expr(),
		meta_range_expr(), 
		const_index(), 
		parent_type(r) {
	// assert check range?
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range::const_range(const parent_type& r) :
		persistent(), 
		meta_index_expr(),
		meta_range_expr(), 
		const_index(), 
		parent_type(r) {
	// assert check range?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_range)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
const_range::dump_brief(ostream& o) const {
	return dump(o);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_range::dump(ostream& o) const {
	if (empty())
		return o << "[]";
	else
		return o << "[" << lower() << ".." << upper() << "]";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns whether or not two intervals overlap.  
	\return r other range to compare.  
	\return overlap range.
 */
const_range
const_range::static_overlap(const const_range& r) const {
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
bool
const_range::must_be_formal_size_equivalent(const meta_range_expr& re) const {
	const const_range* const cr = IS_A(const const_range*, &re);
	if (cr) {
		return (*this) == *cr;
	} else {
		const pint_range* const pr = IS_A(const pint_range*, &re);
		INVARIANT(pr);
		return pr->must_be_formal_size_equivalent(*this);
	}
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
good_bool
const_range::unroll_resolve_range(const unroll_context&, const_range& r) const {
	r = *this;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
const_range::resolve_range(const_range& r) const {
	r = *this;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return deep copy of this constant range, always succeeds.  
 */
count_ptr<const_index>
const_range::unroll_resolve_index(const unroll_context& c) const {
	return count_ptr<const_index>(new const_range(*this));
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
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_range::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, first);
	write_value(f, second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_range::load_object(const persistent_object_manager& m, istream& f) {
	read_value(f, first);
	read_value(f, second);
}

//=============================================================================
// class meta_range_expr method definitions

#if 0
meta_range_expr::meta_range_expr() : meta_index_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_range_expr::~meta_range_expr() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a resolve constant range, or NULL if resolution fails.  
 */
count_ptr<const_index>
meta_range_expr::unroll_resolve_index(const unroll_context& c) const {
	typedef	count_ptr<const_index>	return_type;
	const_range tmp;
	return (unroll_resolve_range(c, tmp).good) ?
		return_type(new const_range(tmp)) :
		return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a resolve constant range, or NULL if resolution fails.  
 */
count_ptr<const_index>
meta_range_expr::resolve_index(void) const {
	typedef	count_ptr<const_index>	return_type;
	const_range tmp;
	return (resolve_range(tmp).good) ?
		return_type(new const_range(tmp)) :
		return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
meta_range_expr::must_be_equivalent_index(const meta_index_expr& i) const {
	const meta_range_expr* const r = IS_A(const meta_range_expr*, &i);
	if (r) {
		return must_be_formal_size_equivalent(*r);
	} else {
		// conservatively
		return false;
	}
}

//=============================================================================
// class meta_range_list method definitions

#if 0
meta_range_list::meta_range_list() : persistent() {
}
#endif

//=============================================================================
// class const_range_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list::const_range_list() : meta_range_list(), list_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implicit conversion.  
 */
const_range_list::const_range_list(const list_type& l) :
		meta_range_list(), list_type(l) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit conversion from an index list to a range list.  
	Only available for constants, of course.
	Converts x[n] to x[n..n].
	Useful for static range-checking of indices.  
 */
const_range_list::const_range_list(const const_index_list& i) :
		meta_range_list(), list_type() {
	const_index_list::const_iterator j = i.begin();
	for ( ; j!=i.end(); j++) {
		const count_ptr<const_index> k(*j);
		NEVER_NULL(k);
		const count_ptr<pint_const> p(k.is_a<pint_const>());
		const count_ptr<const_range> r(k.is_a<const_range>());
		if (p) {
			const int min_max = p->static_constant_value();
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
const_range_list::static_overlap(const meta_range_list& r) const {
#if 0
	// DEBUG
	cerr << "In const_range_list::static_overlap with this = "
		<< this << endl;
	r.dump(cerr << "const meta_range_list& r = ") << endl;
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
			INVARIANT(i->first == pi->static_constant_value());
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
const_range_list::must_be_formal_size_equivalent(
		const meta_range_list& rl) const {
	const const_range_list* const crl = IS_A(const const_range_list*, &rl);
	if (crl) {
		return is_size_equivalent(*crl);
	} else {
		const dynamic_meta_range_list* const
			drl = IS_A(const dynamic_meta_range_list*, &rl);
		INVARIANT(drl);
		return drl->must_be_formal_size_equivalent(*this);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
const_range_list::resolve_ranges(const_range_list& r) const {
	r = *this;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
const_range_list::unroll_resolve(const_range_list& r, 
		const unroll_context& c) const {
	r = *this;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#else
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_range_list::lower_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), _Select1st<const_range>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_range_list::upper_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), _Select2nd<const_range>());
	return ret;
}
#endif

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
multikey_index_type
const_range_list::resolve_sizes(void) const {
	multikey_index_type ret(size());
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
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_range_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const const_range& cr(*i);
		write_value(f, cr.first);
		write_value(f, cr.second);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_range_list::load_object(const persistent_object_manager& m, istream& f) {
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		const_range cr;
		read_value(f, cr.first);
		read_value(f, cr.second);
		push_back(cr);
	}
}

//=============================================================================
// class dynamic_meta_range_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_range_list::dynamic_meta_range_list() : meta_range_list(), list_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_range_list::~dynamic_meta_range_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_meta_range_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_meta_range_list::dump(ostream& o) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		(*i)->dump(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_meta_range_list::size(void) const {
	return list_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_range_list::is_static_constant(void) const {
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
dynamic_meta_range_list::static_overlap(const meta_range_list& r) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param r reference to return value of const_range_list, 
		where final range bounds are saved.  
	\return true if success, false if fail.  
 */
good_bool
dynamic_meta_range_list::resolve_ranges(const_range_list& r) const {
	// resolve ranges each step, each dynamic_range
	r.clear();
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const_range c;
		const count_ptr<const pint_range> ip(*i);
		if (ip->resolve_range(c).good) {
			r.push_back(c);
		} else {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
dynamic_meta_range_list::unroll_resolve(const_range_list& r, 
		const unroll_context& c) const {
	INVARIANT(r.empty());
	// write as transform?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const_range cr;
		const count_ptr<const pint_range> ip(*i);
		if (ip->unroll_resolve_range(c, cr).good) {
			r.push_back(cr);
		} else {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_range_list::must_be_formal_size_equivalent(
		const meta_range_list& rl) const {
	const const_range_list* const crl = IS_A(const const_range_list*, &rl);
	if (crl) {
		INVARIANT(size() == crl->size());
		const_iterator i = begin();
		const const_iterator e = end();
		const_range_list::const_iterator j = crl->begin();
		// use some std:: algorithm ... later
		for ( ; i!=e; i++, j++) {
			// NEVER_NULL(...)
			if (!(*i)->must_be_formal_size_equivalent(*j))
				return false;
		}
	} else {
		const dynamic_meta_range_list* const
			drl = IS_A(const dynamic_meta_range_list*, &rl);
		INVARIANT(drl);
		INVARIANT(size() == drl->size());
		const_iterator i = begin();
		const const_iterator e = end();
		const_iterator j = drl->begin();
		// use some std:: algorithm ... later
		for ( ; i!=e; i++, j++) {
			// NEVER_NULL(...)
			if (!(*i)->must_be_formal_size_equivalent(**j))
				return false;
		}
	}
	// else no errors found
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
dynamic_meta_range_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
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
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_meta_range_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const pint_range> ip(*i);
		m.write_pointer(f, ip);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_meta_range_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<pint_range> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			m.load_object_once(ip);
#endif
		push_back(ip);
	}
}


//=============================================================================
// class meta_index_list method definitions

#if 0
meta_index_list::meta_index_list() : persistent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_index_list::~meta_index_list() { }
#endif

//=============================================================================
// class const_index_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list::const_index_list() : meta_index_list(), parent_type() { }

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
		meta_index_list(), parent_type(l) {
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
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const pint_expr>
			b(i->is_a<const pint_expr>());
		if (b)
			b->dump_brief(o << '[') << ']';
		else	(*i)->dump(o);
	}
	return o;
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
const_index_list
const_index_list::unroll_resolve(const unroll_context& c) const {
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
			(*k)[j] = pc->static_constant_value();
		else 	return false;
	}
	return true;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
excl_ptr<multikey_index_type>
const_index_list::lower_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			dereference<count_ptr<const const_index> >()
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
			dereference<count_ptr<const const_index> >()
		)
	);
	return ret;
}
#else
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_index_list::lower_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			dereference<count_ptr<const const_index> >()
		)
	);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_index_list::upper_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			dereference<count_ptr<const const_index> >()
		)
	);
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given two resolved lists of constant indices, determine
	whether they are dimensionally equal.  
	Bear in mind that collapsed dimensions are to be ignored
		in the two argument lists. 
 */
bool
const_index_list::equal_dimensions(const const_index_list& l) const {
	// compare_if defined in "util/conditional.h"
	return compare_if(begin(), end(), l.begin(), l.end(), 
		mem_fun_ref(&count_ptr<const_index>::is_a<const const_range>), 
		mem_fun_ref(&count_ptr<const_index>::is_a<const const_range>), 
		binary_compose(
			mem_fun_ref(&const_index::range_size_equivalent), 
			dereference<count_ptr<const const_index> >(), 
			dereference<count_ptr<const const_index> >()
		)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::must_be_equivalent_indices(const meta_index_list& l) const {
	const const_index_list* const cl = IS_A(const const_index_list*, &l);
	if (cl) {
		// here, we DO NOT skip collapsed dimensions for comparison
		return std::equal(begin(), end(), cl->begin(), 
		binary_compose(
			mem_fun_ref(&const_index::must_be_equivalent_index), 
			dereference<count_ptr<const const_index> >(), 
			dereference<count_ptr<const const_index> >()
		)
		);
	} else {
		const dynamic_meta_index_list* const
			dl = IS_A(const dynamic_meta_index_list*, &l);
		return dl->must_be_equivalent_indices(*this);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
const_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
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
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_index_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const const_index> ip(*i);
		m.write_pointer(f, ip);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_index_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<const_index> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			m.load_object_once(ip);
#endif
		push_back(ip);
	}
}

//=============================================================================
// class dynamic_meta_index_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_index_list::dynamic_meta_index_list() :
		meta_index_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_index_list::~dynamic_meta_index_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_meta_index_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_meta_index_list::dump(ostream& o) const {
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const pint_expr>
			b(i->is_a<const pint_expr>());
		if (b)
			b->dump_brief(o << '[') << ']';
		else	(*i)->dump(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
dynamic_meta_index_list::push_back(const count_ptr<meta_index_expr>& i) {
	NEVER_NULL(i);
	if (i->dimensions() != 0) {
		cerr << "i->dimensions = " << i->dimensions() << endl;
		INVARIANT(i->dimensions() == 0);
	}
	parent_type::push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_meta_index_list::size(void) const {
	return parent_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Counts which dimensions are collapsed.  
	See description in const_index_list::dimensions_collapsed().  
 */
size_t
dynamic_meta_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		if (i->is_a<const pint_expr>())
			ret++;
		else INVARIANT(i->is_a<const meta_range_expr>());
			// sanity check
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_index_list::may_be_initialized(void) const {
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
dynamic_meta_index_list::must_be_initialized(void) const {
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
dynamic_meta_index_list::is_static_constant(void) const {
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
dynamic_meta_index_list::is_loop_independent(void) const {
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
dynamic_meta_index_list::is_unconditional(void) const {
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
dynamic_meta_index_list::resolve_index_list(void) const {
	const_index_list ret;
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<meta_index_expr> ind(*i);
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
bool
dynamic_meta_index_list::must_be_equivalent_indices(
		const meta_index_list& l) const {
	const const_index_list* const cl = IS_A(const const_index_list*, &l);
	if (cl) {
		INVARIANT(size() == cl->size());
		// heterogenous comparison between const_index, meta_index_expr
		return std::equal(begin(), end(), cl->begin(), 
		binary_compose(
			mem_fun_ref(&meta_index_expr::must_be_equivalent_index), 
			dereference<count_ptr<const meta_index_expr> >(), 
			dereference<count_ptr<const const_index> >()
		)
		);
	} else {
		const dynamic_meta_index_list* const
			dl = IS_A(const dynamic_meta_index_list*, &l);
		INVARIANT(size() == dl->size());
		return std::equal(begin(), end(), dl->begin(), 
		binary_compose(
			mem_fun_ref(&meta_index_expr::must_be_equivalent_index), 
			dereference<count_ptr<const meta_index_expr> >(), 
			dereference<count_ptr<const meta_index_expr> >()
		)
		);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Similar to resolve_index except that this takes a context 
	argument for unroll-time resolution.  
	\return populated const_index_list if all indices have been 
		successful resolved, else an empty list.
 */
const_index_list
dynamic_meta_index_list::unroll_resolve(const unroll_context& c) const {
	const_index_list ret;
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<meta_index_expr> ind(*i);
		const count_ptr<const_index> c_ind(ind.is_a<const_index>());
		if (c_ind) {
			// direct reference copy
			ret.push_back(c_ind);
		} else {
			const count_ptr<const_index>
				r_ind(ind->unroll_resolve_index(c));
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
dynamic_meta_index_list::resolve_multikey(
		excl_ptr<multikey_index_type>& k) const {
	k = excl_ptr<multikey_index_type>(
		multikey_index_type::make_multikey(size()));
	NEVER_NULL(k);
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<const meta_index_expr> ip(*i);
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
dynamic_meta_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const meta_index_expr> ip(*i);
		ip->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_meta_index_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++) {
		const count_ptr<const meta_index_expr> ip(*i);
		m.write_pointer(f, ip);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_meta_index_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<meta_index_expr> ip;
		m.read_pointer(f, ip);
		if (ip)
			m.load_object_once(ip);
		// need to load to know dimensions
		push_back(ip);
	}
}

//=============================================================================
// explicit template instantiations

template class const_collection<pint_tag>;
template class const_collection<pbool_tag>;

template class simple_meta_value_reference<pint_tag>;
template class simple_meta_value_reference<pbool_tag>;

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

#endif	// __OBJECT_ART_OBJECT_EXPR_CC__

