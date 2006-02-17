/**
	\file "Object/meta_param_expr_list.cc"
	Definitions for meta parameter expression lists.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: meta_param_expr_list.cc,v 1.12.4.1 2006/02/17 05:07:32 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_PARAM_EXPR_LIST_CC__
#define	__HAC_OBJECT_EXPR_META_PARAM_EXPR_LIST_CC__

// flags for controlling conditional compilation, mostly for debugging
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/param_expr.h"
#include "Object/expr/const_param.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/inst/param_value_collection.h"
#include "Object/common/dump_flags.h"
#include "Object/persistent_type_hash.h"
#include "common/ICE.h"

#include "util/reserve.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace util {
// specializations of the class what, needed to override the default
// mangled names returned by type_info::name().

// These specializations need not be externally visible, unless
// the class template "what" is used directly.  
// Most of the time, the "what" member function interface is used.  

SPECIALIZE_UTIL_WHAT(HAC::entity::const_param_expr_list,
		"const-param-expr-list")
SPECIALIZE_UTIL_WHAT(HAC::entity::dynamic_param_expr_list,
		"param-expr-list")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::const_param_expr_list,
		CONST_PARAM_EXPR_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::dynamic_param_expr_list,
		DYNAMIC_PARAM_EXPR_LIST_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::persistent_traits;
using util::write_value;
using util::read_value;

//=============================================================================
// class param_expr_list method definitions

//=============================================================================
// class const_param_expr_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_param_expr_list::const_param_expr_list() :
		param_expr_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_param_expr_list::const_param_expr_list(
		const parent_type::value_type& p) :
		param_expr_list(), parent_type() {
	push_back(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_param_expr_list::~const_param_expr_list() {
	STACKTRACE_DTOR("~const_param_expr_list()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_param_expr_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_param_expr_list::dump(ostream& o, const expr_dump_context& c) const {
	if (empty())	return o;
	else		return dump_range(o, c, 0, size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dump a slice of the parameter list from index [j,k).
 */
ostream&
const_param_expr_list::dump_range(ostream& o, 
		const expr_dump_context& c, 
		const size_t j, const size_t k) const {
	INVARIANT(j <= k);
	INVARIANT(k <= size());
	if (j == k)
		return o;
	const_iterator i(begin() +j);
	const const_iterator e(begin() +k);
	NEVER_NULL(*i);
	(*i)->dump(o, c);
	for (i++; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->dump(o << ", ", c);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
const_param_expr_list::size(void) const {
	// STACKTRACE_VERBOSE;
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
count_ptr<const param_expr>
const_param_expr_list::operator [] (const size_t i) const {
	INVARIANT(i < size());
	return parent_type::operator[](i);
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
	const_iterator i(begin());
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
	STACKTRACE("const_expr_list::may_equivalent()");
if (cpl) {
	STACKTRACE("const vs. const");
	if (size() != cpl->size())
		return false;
	const_iterator i(begin());
	const_iterator j(cpl->begin());
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
	STACKTRACE("const vs. dynamic");
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i(begin());
	dynamic_param_expr_list::const_iterator j(dpl->begin());
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
/**
	\return true if all parameters are equivalent.  
 */
bool
const_param_expr_list::must_be_equivalent(const param_expr_list& p) const {
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &p);
	STACKTRACE("const_expr_list::must_equivalent()");
if (cpl) {
	STACKTRACE("const vs. const");
#if 0
	// use this
	return must_be_equivalent(*cpl);
#else
	if (size() != cpl->size())
		return false;
	const_iterator i(begin());
	const_iterator j(cpl->begin());
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
#endif
} else {
	STACKTRACE("const vs. dynamic");
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i(begin());
	dynamic_param_expr_list::const_iterator j(dpl->begin());
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
	\param s the number of strict formal parameters to compare
		before terminating.  
 */
bool
const_param_expr_list::must_be_equivalent(
		const this_type& cpl, const size_t s) const {
	INVARIANT(s <= size());
	INVARIANT(s <= cpl.size());
	if (size() != cpl.size())
		return false;
	const_iterator i(begin());
	const_iterator j(cpl.begin());
	const const_iterator e(begin() +s);
	for ( ; i!=e; i++, j++) {
		const count_ptr<const const_param>& ip(*i);
		const count_ptr<const const_param>& jp(*j);
		INVARIANT(ip && jp);
		if (!ip->must_be_equivalent_generic(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == cpl.begin() +s);		// sanity
	// if there more parameters, they should be checked
	for ( ; i!=end() && j!=cpl.end(); i++, j++) {
		const count_ptr<const const_param>& ip(*i);
		const count_ptr<const const_param>& jp(*j);
		if (ip && jp) {
			if (!ip->must_be_equivalent_generic(*jp))
				return false;
			// else continue
		}
		// else at least one of them is NULL, continue comparing
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compares the actuals starting with the relaxed actuals only
	against the argument list.  
	\pre the number of relaxed actuals is consistent with the 
		argument.  
 */
bool
const_param_expr_list::is_tail_equivalent(const this_type& cpl) const {
	const size_t arg_size = cpl.size();
	const size_t t_size  = size();
	INVARIANT(arg_size <= t_size);
	const size_t skip = t_size -arg_size;
	const_iterator i(begin() +skip);
	const_iterator j(cpl.begin());
	const const_iterator e(end());
	for ( ; i!=e; i++, j++) {
		const count_ptr<const const_param>& ip(*i);
		const count_ptr<const const_param>& jp(*j);
		INVARIANT(ip && jp);
		if (!ip->must_be_equivalent_generic(*jp))
			return false;
		// else continue checking...
	}
	INVARIANT(j == cpl.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	All parameters must match.
 */
bool
const_param_expr_list::must_be_equivalent(const this_type& cpl) const {
	return must_be_equivalent(cpl, size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return newly allocated copy of itself, always succeeds.  
	Eventually will add some context argument, though it is not needed
	because this is already constant.  
 */
param_expr_list::unroll_resolve_return_type
const_param_expr_list::unroll_resolve(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// safe to use default copy construction because
	// count_ptr's are copy-constructible
	return unroll_resolve_return_type(new const_param_expr_list(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks template actuals against formal parameters, 
	filling in the blanks with default values and expressions
	where available.  
	NOTE: non-constness.  
 */
good_bool
const_param_expr_list::certify_template_arguments(
		const template_formals_list_type& tfl) {
	const size_t a_size = size();
	const size_t f_size = tfl.size();
	template_formals_list_type::const_iterator f_iter(tfl.begin());
	const template_formals_list_type::const_iterator f_end(tfl.end());
if (a_size != f_size) {
	if (a_size)
		return good_bool(false);
	// else a_size == 0, passed actuals list is empty, 
	// try to fill in all default arguments
	for ( ; f_iter!=f_end; f_iter++) {
		const never_ptr<const param_value_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		// does default expression have to be constant to be
		// valid?  it should be allowed to
		// depend on other parameters.  
		const count_ptr<const const_param>
			default_expr(pinst->default_value()
				.is_a<const const_param>());
		if (!default_expr) {
			cerr << "ERROR: missing template actual at position "
				<< distance(tfl.begin(), f_iter)+1 <<
				" where no default value is given." << endl;
			return good_bool(false);
		} else {
			push_back(default_expr);
		}
	}
	// if it fails, then list will be incomplete.  
	// if this point is reached, then fill-in was successfull
	return good_bool(true);
} else {
	iterator p_iter(begin());
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_value_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		const count_ptr<const const_param> pex(*p_iter);
		const never_ptr<const param_value_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		if (pex) {
			// type-check assignment, conservative w.r.t. arrays
			if (!pinst->may_type_check_actual_param_expr(*pex).good) {
				cerr << "ERROR: template formal and actual "
					"types mismatch at position " <<
					distance(tfl.begin(), f_iter)+1
					<< ". " << endl;
				return good_bool(false);
			}
			// else continue checking successive arguments
		} else {
			// no parameter expression given, 
			// check for default -- if exists, use it, 
			// else is error
			// TODO: catch case where default is non-const
			// but this method is called.  
			const count_ptr<const param_expr>
				ex(pinst->default_value());
			const count_ptr<const const_param>
				default_expr(ex.is_a<const const_param>());
			if (ex && !default_expr) {
			ICE(cerr, 
				cerr << "(KNOWN BUG) "
					"in const_param_expr_list::"
					"certify_template_actuals(): " << endl;
				cerr << "\tgot non-const param default "
					"expression where const was expected."
					<< endl;
			)
			}
			if (!default_expr) {
				cerr << "ERROR: missing template actual at position "
					<< distance(tfl.begin(), f_iter)+1 <<
				" where no default value is given." << endl;
				return good_bool(false);
			} else {
				// else, actually assign it a copy in the list
				*p_iter = default_expr;
			}
		}
	}
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks template actuals against formal parameters, 
	expecting all non-NULL expressions.  
	This variant remains const, and does not supply default expressions.  
 */
good_bool
const_param_expr_list::certify_template_arguments_without_defaults(
		const template_formals_list_type& tfl) const {
	const size_t a_size = size();
	const size_t f_size = tfl.size();
	template_formals_list_type::const_iterator f_iter(tfl.begin());
	const template_formals_list_type::const_iterator f_end(tfl.end());
if (a_size != f_size) {
	cerr << "ERROR: number of relaxed template actuals doesn\'t match "
		"the number of formal parameters." << endl;
	cerr << "\tgot: " << a_size << ", expected: " << f_size << endl;
	return good_bool(false);
} else {
	const_iterator p_iter(begin());
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_value_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		const count_ptr<const const_param> pex(*p_iter);
		const never_ptr<const param_value_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		NEVER_NULL(pex);
		// type-check assignment, conservative w.r.t. arrays
		if (!pinst->may_type_check_actual_param_expr(*pex).good) {
			cerr << "ERROR: template formal and actual "
				"types mismatch at position " <<
				distance(tfl.begin(), f_iter)+1
				<< ". " << endl;
			return good_bool(false);
		}
		// else continue checking successive arguments
	}
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unlike certify_template_arguments, this only checks
	and doesn't fill in default arguments, hance const.  
	NOTE: in the context of type-checking template formals against
		actuals, some of the formal parameters may depend on
		the earlier actual parameters! 
		Thus we need contextual information to correctly
		handle such cases (common).
		The context needs to be passed to 
		value_collection::must_type_check_actual_param_expr().
 */
good_bool
const_param_expr_list::must_validate_template_arguments(
		const template_formals_list_type& tfl, 
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const size_t a_size = size();
	const size_t f_size = tfl.size();
	template_formals_list_type::const_iterator f_iter(tfl.begin());
	const template_formals_list_type::const_iterator f_end(tfl.end());
if (a_size != f_size) {
	// error message?
	return good_bool(false);
} else {
	const_iterator p_iter(begin());
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_value_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		const count_ptr<const const_param>& pex(*p_iter);
		const never_ptr<const param_value_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		NEVER_NULL(pex);
		// type-check assignment, conservative w.r.t. arrays
		if (!pinst->must_type_check_actual_param_expr(*pex, c).good) {
			cerr << "ERROR: type/size mismatch between "
				"template formal and actual." << endl;
			pex->dump(cerr << "\tgot: ", 
				expr_dump_context::default_value) << endl;
			pinst->dump(cerr << "\texpected: ", 
				dump_flags::verbose) << endl;
			return good_bool(false);
		}
		// else continue checking successive arguments
	}
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dereference and compare.
	Could go in a standard library.  
 */
bool
const_param_expr_list::less_ptr::operator () (
		const value_type& l, const value_type& r) const {
	NEVER_NULL(l);
	NEVER_NULL(r);
	return *l < *r;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overriding the default comparison (between 2 vectors lexicographical)
	Comparison is used by the footprint_manager of definitions.  
 */
bool
const_param_expr_list::operator < (const this_type& t) const {
	INVARIANT(size() == t.size());
	return std::lexicographical_compare(
		begin(), end(), t.begin(), t.end(), less_ptr());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
	Does not register itself persistently.  
 */
void
const_param_expr_list::collect_transient_info_base(
		persistent_object_manager& m) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const count_ptr<const const_param> ip(*i);
		ip->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
	Does register itself persistently.  
 */
void
const_param_expr_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
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
	const_iterator i(begin());
	const const_iterator e(end());
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
dynamic_param_expr_list::dynamic_param_expr_list(const size_t s) :
		param_expr_list(), parent_type() {
	util::reserve(AS_A(parent_type&, *this), s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_param_expr_list::~dynamic_param_expr_list() {
	STACKTRACE_DTOR("~dynamic_param_expr_list()");
#if 0
	cerr << "list contains " << size() << " pointers." << endl;
	dump(cerr) << endl;
	cerr << "reference counts in list:" << endl;
	const_iterator i(begin());
	const const_iterator e(end());
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
dynamic_param_expr_list::dump(ostream& o, const expr_dump_context& c) const {
	if (empty()) return o;
	// else at least 1 item in list
	const_iterator i(begin());
	const const_iterator e(end());
	if (*i)	(*i)->dump(o, c);
	else	o << "(null)";
	for (i++; i!=e; i++) {
		o << ", ";
		if (*i)	(*i)->dump(o, c);
		else	o << "(null)";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_param_expr_list::size(void) const {
	STACKTRACE_VERBOSE;
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
count_ptr<const param_expr>
dynamic_param_expr_list::operator [] (const size_t i) const {
	INVARIANT(i < size());
	return parent_type::operator[](i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::is_static_constant(void) const {
	const_iterator i(begin());
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
/**
	NOTE: when this is called, some expressions may be NULL.  
 */
bool
dynamic_param_expr_list::is_relaxed_formal_dependent(void) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		const count_ptr<const param_expr>& ip(*i);
		if (ip && ip->is_relaxed_formal_dependent()) {
			cerr << "ERROR: expression at position " <<
				distance(begin(), i)+1 <<
				" of param-expr-list depends on "
				"relaxed formal parameter." << endl;
			return true;
		}
		// else continue checking...
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if WANT_IS_TEMPLATE_DEPENDENT
bool
dynamic_param_expr_list::is_template_dependent(void) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		const count_ptr<const param_expr> ip(*i);
		NEVER_NULL(ip);	// nothing may be NULL at this point!
		if (ip->is_template_dependent())
			return true;
		// else continue checking...
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::is_loop_independent(void) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		const count_ptr<const param_expr> ip(*i);
		NEVER_NULL(ip);	// nothing may be NULL at this point!
		if (!ip->is_loop_independent())
			return false;
		// else continue checking...
	}
	return true;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_param_expr_list::may_be_initialized(void) const {
	const_iterator i(begin());
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
	const_iterator i(begin());
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
	const_iterator i(begin());
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
	STACKTRACE("dynamic_expr_list::may_equivalent()");
if (cpl) {
	STACKTRACE("dynamic vs. const");
	if (size() != cpl->size())
		return false;
	const_iterator i(begin());
	const_param_expr_list::const_iterator j(cpl->begin());
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
	STACKTRACE("dynamic vs. dynamic");
	const never_ptr<const dynamic_param_expr_list>
		dpl(IS_A(const dynamic_param_expr_list*, &p));
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i(begin());
	const_iterator j(dpl->begin());
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
	STACKTRACE("dynamic_expr_list::must_equivalent()");
if (cpl) {
	STACKTRACE("dynamic vs. const");
	if (size() != cpl->size())
		return false;
	const_iterator i(begin());
	const_param_expr_list::const_iterator j(cpl->begin());
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
	STACKTRACE("dynamic vs. dynamic");
	const never_ptr<const dynamic_param_expr_list>
		dpl(IS_A(const dynamic_param_expr_list*, &p));
	NEVER_NULL(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i(begin());
	const_iterator j(dpl->begin());
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
 */
param_expr_list::unroll_resolve_return_type
dynamic_param_expr_list::unroll_resolve(const unroll_context& c) const {
	typedef	unroll_resolve_return_type		return_type;
	STACKTRACE_VERBOSE;
	const return_type ret(new const_param_expr_list);
	NEVER_NULL(ret);
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const count_ptr<const param_expr> ip(*i);
		const count_ptr<const_param> pc(ip->unroll_resolve(c));
		if (pc) {
			ret->push_back(pc);
		} else {
			cerr << "ERROR in expression " <<
				distance(begin(), i)+1 <<
				" of param expr list: ";
			ip->dump(cerr, expr_dump_context::error_mode)
				<< endl;
			cerr << "ERROR in dynamic_param_expr_list::unroll_resolve()" << endl;
			return return_type(NULL);
		}
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks template actuals against formals.  
	If actual is missing, will use default value/expression from
	the formal value parameter if available.  
 */
good_bool
dynamic_param_expr_list::certify_template_arguments(
		const template_formals_list_type& tfl) {
	const size_t a_size = size();
	const size_t f_size = tfl.size();
	template_formals_list_type::const_iterator f_iter(tfl.begin());
	const template_formals_list_type::const_iterator f_end(tfl.end());
if (a_size != f_size) {
	if (a_size)
		return good_bool(false);
	// else a_size == 0, passed actuals list is empty, 
	// try to fill in all default arguments
	for ( ; f_iter!=f_end; f_iter++) {
		const never_ptr<const param_value_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		const count_ptr<const param_expr>
			default_expr(pinst->default_value());
		if (!default_expr) {
			cerr << "ERROR: missing template actual at position "
				<< distance(tfl.begin(), f_iter)+1 <<
				" where no default value is given." << endl;
			return good_bool(false);
		} else {
			push_back(default_expr);
		}
	}
	// if it fails, then list will be incomplete.  
	// if this point is reached, then fill-in was successfull
	return good_bool(true);
} else {
	iterator p_iter(begin());
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_value_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		const count_ptr<const param_expr> pex(*p_iter);
		const never_ptr<const param_value_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		if (pex) {
			// type-check assignment, conservative w.r.t. arrays
			if (!pinst->may_type_check_actual_param_expr(*pex).good) {
				cerr << "ERROR: template formal and actual "
					"types mismatch at position " <<
					distance(tfl.begin(), f_iter)+1
					<< ". " << endl;
				return good_bool(false);
			}
			// else continue checking successive arguments
		} else {
			// no parameter expression given, 
			// check for default -- if exists, use it, 
			// else is error
			const count_ptr<const param_expr>
				default_expr(pinst->default_value());
			if (!default_expr) {
				cerr << "ERROR: missing template actual at position "
					<< distance(tfl.begin(), f_iter)+1 <<
					" where no default value is given." << endl;
				return good_bool(false);
			} else {
				// else, actually assign it a copy in the list
				*p_iter = default_expr;
			}
		}
	}
	// end of checking reached, everything passed
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks template actuals against formals.  
	Does not automatically use default values.  
	\pre Actuals must be present in every position.  
 */
good_bool
dynamic_param_expr_list::certify_template_arguments_without_defaults(
		const template_formals_list_type& tfl) const {
	const size_t a_size = size();
	const size_t f_size = tfl.size();
	template_formals_list_type::const_iterator f_iter(tfl.begin());
	const template_formals_list_type::const_iterator f_end(tfl.end());
if (a_size != f_size) {
	cerr << "ERROR: number of relaxed template actuals doesn\'t match "
		"the number of formal parameters." << endl;
	cerr << "\tgot: " << a_size << ", expected: " << f_size << endl;
	return good_bool(false);
} else {
	const_iterator p_iter(begin());
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_value_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		const count_ptr<const param_expr> pex(*p_iter);
		const never_ptr<const param_value_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		NEVER_NULL(pex);
		// type-check assignment, conservative w.r.t. arrays
		if (!pinst->may_type_check_actual_param_expr(*pex).good) {
			cerr << "ERROR: template formal and actual "
				"types mismatch at position " <<
				distance(tfl.begin(), f_iter)+1
				<< ". " << endl;
			return good_bool(false);
		}
		// else continue checking successive arguments
	}
	// end of checking reached, everything passed
	return good_bool(true);
}
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
	const_iterator i(begin());
	const const_iterator e(end());
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
	const_iterator i(begin());
	const const_iterator e(end());
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
}	// end namepace entity
}	// end namepace HAC

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

#endif	// __HAC_OBJECT_EXPR_META_PARAM_EXPR_LIST_CC__

