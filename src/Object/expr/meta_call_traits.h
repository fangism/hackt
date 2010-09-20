/**
	\file "Object/expr/meta_call_traits.h"
	Meta-functions can be dynamically added.
	This header should be installed (eventually).  
	$Id: meta_call_traits.h,v 1.1.2.2 2010/09/20 18:37:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_CALL_TRAITS_H__
#define	__HAC_OBJECT_EXPR_META_CALL_TRAITS_H__

#include <iosfwd>
#include <string>
#include <stdexcept>
#include "util/memory/count_ptr.h"
#include "Object/expr/types.h"
#include "Object/expr/dlfunction_fwd.h"

// for auto-wrap functions
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace entity {
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// call_traits

/**
	Template policy for passing/returning by value or reference.
 */
template <class T>
struct meta_call_traits;

template <>
struct meta_call_traits<pint_value_type> {
	typedef	pint_value_type		type;
	typedef	pint_value_type		return_type;
	typedef	const pint_value_type	argument_type;
};

template <>
struct meta_call_traits<pbool_value_type> {
	typedef	pbool_value_type	type;
	typedef	pbool_value_type	return_type;
	typedef	const pbool_value_type	argument_type;
};

/**
	Consider changing to reference when upgrading to doubles.  
 */
template <>
struct meta_call_traits<real_value_type> {
	typedef	preal_value_type	type;
	typedef	preal_value_type	return_type;
	typedef	const preal_value_type	argument_type;
};

template <>
struct meta_call_traits<pstring_value_type> {
	typedef	pstring_value_type		type;
//	typedef	const pstring_value_type&	return_type;
// should be fast, std::string is shallow ref-counted copy-on-write
	typedef	pstring_value_type		return_type;
	typedef	const pstring_value_type&	argument_type;
};

template <>
struct meta_call_traits<const pstring_value_type&> {
	typedef	pstring_value_type		type;
//	typedef	const pstring_value_type&	return_type;
// should be fast, std::string is shallow ref-counted copy-on-write
	typedef	pstring_value_type		return_type;
	typedef	const pstring_value_type&	argument_type;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// wrappers to extract native type from dynamic HAC type
// each of these throw exceptions in the event of type-check failure

extern
pint_value_type
extract_pint(const const_param&);

inline
int_value_type
extract_int(const const_param& p) {
	return extract_pint(p);
}

extern
pbool_value_type
extract_pbool(const const_param&);

inline
bool_value_type
extract_bool(const const_param& p) {
	return extract_pbool(p);
}

// currently float, see "Object/expr/types.h"
extern
preal_value_type
extract_preal(const const_param&);

inline
real_value_type
extract_real(const const_param& p) {
	return extract_preal(p);
}

extern
pstring_value_type
extract_pstring(const const_param&);

inline
string_value_type
extract_string(const const_param& p) {
	return extract_pstring(p);
}

/**
	intentionally does not use template argument deduction
	Also only explicitly instantiated/specialized for select types.  
	Primary template is undefined.
 */
template <typename V>
typename meta_call_traits<V>::return_type
extract_meta_value(const meta_function_const_argument_type&);

// forward declare specializations
template <>
pint_value_type
extract_meta_value<pint_value_type>(const meta_function_const_argument_type&);

template <>
pbool_value_type
extract_meta_value<pbool_value_type>(const meta_function_const_argument_type&);

template <>
preal_value_type
extract_meta_value<preal_value_type>(const meta_function_const_argument_type&);

template <>
pstring_value_type
extract_meta_value<pstring_value_type>(const meta_function_const_argument_type&);

template <>
inline
pstring_value_type
extract_meta_value<const pstring_value_type&>(
		const meta_function_const_argument_type& a) {
	return extract_meta_value<pstring_value_type>(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// these require count_ptr and const_param to be complete types...
extern
meta_function_return_type
make_meta_value(const pint_value_type);

extern
meta_function_return_type
make_meta_value(const pbool_value_type);

extern
meta_function_return_type
make_meta_value(const preal_value_type);

extern
meta_function_return_type
make_meta_value(const pstring_value_type&);	// pass-by-reference

//=============================================================================
// wrapped and type-checked automatic argument forwarding to native functions

// could use function_traits::arity...
#define	VERIFY_WRAPPED_ARGS(vec, sz)					\
	verify_wrapped_args(vec.size(), sz)

/**
	\throw exception if there is a mismatch of formals to actuals.
 */
extern
void
verify_wrapped_args(const size_t a, const size_t f);


// should these be inlined? or given static linkage?
// TODO: provide declaration macros
// TODO: use something like boost::function

template <typename R>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(void),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 0);
	return make_meta_value((*f)());
}

static
inline
meta_function_return_type
auto_wrap_dlfunction(void (*f)(void),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 0);
	(*f)();
	return meta_function_return_type(NULL);
}

template <typename R, typename A0>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0), const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 1);
	return make_meta_value((*f)(extract_meta_value<A0>(a[0])));
}

template <typename A0>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 1);
	(*f)(extract_meta_value<A0>(a[0]));
	return meta_function_return_type(NULL);
}

// wrapper for special case where argument type is already wrapped
template <typename R>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(const meta_function_argument_list_type&),
		const meta_function_argument_list_type& a) {
	return make_meta_value((*f)(a));
}

// wrapper for special case where argument type is already wrapped
// template <>
static
inline
meta_function_return_type
auto_wrap_dlfunction(void (*f)(const meta_function_argument_list_type&),
		const meta_function_argument_list_type& a) {
	(*f)(a);
	return meta_function_return_type(NULL);
}

template <typename R, typename A0, typename A1>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0, A1),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 2);
	return make_meta_value((*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1])
	));
}

template <typename A0, typename A1>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0, A1),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 2);
	(*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1])
	);
	return meta_function_return_type(NULL);
}

template <typename R, typename A0, typename A1, typename A2>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0, A1, A2),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 3);
	return make_meta_value((*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2])
	));
}

template <typename A0, typename A1, typename A2>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0, A1, A2),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 3);
	(*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2])
	);
	return meta_function_return_type(NULL);
}

template <typename R, typename A0, typename A1, typename A2, typename A3>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0, A1, A2, A3),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 4);
	return make_meta_value((*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3])
	));
}

template <typename A0, typename A1, typename A2, typename A3>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0, A1, A2, A3),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 4);
	(*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3])
	);
	return meta_function_return_type(NULL);
}

template <typename R,
	typename A0, typename A1, typename A2, typename A3, typename A4>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0, A1, A2, A3, A4),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 5);
	return make_meta_value((*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4])
	));
}

template <typename A0, typename A1, typename A2, typename A3, typename A4>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0, A1, A2, A3, A4),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 5);
	(*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4])
	);
	return meta_function_return_type(NULL);
}

template <typename R,
	typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0, A1, A2, A3, A4, A5),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 6);
	return make_meta_value((*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4]),
		extract_meta_value<A5>(a[5])
	));
}

template <typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0, A1, A2, A3, A4, A5),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 6);
	(*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4]),
		extract_meta_value<A5>(a[5])
	);
	return meta_function_return_type(NULL);
}

template <typename R,
	typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0, A1, A2, A3, A4, A5, A6),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 7);
	return make_meta_value((*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4]),
		extract_meta_value<A5>(a[5]),
		extract_meta_value<A6>(a[6])
	));
}

template <typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0, A1, A2, A3, A4, A5, A6),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 7);
	(*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4]),
		extract_meta_value<A5>(a[5]),
		extract_meta_value<A6>(a[6])
	);
	return meta_function_return_type(NULL);
}

template <typename R,
	typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6, typename A7>
meta_function_return_type
auto_wrap_dlfunction(R (*f)(A0, A1, A2, A3, A4, A5, A6, A7),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 8);
	return make_meta_value((*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4]),
		extract_meta_value<A5>(a[5]),
		extract_meta_value<A6>(a[6]),
		extract_meta_value<A7>(a[7])
	));
}

template <typename A0, typename A1, typename A2, typename A3, 
	typename A4, typename A5, typename A6, typename A7>
meta_function_return_type
auto_wrap_dlfunction(void (*f)(A0, A1, A2, A3, A4, A5, A6, A7),
		const meta_function_argument_list_type& a) {
	VERIFY_WRAPPED_ARGS(a, 8);
	(*f)(
		extract_meta_value<A0>(a[0]),
		extract_meta_value<A1>(a[1]),
		extract_meta_value<A2>(a[2]),
		extract_meta_value<A3>(a[3]),
		extract_meta_value<A4>(a[4]),
		extract_meta_value<A5>(a[5]),
		extract_meta_value<A6>(a[6]),
		extract_meta_value<A7>(a[7])
	);
	return meta_function_return_type(NULL);
}

// extend trivially to more arguments as necessary

// usage: return auto_wrap_dlfunction(&func, <const meta_function_argument_list_type&>);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_META_CALL_TRAITS_H__

