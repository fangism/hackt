/**
	\file "sim/chpsim/chpsim_dlfunction.h"
	This is the primary header to include for linking chpsim
	to dlopened libraries.  
	Try not to include other headers explicitly.  
	$Id: chpsim_dlfunction.h,v 1.2.2.1 2007/08/09 23:05:48 fang Exp $
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

chp_function_return_type
auto_wrap_chp_function(void (*f)(void), const const_param_expr_list&) {
	(*f)();
	return chp_function_return_type(NULL);
}

template <typename R, typename A0>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0), const const_param_expr_list& a) {
	return make_chp_value((*f)(extract_chp_value<A0>(a[0])));
}

template <typename A0>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0), const const_param_expr_list& a) {
	(*f)(extract_chp_value<A0>(a[0]));
	return chp_function_return_type(NULL);
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
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1), const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1])
	);
	return chp_function_return_type(NULL);
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
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1, A2), const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2])
	);
	return chp_function_return_type(NULL);
}

template <typename R, typename A0, typename A1, typename A2, typename A3>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2, A3),
		const const_param_expr_list& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3])
	));
}

template <typename A0, typename A1, typename A2, typename A3>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1, A2, A3),
		const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3])
	);
	return chp_function_return_type(NULL);
}

template <typename R,
	typename A0, typename A1, typename A2, typename A3, typename A4>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2, A3, A4),
		const const_param_expr_list& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4])
	));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1, A2, A3, A4),
		const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4])
	);
	return chp_function_return_type(NULL);
}

template <typename R,
	typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2, A3, A4, A5),
		const const_param_expr_list& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4]),
		extract_chp_value<A5>(a[5])
	));
}

template <typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1, A2, A3, A4, A5),
		const const_param_expr_list& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4]),
		extract_chp_value<A5>(a[5])
	);
	return chp_function_return_type(NULL);
}

// extend trivially to more arguments as necessary

// usage: return auto_wrap_chp_function(&func, <const const_param_expr_list&>);

//=============================================================================
#define	USING_CHPSIM_DLFUNCTION_PROLOGUE				\
using HAC::entity::chp_function_return_type;				\
using HAC::entity::chp_function_registrar;				\
using HAC::entity::int_value_type;					\
using HAC::entity::bool_value_type;					\
using HAC::entity::real_value_type;					\
using HAC::entity::const_param_expr_list;				\
using HAC::entity::extract_chp_value;					\
using HAC::entity::make_chp_value;

//=============================================================================
/**
	Declare an object that auto-loads the named function on construction.
	The function name is retained, not wrapped.  
	Part of public interface for 'advanced' users.
 */
#define	REGISTER_DLFUNCTION_RAW(key, fname)				\
static const chp_function_registrar					\
fname ## _receipt (key, fname);

/**
	Name transformation, not for general use.  
 */
#define	WRAP_DLFUNCTION_NAME(fname)		wrapped_ ## fname

/**
	Declare an object that auto-loads the named function on construction.
	Not intended for general use.  
 */
#define	REGISTER_DLFUNCTION(key, fname)					\
static const chp_function_registrar					\
fname ## _receipt (key, WRAP_DLFUNCTION_NAME(fname));

/**
	Intended for general use.  
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

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__

