/**
	\file "operators.h'
	Functors but with virtual resolution.
	$Id: operators.h,v 1.3 2004/12/05 05:07:25 fang Exp $
 */

#ifndef __UTIL_OPERATORS_H__
#define __UTIL_OPERATORS_H__

#include <functional>

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
	using util::shift_right;

namespace util {
// using namespace std;

//=============================================================================
/**
	\param R integer-like class for the return type of 
		arithmetic operations.
	\param A integer-like class for which
		arithmetic operations are defined.
 */
template <class R = int, class A = int>
struct binary_arithmetic_operation {
virtual	R operator () (const A& a, const A& b) const = 0;
};	// end struct binary_arithmetic_operation

//-----------------------------------------------------------------------------
/**
	\param R bool-like class for the return type of 
		relational operations.
	\param A integer-like class for which 
		relational operations are defined.
 */
template <class R = bool, class A = int>
struct binary_relational_operation {
virtual	R operator () (const A& a, const A& b) const = 0;
};	// end struct binary_relational_operation

//-----------------------------------------------------------------------------
/**
	\param R bool-like class for the return type of 
		logical operations.
	\param A bool-like class for which 
		logical operations are defined.
 */
template <class R = bool, class A = bool>
struct binary_logical_operation {
virtual	R operator () (const A& a, const A& b) const = 0;
};	// end struct binary_logical_operation

//-----------------------------------------------------------------------------
/**
	\param R integer-like class for the return type of 
		shift operations.
	\param A integer-like class for which
		shift operations are defined.
	\param S integer-like class for which
		shift operations are defined (shift amount).
 */
template <class R = int, class A = int, class S = int>
struct shift_operation {
virtual	R operator () (const A& a, const S& s) const = 0;
};	// end struct_operation

//=============================================================================
/**
	Wrapper for std::plus binary functor.
 */
template <class R = int, class A = int>
struct plus : binary_arithmetic_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::plus<A>()(a,b); }
};	// end struct plus

//-----------------------------------------------------------------------------
/**
	Wrapper for std::minus binary functor.
 */
template <class R = int, class A = int>
struct minus : public binary_arithmetic_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::minus<A>()(a,b); }
};	// end struct minus

//-----------------------------------------------------------------------------
/**
	Wrapper for std::multiplies binary functor.
 */
template <class R = int, class A = int>
struct multiplies : public binary_arithmetic_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::multiplies<A>()(a,b); }
};	// end struct multiplies

//-----------------------------------------------------------------------------
/**
	Wrapper for std::divides binary functor.
 */
template <class R = int, class A = int>
struct divides : public binary_arithmetic_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::divides<A>()(a,b); }
};	// end struct divides

//-----------------------------------------------------------------------------
/**
	Wrapper for std::modulus binary functor.
 */
template <class R = int, class A = int>
struct modulus : public binary_arithmetic_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::modulus<A>()(a,b); }
};	// end struct modulus

//=============================================================================
/**
	Wrapper for std::equal_to binary functor.
 */
template <class R = bool, class A = int>
struct equal_to : public binary_relational_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::equal_to<A>()(a,b); }
};	// end struct equal_to

//-----------------------------------------------------------------------------
/**
	Wrapper for std::not_equal_to binary functor.
 */
template <class R = bool, class A = int>
struct not_equal_to : public binary_relational_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::not_equal_to<A>()(a,b); }
};	// end struct not_equal_to

//-----------------------------------------------------------------------------
/**
	Wrapper for std::greater binary functor.
 */
template <class R = bool, class A = int>
struct greater : public binary_relational_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::greater<A>()(a,b); }
};	// end struct greater

//-----------------------------------------------------------------------------
/**
	Wrapper for std::less binary functor.
 */
template <class R = bool, class A = int>
struct less : public binary_relational_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::less<A>()(a,b); }
};	// end struct less

//-----------------------------------------------------------------------------
/**
	Wrapper for std::greater_equal binary functor.
 */
template <class R = bool, class A = int>
struct greater_equal : public binary_relational_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::greater_equal<A>()(a,b); }
};	// end struct greater_equal

//-----------------------------------------------------------------------------
/**
	Wrapper for std::less_equal binary functor.
 */
template <class R = bool, class A = int>
struct less_equal : public binary_relational_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::less_equal<A>()(a,b); }
};	// end struct less_equal

//=============================================================================
/**
	Wrapper for std::logical_and binary functor.
 */
template <class R = bool, class A = bool>
struct logical_and : public binary_logical_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::logical_and<A>()(a,b); }
};	// end struct logical_and

//-----------------------------------------------------------------------------
/**
	Wrapper for std::logical_or binary functor.
 */
template <class R = bool, class A = bool>
struct logical_or : public binary_logical_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return std::logical_or<A>()(a,b); }
};	// end struct logical_or

//-----------------------------------------------------------------------------
/**
	There is no std::logical_xor.
 */
template <class R = bool, class A = bool>
struct logical_xor : public binary_logical_operation<R, A> {
	R operator () (const A& a, const A& b) const
		{ return a ^ b; }
};	// end struct logical_xor

//=============================================================================
/**
	There is no std::shift_left.
 */
template <class R = int, class A = int, class S = int>
struct shift_left : public shift_operation<R, A, S> {
	R operator () (const A& a, const S& s) const
		{ return a << s; }
};	// end struct shift_left

//-----------------------------------------------------------------------------
/**
	There is no std::shift_right.
	Note: to distinguish between arithmetic and logical shift, 
	specify an unsigned type.  
 */
template <class R = int, class A = int, class S = int>
struct shift_right : public shift_operation<R, A, S> {
	R operator () (const A& a, const S& s) const
		{ return a >> s; }
};	// end struct shift_right

//=============================================================================
}	// end namespace util

#endif	// __UTIL_OPERATORS_H__

