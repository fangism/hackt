// "binders.h"

/**
 *	This file contains specializations for binder adaptors
 *	base on the standard set found in <functional>.
 */

#ifndef	__BINDERS_H__
#define	__BINDERS_H__

#include <functional>

namespace std {

//=============================================================================
// adapters for binders, using value instead of reference
//=============================================================================
/**
	Essentially a copy operation.  
	Untested.
 */
template <class T>
struct Identity_argval : public unary_function<T, T> {
//	T operator () (T& x) const { return x; }
	T operator () (const T x) const { return x; }
};	// end class Identity_argval

//=============================================================================
/**
	Binder that fixes an argument by value, as opposed to by const
	reference in binder1st, after which this class is modeled.  
	Useful if an argument is already a reference, to avoid
	reference-of-reference.  
	Alternative is to use something like boost::call_traits, 
	which automatically treats a reference-to-reference as a reference.  
	Copy of a reference is just a reference, not a true copy.  
	One only needs to use the helper methods below. 
 */
template <class B>
class binder1st_argval :
	public unary_function<typename B::second_argument_type, 
				typename B::result_type> {
public:
	typedef	typename B::first_argument_type		arg1_type;
	typedef	typename B::second_argument_type	argument_type;
	typedef	typename B::result_type			result_type;
protected:
	B		op;
	arg1_type	arg1;
public:
	binder1st_argval(const B& b, const arg1_type v) : op(b), arg1(v) { }

	result_type
	operator() (const argument_type& x) const {
		return op(arg1, x);
	}
};	// end class binder1st_val

//-----------------------------------------------------------------------------
/**
	Helper function for binding 1st value when it is a const reference.  
	Passing a reference by value (if there is such a thing)
	is akin to just passing a reference.  
	\param op a binary operator.  
	\param v a const reference to the argument.  
 */
template <class B, class T>
inline
binder1st_argval<B>
bind1st_argval(const B& op, const T& v) {
	return binder1st_argval<B>(op, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for binding 1st value when it is a
	non-const reference.  
	This is useful when one wants to pass a modifiable visitor
	as a bound argument to a binary functor.  
	\param op a binary operator.  
	\param v a non-const reference to the argument.  
 */
template <class B, class T>
inline
binder1st_argval<B>
bind1st_argval(const B& op, T& v) {
	return binder1st_argval<B>(op, v);
}

//=============================================================================
/**
	Binder that fixes an argument by value, as opposed to by const
	reference in binder2nd, after which this class is modeled.  
	Useful if an argument is already a reference, to avoid
	reference-of-reference.  
	Alternative is to use something like boost::call_traits, 
	which automatically treats a reference-to-reference as a reference.  
	Copy of a reference is just a reference, not a true copy.  
	One only needs to use the helper methods below. 
 */
template <class B>
class binder2nd_argval :
	public unary_function<typename B::first_argument_type, 
				typename B::result_type> {
public:
	typedef	typename B::second_argument_type	arg2_type;
	typedef	typename B::first_argument_type		argument_type;
	typedef	typename B::result_type			result_type;
protected:
	B		op;
	arg2_type	arg2;
public:
	binder2nd_argval(const B& b, const arg2_type v) : op(b), arg2(v) { }

	result_type
	operator() (const argument_type& x) const {
		return op(x, arg2);
	}
};	// end class binder2nd_val

//-----------------------------------------------------------------------------
/**
	Helper function for binding 2nd value when it is a const reference.  
	Passing a reference by value (if there is such a thing)
	is akin to just passing a reference.  
	\param op a binary operator.  
	\param v a const reference to the argument.  
 */
template <class B, class T>
inline
binder2nd_argval<B>
bind2nd_argval(const B& op, const T& v) {
	return binder2nd_argval<B>(op, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for binding 2nd value when it is a
	non-const reference.  
	This is useful when one wants to pass a modifiable visitor
	as a bound argument to a binary functor.  
	\param op a binary operator.  
	\param v a non-const reference to the argument.  
 */
template <class B, class T>
inline
binder2nd_argval<B>
bind2nd_argval(const B& op, T& v) {
	return binder2nd_argval<B>(op, v);
}

//=============================================================================
}	// end namespace std

#endif	//	__BINDERS_H__

