/**
	\file "util/call_traits.hh"
	$Id: call_traits.hh,v 1.2 2006/05/06 04:18:57 fang Exp $
	Function parameter type wrappers, 
	Based on <boost/call_traits.hpp> by:

	(C) Copyright Steve Cleary, Beman Dawes, 
	Howard Hinnant & John Maddock 2000.
	Use, modification and distribution are subject to the 
	Boost Software License, Version 1.0. 
	(See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt).

	See http://www.boost.org/libs/utility for most recent version 
	including documentation.

	call_traits: defines typedefs for function usage
	(see libs/utility/call_traits.htm)

	Release notes:
	23rd July 2000:
	Fixed array specialization. (JM)
	Added Borland specific fixes for reference types
	(issue raised by Steve Cleary).
 */

#ifndef __UTIL_CALL_TRAITS_H__
#define __UTIL_CALL_TRAITS_H__

// #include <cstddef>
#if 0
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/detail/workaround.hpp>
#endif
#include "util/type_traits.hh"

namespace util {

namespace detail {

/**
	If large enough, pass by const reference.  
 */
template <typename T, bool small_>
struct ct_imp2 {
	typedef const T&	param_type;
};

/**
	If small enough, pass by value, likely to register-allocate.  
 */
template <typename T>
struct ct_imp2<T, true> {
	typedef const T		param_type;
};

/**
	General definition.  
 */
template <typename T, bool isp, bool b1>
struct ct_imp {
	typedef const T&	param_type;
};

/**
	Partial specialization: if type is arithmetic.
 */
template <typename T, bool isp>
struct ct_imp<T, isp, true> {
	typedef typename ct_imp2<T, sizeof(T) <= sizeof(void*)>::param_type
				param_type;
};

/**
	Partial specialization: if type is pointer.
 */
template <typename T, bool b1>
struct ct_imp<T, true, b1> {
	typedef T const		param_type;
};

}	// end namespace detail

template <typename T>
struct call_traits {
	public:
	typedef T			value_type;
	typedef T&			reference;
	typedef const T&		const_reference;

	// C++ Builder workaround: we should be able to define a compile time
	// constant and pass that as a single template parameter to 
	// ct_imp<T,bool>, however compiler bugs prevent this - 
	// instead pass three bool's to ct_imp<T,bool,bool,bool> 
	// and add an extra partial specialisation
	// of ct_imp to handle the logic. (JM)
	typedef	typename detail::ct_imp<T,
			is_pointer<T>::value,
			is_arithmetic<T>::value
		>::param_type		param_type;
};

template <typename T>
struct call_traits<T&>
{
	typedef	T&		value_type;
	typedef	T&		reference;
	typedef	const T&	const_reference;
	typedef	T&		param_type;
};

#if 0
// no array support for now
template <typename T, std::size_t N>
struct call_traits<T [N]>
{
private:
   typedef T array_type[N];
public:
   // degrades array to pointer:
   typedef const T* value_type;
   typedef array_type& reference;
   typedef const array_type& const_reference;
   typedef const T* const param_type;
};

template <typename T, std::size_t N>
struct call_traits<const T [N]>
{
private:
   typedef const T array_type[N];
public:
   // degrades array to pointer:
   typedef const T* value_type;
   typedef array_type& reference;
   typedef const array_type& const_reference;
   typedef const T* const param_type;
};
#endif

}	// end namespace util

#endif	// __UTIL_CALL_TRAITS_H__


