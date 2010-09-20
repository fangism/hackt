/**
	\file "util/function_traits.h"
	Argument and return type trait information of functions.
	Inspired somewhat by boost::function, and boost::result_of.
	$Id: function_traits.h,v 1.1.2.1 2010/09/20 18:37:38 fang Exp $
 */

#ifndef	__UTIL_FUNCTION_TRAITS_H__
#define	__UTIL_FUNCTION_TRAITS_H__

#include "util/typeof.h"

namespace util {
//=============================================================================
/**
	Current limitations: only works on pointer-to-function.
	TODO: add support for pointer-to-member-function.
 */
template <typename F>
class function_traits;

// define by specialization
template <typename R>
struct function_traits<R (*)(void)> {
	typedef	R		return_type;
	typedef	void		arg1_type;
	typedef	arg1_type	arg_type;
	enum {	arity = 0	};
};

template <typename R, typename A1>
struct function_traits<R (*)(A1)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	enum {	arity = 1	};
};

template <typename R, typename A1, typename A2>
struct function_traits<R (*)(A1, A2)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	typedef	A2		arg2_type;
	enum {	arity = 2	};
};

template <typename R, typename A1, typename A2, typename A3>
struct function_traits<R (*)(A1, A2, A3)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	typedef	A2		arg2_type;
	typedef	A3		arg3_type;
	enum {	arity = 3	};
};

template <typename R, typename A1, typename A2, typename A3, typename A4>
struct function_traits<R (*)(A1, A2, A3, A4)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	typedef	A2		arg2_type;
	typedef	A3		arg3_type;
	typedef	A4		arg4_type;
	enum {	arity = 4	};
};

template <typename R, typename A1, typename A2, typename A3, typename A4, 
	typename A5>
struct function_traits<R (*)(A1, A2, A3, A4, A5)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	typedef	A2		arg2_type;
	typedef	A3		arg3_type;
	typedef	A4		arg4_type;
	typedef	A5		arg5_type;
	enum {	arity = 5	};
};

template <typename R, typename A1, typename A2, typename A3, typename A4, 
	typename A5, typename A6>
struct function_traits<R (*)(A1, A2, A3, A4, A5, A6)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	typedef	A2		arg2_type;
	typedef	A3		arg3_type;
	typedef	A4		arg4_type;
	typedef	A5		arg5_type;
	typedef	A6		arg6_type;
	enum {	arity = 6	};
};

template <typename R, typename A1, typename A2, typename A3, typename A4, 
	typename A5, typename A6, typename A7>
struct function_traits<R (*)(A1, A2, A3, A4, A5, A6, A7)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	typedef	A2		arg2_type;
	typedef	A3		arg3_type;
	typedef	A4		arg4_type;
	typedef	A5		arg5_type;
	typedef	A6		arg6_type;
	typedef	A7		arg7_type;
	enum {	arity = 7	};
};

template <typename R, typename A1, typename A2, typename A3, typename A4, 
	typename A5, typename A6, typename A7, typename A8>
struct function_traits<R (*)(A1, A2, A3, A4, A5, A6, A7, A8)> {
	typedef	R		return_type;
	typedef	A1		arg1_type;
	typedef	A2		arg2_type;
	typedef	A3		arg3_type;
	typedef	A4		arg4_type;
	typedef	A5		arg5_type;
	typedef	A6		arg6_type;
	typedef	A7		arg7_type;
	typedef	A8		arg8_type;
	enum {	arity = 8	};
};

// add more as needed ...

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename F>
struct return_type {
	typedef typename function_traits<F>::return_type	type;
};

template <typename F>
struct arity {
	enum { value = function_traits<F>::arity	};
};

//=============================================================================
/**
	Macro for conveniently refering to the return type of a function.
 */
#define	RESULT_OF(f)	util::return_type<TYPEOF(&f)>::type
#define	ARITY(f)	util::arity<TYPEOF(&f)>::value

//=============================================================================
}	// end namespace util

#endif	// __UTIL_FUNCTION_TRAITS_H__
