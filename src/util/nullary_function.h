/**
	\file "util/nullary_function.h"
	Generic generator functor base.
	$Id: nullary_function.h,v 1.3 2005/05/10 04:51:28 fang Exp $
 */

#ifndef	__UTIL_NULLARY_FUNCTION_H__
#define	__UTIL_NULLARY_FUNCTION_H__

#include "util/nullary_function_fwd.h"

namespace util {
//=============================================================================
struct nullary_function_virtual_parent_tag { };
struct nullary_function_nonvirtual_parent_tag { };

//=============================================================================
/**
	Template for functor that takes no arguments.  
	Oddly enough this isn't in <bits/stl_function.h>.
	(Willing to bet it's in Boost.)
 */
template <class _Result>
struct nullary_function {
	/// the return type
	typedef	_Result		result_type;
};	// end struct nullary_function

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pure virtual base version.  
 */
template <class _Result>
struct nullary_function_virtual {
	/// the return type
	typedef	_Result		result_type;

virtual	~nullary_function_virtual() { }

virtual	result_type
	operator () (void) const = 0;
};	// end struct nullary_function

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Result, class Tag>
struct nullary_function_parent_select;

template <class Result>
struct nullary_function_parent_select<Result, nullary_function_nonvirtual_parent_tag> {
	typedef	nullary_function<Result>		type;
};

template <class Result>
struct nullary_function_parent_select<Result, nullary_function_virtual_parent_tag> {
	typedef	nullary_function_virtual<Result>	type;
};

//-----------------------------------------------------------------------------
/**
	Functor for operation with no arguments, and with some
	return value.  
 */
template <class _Result, class Tag>
class pointer_to_nullary_function :
	public nullary_function_parent_select<_Result, Tag>::type {
protected:
	typedef typename nullary_function_parent_select<_Result, Tag>::type
							parent_type;
protected:
	_Result (*nf)(void);
public:
	pointer_to_nullary_function() : nf(NULL) { }

	explicit
	pointer_to_nullary_function(_Result (*f)(void)) : nf(f) { }

	~pointer_to_nullary_function() { }

	/**
		If parent is virtual, this will override parent's.  
	 */
	_Result
	operator () (void) const { return nf(); }

};	// end class pointer_to_nullary_function

//-----------------------------------------------------------------------------
/**
	Binds a unary function to a nullary function.  
	This *could* go in "binders.h"...
 */
template <class Op, class Tag>
class binder_null :
	public nullary_function_parent_select<typename Op::result_type, Tag> {
public:
	typedef	typename Op::result_type	result_type;
protected:
	typedef typename nullary_function_parent_select<result_type, Tag>::type
						parent_type;
	typedef	typename Op::argument_type	argument_type;
protected:
	Op		op;
	argument_type	value;
public:
	binder_null(const Op& f, const argument_type& x) : op(f), value(x) { }
	~binder_null() { }

	result_type
	operator () (void) const { return op(value); }

};	// end class binder_null

//-----------------------------------------------------------------------------
/**
	Helper inline function for binding unary to nullary function.  
 */
template <class Op, class T, class Tag>
inline
binder_null<Op>
bind_null(const Op& f, const T& x) {
	typedef	typename Op::argument_type	arg_type;
	return binder_null<Op, Tag>(f, arg_type(x));
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_NULLARY_FUNCTION_H__

