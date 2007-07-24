/**
	\file "sim/chpsim/chpsim_dlfunction.h"
	This is the primary header to include for linking chpsim
	to dlopened libraries.  
	Try not to include other headers explicitly.  
	$Id: chpsim_dlfunction.h,v 1.1.2.1 2007/07/24 03:35:21 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__
#define	__HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__

#include "Object/expr/dlfunction.h"

// The following headers are needed for the API
#include "util/memory/count_ptr.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace entity {

//=============================================================================
// wrapped and type-checked automatic argument forwarding to native functions

// TODO: provide declaration macros
// TODO: use something like boost::function

template <typename R>
chp_function_return_type
auto_wrap_chp_function(R (*f)(void), const const_param_expr_list&) {
	return make_chp_value((*f)());
}

void
auto_wrap_chp_function(void (*f)(void), const const_param_expr_list&) {
	(*f)();
}

template <typename R, typename A0>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0), const const_param_expr_list& a) {
	return make_chp_value((*f)(extract_chp_value<A0>(a[0])));
}

template <typename A0>
void
auto_wrap_chp_function(void (*f)(A0), const const_param_expr_list& a) {
	(*f)(extract_chp_value<A0>(a[0]));
}

template <typename R, typename A0, typename A1>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1), const const_param_expr_list& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1])
	));
}

template <typename A0, typename A1>
void
auto_wrap_chp_function(void (*f)(A0, A1), const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1])
	);
}

template <typename R, typename A0, typename A1, typename A2>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2), const const_param_expr_list& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2])
	));
}

template <typename A0, typename A1, typename A2>
void
auto_wrap_chp_function(void (*f)(A0, A1, A2), const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2])
	);
}

template <typename R, typename A0, typename A1, typename A2, typename A3>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2, A3), const const_param_expr_list& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3])
	));
}

template <typename A0, typename A1, typename A2, typename A3>
void
auto_wrap_chp_function(void (*f)(A0, A1, A2, A3), const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3])
	);
}

// extend trivially to more arguments as necessary

// usage: return auto_wrap_chp_function(&func, <const const_param_expr_list&>);

//=============================================================================
#define	USING_CHPSIM_DLFUNCTION_PROLOGUE				\
using HAC::entity::chp_function_return_type;				\
using HAC::entity::const_param_expr_list;

//=============================================================================
#define	WRAP_DLFUNCTION_NAME(fname)		wrapped_ ## fname

/**
	Declare an object that auto-loads the named function on construction.
 */
#define	REGISTER_DLFUNCTION(key, fname)					\
static const chp_function_registrar					\
fname ## _receipt (key, WRAP_DLFUNCTION_NAME(fname));

/**
	\param key is the string for the function name.
	\param fname is the function in the local module to register.
 */
#define	CHP_DLFUNCTION_LOAD_DEFAULT(key, fname)				\
static									\
chp_function_return_type						\
WRAP_DLFUNCTION_NAME(fname) (const const_param_expr_list& a) {		\
	return auto_wrap_chp_function(fname, a);			\
}									\
REGISTER_DLFUNCTION(key, fname)

/**
	and because I'm only a 3th degree black belt in C++ TMP, 
	here's a variant of the macro for void return types.  
 */
#define	CHP_DLFUNCTION_LOAD_VOID_DEFAULT(key, fname)			\
static									\
void									\
WRAP_DLFUNCTION_NAME(fname) (const const_param_expr_list& a) {		\
	auto_wrap_chp_function(fname, a);				\
}									\
REGISTER_DLFUNCTION(key, fname)

//=============================================================================
#if 0
// EXAMPLE:

static
int
my_gcd(int a, int b) { ... }

static
chp_function_return_type
wrap_my_gcd(const HAC::entity::const_param_expr_list& a) {
	return make_chp_value(
		my_gcd(
			extract_int(a[0]), 
			extract_int(a[1])
		)
	);
	// or return auto_wrap_chp_function(&my_gcd, a);
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__

