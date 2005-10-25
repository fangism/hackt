/**
	\file "Object/type/template_actuals.cc"
	Class implementation of template actuals.
	This file was previously named "Object/type/template_actuals.cc"
	$Id: template_actuals.cc,v 1.4 2005/10/25 20:51:58 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <algorithm>
#include <iterator>
#include "Object/type/template_actuals.h"
#include "Object/def/template_formals_manager.h"
#include "Object/unroll/unroll_context.h"
#include "Object/expr/param_expr.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/expr_dump_context.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"

#define	ADDRESS_IF_NULL(x)						\
	if (x) { cerr << &*x; } else { cerr << "(null)"; }

#if ENABLE_STACKTRACE
#define	CHECK_ARG_ADDRESSES						\
	cerr << "template_actuals @ " << this << ": ";			\
	ADDRESS_IF_NULL(strict_template_args);				\
	cerr << ", ";							\
	ADDRESS_IF_NULL(relaxed_template_args);				\
	cerr << endl;
#else
#define	CHECK_ARG_ADDRESSES
#endif

namespace ART {
namespace entity {
using std::copy;
using std::back_inserter;
#include "util/using_ostream.h"
//=============================================================================
// class template_actuals method definitions
//=============================================================================

template_actuals::template_actuals() :
		strict_template_args(), relaxed_template_args() {
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "ctor ";
	CHECK_ARG_ADDRESSES
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Argument list constructor.  
	\param s list of strict template actuals (may be NULL)
	\param r list of relaxed template actuals (may be NULL)
 */
template_actuals::template_actuals(const arg_list_ptr_type& s, 
		const const_arg_list_ptr_type& r) :
		strict_template_args(s), relaxed_template_args(r) {
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "ctor ";
	CHECK_ARG_ADDRESSES
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Special purpose construct designated for fusing 
	types actuals together to form a complete, strict type.  
	See its uses in *_type_reference::merge_relaxed_actuals.
	\param t to existing set of template actuals.  
	\param a non-NULL pointer to relaxed template args.
	\pre t must not already have relaxed actuals.  
 */
template_actuals::template_actuals(const template_actuals& t, 
		const const_arg_list_ptr_type& a) :
		strict_template_args(t.strict_template_args), 
		relaxed_template_args(a) {
	NEVER_NULL(a);
	INVARIANT(!t.relaxed_template_args);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "ctor ";
	CHECK_ARG_ADDRESSES
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_actuals::~template_actuals() {
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "dtor ";
	CHECK_ARG_ADDRESSES
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out template arguments, strict and relaxed.  
	arbitrary: first set of angle brackets is mandatory, even if
	strict template args are NULL.  
	TODO: take expr_dump_context argument.  
 */
ostream&
template_actuals::dump(ostream& o) const {
	o << '<';
	if (strict_template_args)
		strict_template_args->dump(o, expr_dump_context::default_value);
	o << '>';
	if (relaxed_template_args)
		relaxed_template_args->dump(o << '<', 
			expr_dump_context::default_value) << '>';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unifies strict and relaxed parameters into a single list.  
	\pre strict
 */
count_ptr<const const_param_expr_list>
template_actuals::make_const_param_list(void) const {
	const count_ptr<const_param_expr_list> ret(new const_param_expr_list);
	if (strict_template_args) {
		const count_ptr<const const_param_expr_list>
			csa(strict_template_args.
				is_a<const const_param_expr_list>());
		INVARIANT(csa);
		copy(csa->begin(), csa->end(), back_inserter(*ret));
	}
	if (relaxed_template_args) {
		const count_ptr<const const_param_expr_list>
			rsa(relaxed_template_args.
				is_a<const const_param_expr_list>());
		INVARIANT(rsa);
		copy(rsa->begin(), rsa->end(), back_inserter(*ret));
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
template_actuals::is_resolved(void) const {
	return (!strict_template_args ||
		strict_template_args.is_a<const const_param_expr_list>()) && 
		(!relaxed_template_args ||
		relaxed_template_args.is_a<const const_param_expr_list>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_actuals::operator bool () const {
	return strict_template_args || relaxed_template_args;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	0-indexed parameter lookup.  
	\param i is index into template arguments list
	\pre i is already range-checked.  
 */
count_ptr<const param_expr>
template_actuals::operator [] (const size_t i) const {
	STACKTRACE_VERBOSE;
	if (strict_template_args) {
#if 1
		CHECK_ARG_ADDRESSES
//		strict_template_args->what(cerr) << endl;
#endif
		const size_t s = strict_template_args->size();
		if (i < s) {
			return (*strict_template_args)[i];
		} else {
			NEVER_NULL(relaxed_template_args);
			INVARIANT(i < s +relaxed_template_args->size());
			return (*relaxed_template_args)[i-s];
		}
	} else {
		INVARIANT(relaxed_template_args);
		return (*relaxed_template_args)[i];
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return Whether or not all params have been resolved to constants.  
	NULL parameter lists count as constant.  
 */
bool
template_actuals::is_constant(void) const {
	return (!strict_template_args ||
			strict_template_args.is_a<const_param_expr_list>() ||
			strict_template_args->is_static_constant()) &&
		(!relaxed_template_args ||
			relaxed_template_args.is_a<const const_param_expr_list>() ||
			relaxed_template_args->is_static_constant());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_actuals::const_arg_list_ptr_type
template_actuals::get_strict_args(void) const {
	return strict_template_args;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_actuals::arg_list_ptr_type
template_actuals::get_strict_args(void) {
	return strict_template_args;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_actuals::const_arg_list_ptr_type
template_actuals::get_relaxed_args(void) const {
	return relaxed_template_args;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: clarify when template actuals are resolved.  
	TODO: handle when template parameters depend on other 
		defaulting template parameters.  
		Template formals are NOT available here, 
		so we must require the caller to provide them in context.  
 */
template_actuals
template_actuals::unroll_resolve(const unroll_context& c) const {
	bool err = false;
	arg_list_ptr_type sr, rr;
	if (strict_template_args) {
		sr = strict_template_args->unroll_resolve(c);
		if (!sr) {
			cerr << "ERROR in resolving strict template actuals."
				<< endl;
			strict_template_args->dump(cerr << '\t', 
				expr_dump_context::error_mode) << endl;
			err = true;
		}
	}
	if (relaxed_template_args) {
		rr = relaxed_template_args->unroll_resolve(c);
		if (!rr) {
			cerr << "ERROR in resolving relaxed template actuals."
				<< endl;
			relaxed_template_args->dump(cerr << '\t', 
				expr_dump_context::error_mode) << endl;
			err = true;
		}
	}
	return err ? this_type() : this_type(sr, rr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a set of resolved, constant template parameters.
	\param m the map from instance reference to template actuals.  
		Value reference in this set of actuals that depend
		on template parameters will map to the actuals
		provided in argument a.  
 */
template_actuals
template_actuals::transform_template_actuals(const this_type& a, 
		const template_formals_manager& m) const {
	INVARIANT(a.is_constant());
	const unroll_context c(a, m);
	return unroll_resolve(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compatibility is defined as both template actuals begin
	relaxed or both being strict.  
	If they differ, they are incompatible.  
	NOTE: this does not compare the strict template arguments.  
 */
bool
template_actuals::is_strictly_compatible_with(const this_type& a) const {
	return !(relaxed_template_args ^ a.relaxed_template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	May-equivalence between two sets of template args.  
	Relaxed equivalence requires only that the strict
	template args match.  
	Completely ignores relaxed template arguments.  
	\return true if arguments may be relaxed-equivalent.  
 */
bool
template_actuals::may_be_relaxed_equivalent(const this_type& t) const {
	STACKTRACE_VERBOSE;
	if (strict_template_args) {
		if (!t.strict_template_args)
			return false;
		else return strict_template_args->may_be_equivalent(
			*t.strict_template_args);
	} else {
		return !t.strict_template_args;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must-equivalence between two sets of template args.  
	Disregards relaxed template arguments.  
	\return true if arguments must be equivalent.  
 */
bool
template_actuals::must_be_relaxed_equivalent(const this_type& t) const {
	STACKTRACE_VERBOSE;
	if (strict_template_args) {
		if (!t.strict_template_args)
			return false;
		else return strict_template_args->must_be_equivalent(
			*t.strict_template_args);
	} else {
		return !t.strict_template_args;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Stronger than relaxed equivalence, strict equivalence requires
	that the relaxed template arguments be equivalent.  
	If either relaxed template args is missing, then conservatively
	return true.  
	NOTE: requivalent relaxed arguments should have empty lists, 
		neither list pointer should be NULL.  
 */
bool
template_actuals::may_be_strict_equivalent(const this_type& t) const {
	STACKTRACE_VERBOSE;
	if (!may_be_relaxed_equivalent(t))
		return false;
	else if (relaxed_template_args && t.relaxed_template_args)
		return relaxed_template_args->may_be_equivalent(
			*t.relaxed_template_args);
	// if either is NULL return true
	else return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Stronger than relaxed equivalence, strict equivalence requires
	that the relaxed template arguments be equivalent.  
	If either relaxed template args is missing, then conservatively
	return false.  
	NOTE: requivalent relaxed arguments should have empty lists, 
		neither list pointer should be NULL.  
 */
bool
template_actuals::must_be_strict_equivalent(const this_type& t) const {
	STACKTRACE_VERBOSE;
	if (!must_be_relaxed_equivalent(t))
		return false;
	else if (relaxed_template_args && t.relaxed_template_args)
		return relaxed_template_args->must_be_equivalent(
			*t.relaxed_template_args);
	// if both are NULL, return true
	else return (!relaxed_template_args && !t.relaxed_template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
template_actuals::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (strict_template_args)
		strict_template_args->collect_transient_info(m);
	if (relaxed_template_args)
		relaxed_template_args->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
template_actuals::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, strict_template_args);
	m.write_pointer(o, relaxed_template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: document why loading pointers is necessary (if it is)
 */
void
template_actuals::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, strict_template_args);
	m.read_pointer(i, relaxed_template_args);
	if (strict_template_args)
		m.load_object_once(&*strict_template_args);
	if (relaxed_template_args)
		m.load_object_once(
			const_cast<expr_list_type*>(&*relaxed_template_args));
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

