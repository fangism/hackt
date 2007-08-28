/**
	\file "sim/chpsim/chpsim_dlfunction.h"
	This is the primary header to include for linking chpsim
	to dlopened libraries.  
	Try not to include other headers explicitly.  
	$Id: chpsim_dlfunction.h,v 1.4 2007/08/28 04:54:27 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__
#define	__HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__

#include "Object/expr/dlfunction.h"

// The following headers are needed for the API
#include "util/memory/count_ptr.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/cppcat.h"	// for uniquify

namespace HAC {
namespace entity {

//=============================================================================
// wrapped and type-checked automatic argument forwarding to native functions

// should these be inlined? or given static linkage?
// TODO: provide declaration macros
// TODO: use something like boost::function

template <typename R>
chp_function_return_type
auto_wrap_chp_function(R (*f)(void), const chp_function_argument_list_type&) {
	return make_chp_value((*f)());
}

chp_function_return_type
auto_wrap_chp_function(void (*f)(void), const chp_function_argument_list_type&) {
	(*f)();
	return chp_function_return_type(NULL);
}

template <typename R, typename A0>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0), const chp_function_argument_list_type& a) {
	return make_chp_value((*f)(extract_chp_value<A0>(a[0])));
}

template <typename A0>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0), const chp_function_argument_list_type& a) {
	(*f)(extract_chp_value<A0>(a[0]));
	return chp_function_return_type(NULL);
}

// wrapper for special case where argument type is already wrapped
template <typename R>
chp_function_return_type
auto_wrap_chp_function(R (*f)(const chp_function_argument_list_type&),
		const chp_function_argument_list_type& a) {
	return make_chp_value((*f)(a));
}

// wrapper for special case where argument type is already wrapped
// template <>
chp_function_return_type
auto_wrap_chp_function(void (*f)(const chp_function_argument_list_type&),
		const chp_function_argument_list_type& a) {
	(*f)(a);
	return chp_function_return_type(NULL);
}

template <typename R, typename A0, typename A1>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1), const chp_function_argument_list_type& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1])
	));
}

template <typename A0, typename A1>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1), const chp_function_argument_list_type& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1])
	);
	return chp_function_return_type(NULL);
}

template <typename R, typename A0, typename A1, typename A2>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2), const chp_function_argument_list_type& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2])
	));
}

template <typename A0, typename A1, typename A2>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1, A2), const chp_function_argument_list_type& a) {
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
		const chp_function_argument_list_type& a) {
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
		const chp_function_argument_list_type& a) {
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
		const chp_function_argument_list_type& a) {
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
		const chp_function_argument_list_type& a) {
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
		const chp_function_argument_list_type& a) {
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
		const chp_function_argument_list_type& a) {
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

template <typename R,
	typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2, A3, A4, A5, A6),
		const chp_function_argument_list_type& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4]),
		extract_chp_value<A5>(a[5]),
		extract_chp_value<A6>(a[6])
	));
}

template <typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1, A2, A3, A4, A5, A6),
		const chp_function_argument_list_type& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4]),
		extract_chp_value<A5>(a[5]),
		extract_chp_value<A6>(a[6])
	);
	return chp_function_return_type(NULL);
}

template <typename R,
	typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6, typename A7>
chp_function_return_type
auto_wrap_chp_function(R (*f)(A0, A1, A2, A3, A4, A5, A6, A7),
		const chp_function_argument_list_type& a) {
	return make_chp_value((*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4]),
		extract_chp_value<A5>(a[5]),
		extract_chp_value<A6>(a[6]),
		extract_chp_value<A7>(a[7])
	));
}

template <typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6, typename A7>
chp_function_return_type
auto_wrap_chp_function(void (*f)(A0, A1, A2, A3, A4, A5, A6, A7),
		const chp_function_argument_list_type& a) {
	(*f)(
		extract_chp_value<A0>(a[0]),
		extract_chp_value<A1>(a[1]),
		extract_chp_value<A2>(a[2]),
		extract_chp_value<A3>(a[3]),
		extract_chp_value<A4>(a[4]),
		extract_chp_value<A5>(a[5]),
		extract_chp_value<A6>(a[6]),
		extract_chp_value<A7>(a[7])
	);
	return chp_function_return_type(NULL);
}

// extend trivially to more arguments as necessary

// usage: return auto_wrap_chp_function(&func, <const chp_function_argument_list_type&>);

//=============================================================================
#define	USING_CHPSIM_DLFUNCTION_PROLOGUE				\
using HAC::entity::chp_function_return_type;				\
using HAC::entity::chp_function_registrar;				\
using HAC::entity::chp_function_argument_list_type;			\
using HAC::entity::int_value_type;					\
using HAC::entity::bool_value_type;					\
using HAC::entity::real_value_type;					\
using HAC::entity::string_value_type;					\
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
UNIQUIFY(fname ## _receipt_) (key, fname);

/**
	Name transformation, not for general use.  
 */
#define	WRAP_DLFUNCTION_NAME(fname)	UNIQUIFY(wrapped_ ## fname ## _)

/**
	Declare an object that auto-loads the named function on construction.
	Not intended for general use.  
 */
#define	REGISTER_DLFUNCTION(key, fname)					\
static const chp_function_registrar					\
UNIQUIFY(fname ## _receipt_) (key, WRAP_DLFUNCTION_NAME(fname));

/**
	Intended for general use.  
	\param key is the string for the function name.
	\param fname is the function in the local module to register.
 */
#define	CHP_DLFUNCTION_LOAD_DEFAULT(key, fname)				\
static									\
chp_function_return_type						\
WRAP_DLFUNCTION_NAME(fname) (const chp_function_argument_list_type& a) { \
	return auto_wrap_chp_function(fname, a);			\
}									\
REGISTER_DLFUNCTION(key, fname)

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__

