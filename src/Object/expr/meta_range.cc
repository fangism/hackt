/**
	\file "Object/expr/meta_range.cc"
	Meta range expression class definitions.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: meta_range.cc,v 1.19 2007/07/31 23:23:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_RANGE_CC__
#define	__HAC_OBJECT_EXPR_META_RANGE_CC__

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

#include "Object/expr/pint_range.h"
#include "Object/expr/const_range.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pint_arith_expr.h"
#include "Object/expr/expr_visitor.h"
#include "Object/persistent_type_hash.h"

#include "util/stacktrace.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/discrete_interval_set.tcc"

//=============================================================================
namespace util {
// specializations of the class what, needed to override the default
// mangled names returned by type_info::name().

// These specializations need not be externally visible, unless
// the class template "what" is used directly.  
// Most of the time, the "what" member function interface is used.  

SPECIALIZE_UTIL_WHAT(HAC::entity::pint_range, 
		"pint-range")
SPECIALIZE_UTIL_WHAT(HAC::entity::const_range, 
		"const-range")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_range, DYNAMIC_RANGE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::const_range, CONST_RANGE_TYPE_KEY, 0)

namespace memory {
	// pool-allocator managed types that are safe to destroy lazily
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::const_range)
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
using util::discrete_interval_set;
using util::write_value;
using util::read_value;
using util::persistent_traits;
using util::persistent_object_manager;
using std::istream;
#include "util/using_ostream.h"

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
// class meta_index_expr method definitions

count_ptr<const nonmeta_index_expr_base>
meta_index_expr::unroll_resolve_copy(const unroll_context& c,
		const count_ptr<const nonmeta_index_expr_base>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const meta_index_expr>());
}

//=============================================================================
// class meta_range_expr method definitions

/**
	Converts a single index (implicit range) to an explicit range.
	If argument is already an explicit range, then return it.  
 */
count_ptr<const meta_range_expr>
meta_range_expr::make_explicit_range(const count_ptr<const parent_type>& i) {
	NEVER_NULL(i);
{
	const count_ptr<const this_type> ret(i.is_a<const this_type>());
	if (ret) return ret;
}
{
	const count_ptr<const pint_expr> ret(i.is_a<const pint_expr>());
	NEVER_NULL(ret);
	if (ret->is_static_constant()) {
		return count_ptr<const this_type>(
			new const_range(ret->static_constant_value()));
	} else {
		return count_ptr<const this_type>(new pint_range(ret));
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_range>
meta_range_expr::make_explicit_pint_range(
		const count_ptr<const parent_type>& i) {
	NEVER_NULL(i);
{
	const count_ptr<const pint_range> ret(i.is_a<const pint_range>());
	if (ret) return ret;
}
	const count_ptr<const pint_expr> ret(i.is_a<const pint_expr>());
	NEVER_NULL(ret);
	return count_ptr<const pint_range>(new pint_range(ret));
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
		upper(new pint_arith_expr(n, '-', 
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
pint_range::dump(ostream& o, const expr_dump_context& c) const {
	// consider copy-modifying the expr_dump_context to be `brief'
	return upper->dump(
		lower->dump(o << '[', c) << "..", c) << ']';
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
bool
pint_range::is_relaxed_formal_dependent(void) const {
	return lower->is_relaxed_formal_dependent() ||
		upper->is_relaxed_formal_dependent();
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
void
pint_range::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
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
count_ptr<const const_index>
pint_range::unroll_resolve_copy(const unroll_context& c,
		const count_ptr<const meta_index_expr>& p) const {
	INVARIANT(p == this);
	const count_ptr<const_range> ret(new const_range);
	if (unroll_resolve_range(c, *ret).good) {
		return ret;
	} else {
		return count_ptr<const const_index>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return NULL because nonmeta ranges don't exist yet. 
 */
count_ptr<const pint_const>
pint_range::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const nonmeta_index_expr_base>& p) const {
	return count_ptr<const pint_const>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const meta_index_expr>
pint_range::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const meta_index_expr>& i) const {
	typedef	count_ptr<const meta_index_expr>	return_type;
	NEVER_NULL(i);
	INVARIANT(i == this);
	const count_ptr<const pint_expr>
		lb(lower->substitute_default_positional_parameters(f, e, lower)),
		ub(upper->substitute_default_positional_parameters(f, e, upper));
	if (lb && ub) {
		if (lb == lower && ub == upper) {
			// return copy of self, unsubstituted
			return i;
		} else {
			// something was different, return new range
			return return_type(new this_type(lb, ub));
		}
	} else {
		return return_type(NULL);
	}
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
#if !ALLOW_NEGATIVE_RANGES
	INVARIANT(!empty());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit constructor of a dense range from 0 to N-1.  
	\param n must be > 0, else assertion will fail.
 */
const_range::const_range(const pint_const& n) :
		meta_range_expr(), const_index(), 
		parent_type(0, n.static_constant_value() -1) {
#if !ALLOW_NEGATIVE_RANGES
	INVARIANT(!empty());
#endif
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
#if !ALLOW_NEGATIVE_RANGES
	INVARIANT(!empty());
#endif
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
const_range::dump(ostream& o, const expr_dump_context&) const {
#if !ALLOW_NEGATIVE_RANGES
	if (empty())
		return o << "[]";
	else
#endif
		return dump_force(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps the range, whether or not it is valid.  
 */
ostream&
const_range::dump_force(ostream& o) const {
	return o << "[" << first << ".." << second << "]";
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
void
const_range::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
const_range::unroll_resolve_range(const unroll_context&, const_range& r) const {
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
count_ptr<const const_index>
const_range::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const meta_index_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const const_range>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return NULL because nonmeta ranges don't exist yet. 
 */
count_ptr<const pint_const>
const_range::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const nonmeta_index_expr_base>& p) const {
	return count_ptr<const pint_const>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const meta_index_expr>
const_range::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const meta_index_expr>& i) const {
	NEVER_NULL(i);
	INVARIANT(i == this);
	return i;
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
/**
	Prints error diagnostic for a bad range.
 */
ostream&
const_range::diagnose_bad_range(ostream& o, const const_range& r) {
	return o << "bad range: " << r.first << ".." << r.second;
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

#endif	// __HAC_OBJECT_EXPR_META_RANGE_CC__

