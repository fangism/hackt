/**
	\file "Object/art_object_template_actuals.cc"
	Class implementation of template actuals.
	$Id: art_object_template_actuals.cc,v 1.1.4.2 2005/06/30 23:22:24 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "Object/art_object_template_actuals.h"
#include "Object/art_object_template_formals_manager.h"
#include "Object/art_object_unroll_context.h"
#include "Object/art_object_expr_const.h"	// for const_param_expr_list
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
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
		const arg_list_ptr_type& r) :
		strict_template_args(s), relaxed_template_args(r) {
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
		const size_t s = strict_template_args->size();
		return (i < s) ?
			(*strict_template_args)[i] :
			(*relaxed_template_args)[i-s];
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
			relaxed_template_args.is_a<const_param_expr_list>() ||
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
template_actuals::arg_list_ptr_type
template_actuals::get_relaxed_args(void) {
	return relaxed_template_args;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: clarify when template actuals are resolved.  
 */
template_actuals
template_actuals::unroll_resolve(unroll_context& c) const {
	arg_list_ptr_type sr, rr;
	if (strict_template_args)
		sr = strict_template_args->unroll_resolve(c);
	if (relaxed_template_args)
		rr = relaxed_template_args->unroll_resolve(c);
	return this_type(sr, rr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
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
#endif

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
		m.load_object_once(
			const_cast<expr_list_type*>(&*strict_template_args));
	if (relaxed_template_args)
		m.load_object_once(
			const_cast<expr_list_type*>(&*relaxed_template_args));
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

