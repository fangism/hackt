/**
	\file "Object/common/lookup-bug.cc"
	Reduced test case triggering bug in g++-4.0.2.
	Simplified from Object/common/namespace.cc".
	$Id: lookup-bug.cc,v 1.1.2.1 2005/10/06 21:47:57 fang Exp $
 */

/**
	Original code uses refernce (1), 
	and removing reference (0) doesn't help.  
 */
#define	_REFERENCE_ARG		1

namespace std {

class ostream;

template <class _Arg, class _Result>
struct unary_function {
	typedef _Arg argument_type;
	typedef _Result result_type;
};

template <class _Arg1, class _Arg2, class _Result>
struct binary_function {
	typedef _Arg1 first_argument_type;
	typedef _Arg2 second_argument_type;
	typedef _Result result_type;
};

}

//=============================================================================
namespace util {
using std::binary_function;
using std::unary_function;

//=============================================================================
// adapters for pointer-class member functors
//=============================================================================
#if 0
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
#if _REFERENCE_ARG
mem_fun(R (T::*f)(), const P<T>& null)
#else
mem_fun(R (T::*f)(), const P<T>)
#endif
{
	// without pointer-class argument, constructor knows nothing about P!
	return mem_fun_p_t<R, P, T>(f);
}
#endif

//=============================================================================
#if 0
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
#if _REFERENCE_ARG
// NEED: P<const T>!!!
mem_fun(R (T::*f)() const, const P<T>& null)
#else
mem_fun(R (T::*f)() const, const P<T>)
#endif
{
	return const_mem_fun_p_t<R, P, T>(f);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// partial specializations of function template forbidden :S
// consider emulating with member templates...

//=============================================================================
#if 0
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
#if _REFERENCE_ARG
mem_fun(R (T::*f)(A), const P<T>& null)
#else
mem_fun(R (T::*f)(A), const P<T>)
#endif
{
	return mem_fun1_p_t<R, P, T, A>(f);
}
#endif

//=============================================================================
#if 1
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
// NOTE: need P<const T>, not P<T>!!!
#if _REFERENCE_ARG
mem_fun(R (T::*f)(A) const, const P<const T>& null)
#else
mem_fun(R (T::*f)(A) const, const P<const T>)
#endif
{
	return const_mem_fun1_p_t<R, P, T, A>(f);
}
#endif

//=============================================================================


namespace memory {
template <class T>
class never_ptr {
	T*	ptr;
};
}	// end namespace memory
}	// end namespace util

namespace foo {
using util::memory::never_ptr;
using std::ostream;

class name_space {
public:
	ostream&
	dump(ostream&) const;
};

class instance_collection_base {
public:
	ostream&
	pair_dump(ostream&) const;

	static const never_ptr<const instance_collection_base>	null;
	// static const never_ptr<instance_collection_base>	null;
};

using util::mem_fun;

//=============================================================================
// class name_space method definitions

/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
 */
ostream&
name_space::dump(ostream& o) const {
	// to canonicalize the dump, we bin and sort into maps
	mem_fun(&instance_collection_base::pair_dump, 
		instance_collection_base::null);
	return o;
}

//=============================================================================
}	// end namespace foo

