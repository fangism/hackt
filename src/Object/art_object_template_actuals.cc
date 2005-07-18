/**
	\file "Object/art_object_template_actuals.cc"
	Class implementation of template actuals.
	$Id: art_object_template_actuals.cc,v 1.1.4.9 2005/07/18 00:02:09 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "Object/art_object_template_actuals.h"
#include "Object/art_object_template_formals_manager.h"
#include "Object/art_object_unroll_context.h"
#include "Object/expr/param_expr.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class template_actuals method definitions
//=============================================================================

template_actuals::template_actuals() :
		strict_template_args(), relaxed_template_args() {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_actuals::~template_actuals() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out template arguments, strict and relaxed.  
	arbitrary: first set of angle brackets is mandatory, even if
	strict template args are NULL.  
 */
ostream&
template_actuals::dump(ostream& o) const {
	o << '<';
	if (strict_template_args)
		strict_template_args->dump(o);
	o << '>';
	if (relaxed_template_args)
		relaxed_template_args->dump(o << '<') << '>';
	return o;
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
	if (strict_template_args) {
		const size_t s(strict_template_args->size());
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
			strict_template_args->dump(cerr << '\t') << endl;
			err = true;
		}
	}
	if (relaxed_template_args) {
		rr = relaxed_template_args->unroll_resolve(c);
		if (!rr) {
			cerr << "ERROR in resolving relaxed template actuals."
				<< endl;
			relaxed_template_args->dump(cerr << '\t') << endl;
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
	unroll_context c;
	const template_actuals_transformer tx(c, a, m);
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

