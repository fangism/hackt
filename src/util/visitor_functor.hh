/**
	\file "util/visitor_functor.hh"
	Convenient visitor functors that bind the visitor.  
	Why not use std::mem_fun, std::mem_fun_ref, and std::bind?
	$Id: visitor_functor.hh,v 1.3 2008/10/31 02:11:48 fang Exp $
 */

#ifndef	__UTIL_VISITOR_FUNCTOR_H__
#define	__UTIL_VISITOR_FUNCTOR_H__

// #include <functional>

namespace util {
//=============================================================================
/**
	Assumes that the type has an accept method that accepts
	the parameter type V.  
 */
template <class V>
struct visitor_ref_t {
	V&		_visitor;

	explicit
	visitor_ref_t(V& v) : _visitor(v) { }

	template <class T>
	void
	operator () (T& t) const {
		t.accept(_visitor);
	}
};	// end struct visitor_ref_t

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Statically typed version, for use with functional composition.
 */
template <class V, class T>
struct typed_visitor_ref_t : public visitor_ref_t<V> {
	typedef	visitor_ref_t<V>	base_type;
	typedef	T&			argument_type;
	typedef	void			result_type;

	explicit
	typed_visitor_ref_t(V& v) : base_type(v) { }

	void
	operator () (T& t) const {
		base_type::operator()(t);
	}
};

//-----------------------------------------------------------------------------
/**
	Helper functor generator.  
 */
template <class V>
inline
visitor_ref_t<V>
visitor_ref(V& v) {
	return visitor_ref_t<V>(v);
}

template <class V, class T>
inline
typed_visitor_ref_t<V, T>
typed_visitor_ref(V& v) {
	return typed_visitor_ref_t<V, T>(v);
}

//=============================================================================
/**
	Instead of iterating over references, this accepts pointers
	(and pointer classes) as arguments.  
 */
template <class V>
struct visitor_ptr_t {
	V&		_visitor;

	explicit
	visitor_ptr_t(V& v) : _visitor(v) { }

	template <class P>
	void
	operator () (P& p) const {
		if (p) p->accept(_visitor);
	}
};	// end struct visitor_ptr_t

//-----------------------------------------------------------------------------
/**
	Helper functor generator.  
 */
template <class V>
inline
visitor_ptr_t<V>
visitor_ptr(V& v) {
	return visitor_ptr_t<V>(v);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_VISITOR_FUNCTOR_H__

