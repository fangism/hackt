/**
 *	\file "ptrs_functional.h"
 *	This file contains specializations and extensions
 *	for member function adaptors
 *	base on the standard set found in <functional>.
 *	$Id: ptrs_functional.h,v 1.5.24.1 2005/02/06 08:33:39 fang Exp $
 */

#ifndef	__PTRS_FUNCTIONAL_H__
#define	__PTRS_FUNCTIONAL_H__

#include <functional>

namespace std {
//=============================================================================
// adapters for pointer-class member functors
//=============================================================================
/**
	Unary function adaptor, base on std::mem_fun_t.
	Type R is the return type, T is the class type.  
	Type P is the pointer-class.  
	The only requirement for class P is that indirection
	via * and -> are implemented.
 */
template <class R, template <class> class P, class T>
class mem_fun_p_t : public unary_function<P<T>, R> {
private:
	/** pointer to member function of T */
	R (T::*pmf) ();
public:
	mem_fun_p_t(R (T::*p)(), const P<T>& d) : pmf(p) { }
	R operator () (const P<T>& p) const { return (*p.*pmf)(); }
};	// end class mem_fun_p_t

//-----------------------------------------------------------------------------
/**
	Specialization for void return type.  
 */
template <template <class> class P, class T>
class mem_fun_p_t<void, P, T> : public unary_function<P<T>, void> {
private:
	/** pointer to member function of T */
	void (T::*pmf) ();
public:
	mem_fun_p_t(void (T::*p)()) : pmf(p) { }
	void operator () (const P<T>& p) const { (*p.*pmf)(); }
};	// end class specialization mem_fun_p_t

//-----------------------------------------------------------------------------
// helper wrapper function
template <class R, template <class> class P, class T>
inline
mem_fun_p_t<R, P, T>
mem_fun(R (T::*f)(), const P<T>& null) {
	// without pointer-class argument, constructor knows nothing about P!
	return mem_fun_p_t<R, P, T>(f);
}

//=============================================================================
/**
	Unary function adaptor, base on std::const_mem_fun_t.
	Type R is the return type, T is the class type.  
	Type P is the pointer-class.  
	The only requirement for class P is that indirection
	via * and -> are implemented.
	Consider: making methods depend somehow on P, 
		but then what would be parent unary_function?
 */
template <class R, template <class> class P, class T>
class const_mem_fun_p_t : public unary_function<P<T>, R> {
private:
	/** pointer to const member function of T */
	R (T::*pmf) () const;
public:
	// without pointer-class argument, constructor knows nothing about P!
explicit	const_mem_fun_p_t(R (T::*p)() const) : pmf(p) { }
	R operator () (const P<T>& p) const { return (*p.*pmf)(); }
};	// end class const_mem_fun_p_t

//-----------------------------------------------------------------------------
/**
	Specialization for void return type.  
 */
template <template <class> class P, class T>
class const_mem_fun_p_t<void, P, T> : public unary_function<P<T>, void> {
private:
	/** pointer to member function of T */
	void (T::*pmf) () const;
public:
explicit	const_mem_fun_p_t(void (T::*p)() const) : pmf(p) { }
	void operator () (const P<T>& p) const { (*p.*pmf)(); }
};	// end class specialization const_mem_fun_p_t

//-----------------------------------------------------------------------------
/**
	Helper wrapper function.  
	User should only have to call mem_fun for this to be invoked.
	Note that this is overloaded with 2 arguments, whereas the 
		original mem_fun only required one argument.  
	\param f is the pointer to the member function of T.
	\param nul is an ignored pointer whose sole purpose is to 
		identify the pointer class type of P.  Rather than having the
		user supply all three template arguments, this reduces the 
		inconvenience to adding one additional argument, 
		to allow all template arguments to be inferred.  
 */
template <class R, template <class> class P, class T>
inline
const_mem_fun_p_t<R, P, T>
mem_fun(R (T::*f)() const, const P<T>& null) {
	return const_mem_fun_p_t<R, P, T>(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// partial specializations of function template forbidden :S
// consider emulating with member templates...

//=============================================================================
/**
	Binary function adaptor, based on std::mem_fun1_t.
	class A is the argument type.  
 */
template <class R, template <class> class P, class T, class A>
class mem_fun1_p_t : public binary_function<P<T>, A, R> {
private:
	/** pointer to unary member function of T */
	R (T::*pmf) (A);
public:
explicit	mem_fun1_p_t(R (T::*p)(A)) : pmf(p) { }
	R operator () (const P<T>& p, A a) const { return (*p.*pmf)(a); }
};	// end class mem_fun1_p_t

//-----------------------------------------------------------------------------
/**
	Specialization for void return type.  
 */
template <template <class> class P, class T, class A>
class mem_fun1_p_t<void, P, T, A> : public binary_function<P<T>, A, void> {
private:
	/** pointer to member function of T */
	void (T::*pmf) (A);
public:
explicit	mem_fun1_p_t(void (T::*p)(A)) : pmf(p) { }
	void operator () (const P<T>& p, A a) const { (*p.*pmf)(a); }
};	// end class specialization mem_fun1_p_t

//-----------------------------------------------------------------------------
// helper wrapper function
template <class R, template <class> class P, class T, class A>
inline
mem_fun1_p_t<R, P, T, A>
mem_fun(R (T::*f)(A), const P<T>& null) {
	return mem_fun1_p_t<R, P, T, A>(f);
}

//=============================================================================
/**
	Binary function adaptor, based on std::const_mem_fun1_t.
	class A is the argument type.  
 */
template <class R, template <class> class P, class T, class A>
class const_mem_fun1_p_t : public binary_function<P<T>, A, R> {
private:
	/** pointer to unary member function of T */
	R (T::*pmf) (A) const;
public:
explicit	const_mem_fun1_p_t(R (T::*p)(A) const) : pmf(p) { }
	R operator () (const P<T>& p, A a) const { return (*p.*pmf)(a); }
};	// end class const_mem_fun1_p_t

//-----------------------------------------------------------------------------
/**
	Specialization for void return type.  
 */
template <template <class> class P, class T, class A>
class const_mem_fun1_p_t<void, P, T, A> :
		public binary_function<P<T>, A, void> {
private:
	/** pointer to member function of T */
	void (T::*pmf) (A) const;
public:
explicit	const_mem_fun1_p_t(void (T::*p)(A) const) : pmf(p) { }
	void operator () (const P<T>& p, A a) const { (*p.*pmf)(a); }
};	// end class specialization const_mem_fun1_p_t

//-----------------------------------------------------------------------------
// helper wrapper function
template <class R, template <class> class P, class T, class A>
inline
const_mem_fun1_p_t<R, P, T, A>
mem_fun(R (T::*f)(A) const, const P<T>& null) {
	return const_mem_fun1_p_t<R, P, T, A>(f);
}

//=============================================================================
}	// end namespace std

#endif	//	__PTRS_FUNCTIONAL_H__

