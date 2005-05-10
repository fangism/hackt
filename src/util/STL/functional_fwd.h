/**
	\file "util/STL/functional_fwd.h"
	Forward declarations of the <functional> header.
	$Id: functional_fwd.h,v 1.3 2005/05/10 04:51:31 fang Exp $
 */

#ifndef	__UTIL_STL_FUNCTIONAL_FWD_H__
#define	__UTIL_STL_FUNCTIONAL_FWD_H__

namespace std {

template <class Arg, class Result>
struct unary_function;

template <class Arg1, class Arg2, class Result>
struct binary_function;

template <class T>
struct plus;

template <class T>
struct minus;

template <class T>
struct multiplies;

template <class T>
struct divides;

template <class T>
struct modulus;

template <class T>
struct negate;

template <class T>
struct equal_to;

template <class T>
struct not_equal_to;

template <class T>
struct greater;

template <class T>
struct less;

template <class T>
struct greter_equal;

template <class T>
struct less_equal;

template <class T>
struct logical_and;

template <class T>
struct logical_or;

template <class T>
struct logical_not;

template <class P>
class unary_negate;

template <class P>
class binary_negate;

template <class O>
class binder1st;

template <class O>
class binder2nd;

template <class O, class T>
binder1st<O>
bind1st(const O&, const T&);

template <class O, class T>
binder2nd<O>
bind2nd(const O&, const T&);

template <class P>
struct _Select1st;

template <class P>
struct _Select2nd;

template <class Arg, class Result>
class pointer_to_unary_function;

template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function;

template <class Arg, class Result>
pointer_to_unary_function<Arg, Result>
ptr_fun(Result (*) (Arg));

template <class Arg1, class Arg2, class Result>
pointer_to_binary_function<Arg1, Arg2, Result>
ptr_fun(Result (*) (Arg1, Arg2));

template <class Ret, class T>
class mem_fun_t;

template <class Ret, class T>
class const_mem_fun_t;

template <class Ret, class T>
class mem_fun_ref_t;

template <class Ret, class T>
class const_mem_fun_ref_t;

template <class Ret, class T, class Arg>
class mem_fun1_t;

template <class Ret, class T, class Arg>
class const_mem_fun1_t;

template <class Ret, class T, class Arg>
class mem_fun1_ref_t;

template <class Ret, class T, class Arg>
class const_mem_fun1_ref_t;

template <class Ret, class T>
mem_fun_t<Ret,T>
mem_fun(Ret (T::*)());

template <class Ret, class T>
const_mem_fun_t<Ret,T>
mem_fun(Ret (T::*)() const);

template <class Ret, class T>
mem_fun_ref_t<Ret,T>
mem_fun_ref(Ret (T::*)());

template <class Ret, class T>
const_mem_fun_ref_t<Ret,T>
mem_fun_ref(Ret (T::*)() const);

template <class Ret, class T, class Arg>
mem_fun1_t<Ret,T,Arg>
mem_fun(Ret (T::*)(Arg));

template <class Ret, class T, class Arg>
const_mem_fun1_t<Ret,T,Arg>
mem_fun(Ret (T::*)(Arg) const);

template <class Ret, class T, class Arg>
mem_fun1_ref_t<Ret,T,Arg>
mem_fun_ref(Ret (T::*)(Arg));

template <class Ret, class T, class Arg>
const_mem_fun1_ref_t<Ret,T,Arg>
mem_fun_ref(Ret (T::*)(Arg) const);

}

#endif	// __UTIL_STL_FUNCTIONAL_FWD_H__

