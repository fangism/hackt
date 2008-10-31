/**
	\file "util/index_functor.h"
	Define function object that takes index, returns element, 
	using operator [].
	$Id: index_functor.h,v 1.1 2008/10/31 02:11:47 fang Exp $
 */

#ifndef	__UTIL_INDEX_FUNCTOR_H__
#define	__UTIL_INDEX_FUNCTOR_H__

#include <functional>	// for unary_function

namespace util {
//=============================================================================
/**
	Binds the base of an index expression, using reference (modifiable).
	Basically bind1st(ptr_fun(T::operator[]))
 */
template <class A>
class index_functor_t : public std::unary_function<
		typename A::size_type, 
		typename A::reference> {
public:
	typedef	A				base_type;
	// might need type_traits if A is a natural array type or pointer
	typedef	typename base_type::reference	reference;
	typedef	typename base_type::size_type	size_type;
	typedef	size_type			argument_type;
	typedef	reference			result_type;
protected:
	base_type&				base;

public:
	explicit
	index_functor_t(base_type& a) : base(a) { }

	result_type
	operator () (const argument_type i) const {
		return base[i];
		// return base.operator[](i);
	}
};	// end class index_functor_t

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Binds the base of an index expression, using const-reference.
	Basically bind1st(ptr_fun(T::operator[]))
 */
template <class A>
class index_functor_const_t : public std::unary_function<
		typename A::size_type, 
		typename A::reference> {
public:
	typedef	A				base_type;
	// might need type_traits if A is a natural array type or pointer
	typedef	typename base_type::const_reference	const_reference;
	typedef	typename base_type::size_type	size_type;
	typedef	size_type			argument_type;
	typedef	const_reference			result_type;

protected:
	const base_type&			base;

public:
	explicit
	index_functor_const_t(const base_type& a) : base(a) { }

	result_type
	operator () (const argument_type i) const {
		return base[i];
		// return base.operator[](i);
	}
};	// end class index_functor_t

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class A>
inline
index_functor_t<A>
index_functor(A& a) {
	return index_functor_t<A>(a);
}

template <class A>
inline
index_functor_const_t<A>
index_functor(const A& a) {
	return index_functor_const_t<A>(a);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_INDEX_FUNCTOR_H__

