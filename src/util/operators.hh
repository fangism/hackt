/**
	\file "util/operators.hh"
	Functors but with virtual resolution.
	$Id: operators.hh,v 1.10 2006/07/16 03:34:57 fang Exp $
 */

#ifndef __UTIL_OPERATORS_H__
#define __UTIL_OPERATORS_H__

#include "config.h"
#include "util/likely.h"
#include "util/numeric/zerodiv_detect.hh"
#include <functional>
#include <stdexcept>
#include <cmath>

#define	USING_UTIL_OPERATIONS						\
	using util::binary_arithmetic_operation;			\
	using util::binary_relational_operation;			\
	using util::binary_logical_operation;				\
	using util::shift_operation;					\
	using util::plus;						\
	using util::minus;						\
	using util::multiplies;						\
	using util::divides;						\
	using util::modulus;						\
	using util::equal_to;						\
	using util::not_equal_to;					\
	using util::greater;						\
	using util::less;						\
	using util::greater_equal;					\
	using util::less_equal;						\
	using util::shift_left;						\
	using util::shift_right;					\
	using util::bitwise_and;					\
	using util::bitwise_or;						\
	using util::bitwise_xor;

//=============================================================================
namespace std {
/**
	modulus specialization for type float.
	This complete specialization allows us to perform floating point
	modulus operations, by forwarding to the standard library fmodf.  
 */
template <>
struct modulus<float>
#if __cplusplus < 201103L
: public std::binary_function<float,float,float>
#endif
{
	modulus() { }
	float
	operator () (const float& x, const float& y) const {
#if	defined(HAVE_FMODF)
		// preferred single-precision library function
		return fmodf(x, y);
#elif	defined(HAVE_FMOD)
		// fall back to double precision
		return fmod(x, y);
#else
#error	"I don't know what to do without fmod or fmodf."
#endif
	}
};	// end struct modulus

/**
	modulus specialization for type double.
	This complete specialization allows us to perform floating point
	modulus operations, by forwarding to the standard library fmod.  
 */
template <>
struct modulus<double>
#if __cplusplus < 201103L
: public std::binary_function<double,double,double>
#endif
{
	modulus() { }
	double
	operator () (const double& x, const double& y) const {
#if	defined(HAVE_FMOD)
		return fmod(x, y);
#else
#error	"I don't know what to do without fmod."
#endif
	}
};	// end struct modulus

}	// end namespace std
//=============================================================================

namespace util {

//=============================================================================
/**
	\param R integer-like class for the return type of 
		arithmetic operations.
	\param A integer-like class for which
		arithmetic operations are defined.
 */
template <class R = int, class A = R>
struct binary_arithmetic_operation
#if __cplusplus < 201103L
: public std::binary_function<A,A,R>
#endif
{
/**
	Required by gcc4 with -W.
	... even if we never delete subclass objects through these pointers.  
 */
virtual	~binary_arithmetic_operation() { }

/**
	Standard function interface for operators.  
 */
virtual	R operator () (const A& a, const A& b) const = 0;
virtual	bool is_associative(void) const = 0;
};	// end struct binary_arithmetic_operation

//-----------------------------------------------------------------------------
/**
	\param R bool-like class for the return type of 
		relational operations.
	\param A integer-like class for which 
		relational operations are defined.
 */
template <class R = bool, class A = R>
struct binary_relational_operation : public std::binary_function<A,A,R> {
/**
	Required by gcc4 with -W.
	... even if we never delete subclass objects through these pointers.  
 */
virtual	~binary_relational_operation() { }

/**
	Standard function interface for operators.  
 */
virtual	R operator () (const A& a, const A& b) const = 0;
};	// end struct binary_relational_operation

//-----------------------------------------------------------------------------
/**
	\param R bool-like class for the return type of 
		logical operations.
	\param A bool-like class for which 
		logical operations are defined.
 */
template <class R = bool, class A = R>
struct binary_logical_operation : public std::binary_function<A,A,R> {
/**
	Required by gcc4 with -W.
	... even if we never delete subclass objects through these pointers.  
 */
virtual	~binary_logical_operation() { }

/**
	Standard function interface for operators.  
 */
virtual	R operator () (const A& a, const A& b) const = 0;
};	// end struct binary_logical_operation

//-----------------------------------------------------------------------------
/**
	Typedef	template.  
	\param R integer-like class for the return type of 
		shift operations.
	\param A integer-like class for which
		shift operations are defined, also used for the shift amount.
 */
template <class R, class A>
struct shift_operation {
	typedef	binary_arithmetic_operation<R,A>	type;
};	// end struct shift_operation

//-----------------------------------------------------------------------------
/**
	Typedef template for bitwise operations.  
	\param R integer-like class for the return type of 
		shift operations.
	\param A integer-like class for which
		shift operations are defined, also used for the shift amount.
 */
template <class R, class A>
struct bitwise_operation {
	typedef	binary_arithmetic_operation<R,A>	type;
};	// end struct bitwise_operation

//=============================================================================
/**
	Wrapper for std::plus binary functor.
 */
template <class R = int, class A = R>
struct plus : public binary_arithmetic_operation<R, A> {
	plus() { }
	R operator () (const A& a, const A& b) const
		{ return std::plus<A>()(a,b); }
	bool is_associative(void) const { return true; }
};	// end struct plus

//-----------------------------------------------------------------------------
/**
	Wrapper for std::minus binary functor.
 */
template <class R = int, class A = R>
struct minus : public binary_arithmetic_operation<R, A> {
	minus() { }
	R operator () (const A& a, const A& b) const
		{ return std::minus<A>()(a,b); }
	bool is_associative(void) const { return false; }
};	// end struct minus

//-----------------------------------------------------------------------------
/**
	Wrapper for std::multiplies binary functor.
 */
template <class R = int, class A = R>
struct multiplies : public binary_arithmetic_operation<R, A> {
	multiplies() { }
	R operator () (const A& a, const A& b) const
		{ return std::multiplies<A>()(a,b); }
	bool is_associative(void) const { return true; }
};	// end struct multiplies

//-----------------------------------------------------------------------------
/**
	Wrapper for std::divides binary functor.
 */
template <class R = int, class A = R>
struct divides : public binary_arithmetic_operation<R, A> {
	divides() { }

	/**
		\throw domain error if divide by zero.  
	 */
	R
	operator () (const A& a, const A& b) const {
		if (UNLIKELY(numeric::zerodiv_detect(b))) {
			throw std::domain_error("Detected divide by 0!");
		}
		return std::divides<A>()(a,b);
	}
	bool is_associative(void) const { return false; }
};	// end struct divides

//-----------------------------------------------------------------------------
/**
	Wrapper for std::modulus binary functor.
 */
template <class R = int, class A = R>
struct modulus : public binary_arithmetic_operation<R, A> {
	modulus() { }

	/**
		\throw domain error if divide by zero.  
	 */
	R
	operator () (const A& a, const A& b) const {
		if (UNLIKELY(numeric::zerodiv_detect(b))) {
			throw std::domain_error("Detected divide by 0!");
		}
		return std::modulus<A>()(a,b);
	}
	bool is_associative(void) const { return false; }
};	// end struct modulus

//=============================================================================
/**
	Wrapper for std::equal_to binary functor.
 */
template <class R = bool, class A = int>
struct equal_to : public binary_relational_operation<R, A> {
	equal_to() { }
	R operator () (const A& a, const A& b) const
		{ return std::equal_to<A>()(a,b); }
};	// end struct equal_to

//-----------------------------------------------------------------------------
/**
	Wrapper for std::not_equal_to binary functor.
 */
template <class R = bool, class A = int>
struct not_equal_to : public binary_relational_operation<R, A> {
	not_equal_to() { }
	R operator () (const A& a, const A& b) const
		{ return std::not_equal_to<A>()(a,b); }
};	// end struct not_equal_to

//-----------------------------------------------------------------------------
/**
	Wrapper for std::greater binary functor.
 */
template <class R = bool, class A = int>
struct greater : public binary_relational_operation<R, A> {
	greater() { }
	R operator () (const A& a, const A& b) const
		{ return std::greater<A>()(a,b); }
};	// end struct greater

//-----------------------------------------------------------------------------
/**
	Wrapper for std::less binary functor.
 */
template <class R = bool, class A = int>
struct less : public binary_relational_operation<R, A> {
	less() { }
	R operator () (const A& a, const A& b) const
		{ return std::less<A>()(a,b); }
};	// end struct less

//-----------------------------------------------------------------------------
/**
	Wrapper for std::greater_equal binary functor.
 */
template <class R = bool, class A = int>
struct greater_equal : public binary_relational_operation<R, A> {
	greater_equal() { }
	R operator () (const A& a, const A& b) const
		{ return std::greater_equal<A>()(a,b); }
};	// end struct greater_equal

//-----------------------------------------------------------------------------
/**
	Wrapper for std::less_equal binary functor.
 */
template <class R = bool, class A = int>
struct less_equal : public binary_relational_operation<R, A> {
	less_equal() { }
	R operator () (const A& a, const A& b) const
		{ return std::less_equal<A>()(a,b); }
};	// end struct less_equal

//=============================================================================
/**
	Wrapper for std::logical_and binary functor.
 */
template <class R = bool, class A = bool>
struct logical_and : public binary_logical_operation<R, A> {
	logical_and() { }
	R operator () (const A& a, const A& b) const
		{ return std::logical_and<A>()(a,b); }
	bool is_associative(void) const { return true; }
};	// end struct logical_and

//-----------------------------------------------------------------------------
/**
	Wrapper for std::logical_or binary functor.
	Not short circuit OR.  
 */
template <class R = bool, class A = bool>
struct logical_or : public binary_logical_operation<R, A> {
	logical_or() { }
	R operator () (const A& a, const A& b) const
		{ return std::logical_or<A>()(a,b); }
	bool is_associative(void) const { return true; }
};	// end struct logical_or

//-----------------------------------------------------------------------------
/**
	There is no std::logical_xor.
 */
template <class R = bool, class A = bool>
struct logical_xor : public binary_logical_operation<R, A> {
	logical_xor() { }
	R operator () (const A& a, const A& b) const
		{ return a ^ b; }
	bool is_associative(void) const { return true; }
};	// end struct logical_xor

//-----------------------------------------------------------------------------
/**
	There is no std::logical_xnor.
 */
template <class R = bool, class A = bool>
struct logical_xnor : public binary_logical_operation<R, A> {
	logical_xnor() { }
	R operator () (const A& a, const A& b) const
		{ return !(a ^ b); }
	bool is_associative(void) const { return true; }
};	// end struct logical_xor

//=============================================================================
/**
	There is no std::shift_left.
 */
template <class R = int, class A = int>
struct shift_left : public shift_operation<R, A>::type {
	shift_left() { }
	R operator () (const A& a, const A& s) const
		{ return a << s; }
	bool is_associative(void) const { return false; }
};	// end struct shift_left

//-----------------------------------------------------------------------------
/**
	There is no std::shift_right.
	Note: to distinguish between arithmetic and logical shift, 
	specify an unsigned type.  
 */
template <class R = int, class A = int>
struct shift_right : public shift_operation<R, A>::type {
	shift_right() { }
	R operator () (const A& a, const A& s) const
		{ return a >> s; }
	bool is_associative(void) const { return false; }
};	// end struct shift_right

//=============================================================================
/**
	Wrapper for std::logical_and binary functor.
 */
template <class R, class A = R>
struct bitwise_and : public bitwise_operation<R, A>::type {
	bitwise_and() { }
	R operator () (const A& a, const A& b) const
		{ return a & b; }
	bool is_associative(void) const { return true; }
};	// end struct bitwise_and

//-----------------------------------------------------------------------------
/**
	Wrapper for std::bitwise_or binary functor.
	Not short circuit OR.  
 */
template <class R, class A = R>
struct bitwise_or : public bitwise_operation<R, A>::type {
	bitwise_or() { }
	R operator () (const A& a, const A& b) const
		{ return a | b; }
	bool is_associative(void) const { return true; }
};	// end struct bitwise_or

//-----------------------------------------------------------------------------
/**
	There is no std::bitwise_xor.
 */
template <class R, class A = R>
struct bitwise_xor : public bitwise_operation<R, A>::type {
	bitwise_xor() { }
	R operator () (const A& a, const A& b) const
		{ return a ^ b; }
	bool is_associative(void) const { return true; }
};	// end struct bitwise_xor

//=============================================================================
}	// end namespace util

#endif	// __UTIL_OPERATORS_H__

