/**
	\file "util/reference_wrapper.h"
	Based on Boost's boost::reference_wrapper utility, by
	Jakko J\"arvi, Peter Dimov, Doug Gregor, and Dave Abrahams.  
	Included here to avoid external library dependence.  
	(Would rather have boost be standardized...)
	This version, however, is a little simplified.  

	This is generally useful with binding of functors.  
	Note that this eliminates the need for the the "util/binders.h"
	header, at least the reference/value binder defined therein.  
	$Id: reference_wrapper.h,v 1.2 2006/05/06 04:18:58 fang Exp $
 */

#ifndef	__UTIL_REFERENCE_WRAPPER_H__
#define	__UTIL_REFERENCE_WRAPPER_H__

namespace util {
//=============================================================================
// forward declarations, just for good measure

template<typename T>
class reference_wrapper;

template<typename T>
reference_wrapper<T>
ref(T&);

template<typename T>
reference_wrapper<T const>
cref(T const&);

template<typename T>
struct is_reference_wrapper;

template<typename T>
struct unwrap_reference;

//=============================================================================
/**
	Wraps around reference to be able to pass a reference like a value.  
 */
template<typename T> 
class reference_wrapper {
public:
	// types
	typedef T		type;
	typedef	T&		reference;
	typedef	T*		pointer;
private:
	pointer			ref;
public:
	// construct/copy/destruct
	explicit
	reference_wrapper(reference r) : ref(&r) { }

	// default copy-constructor
	// default destructor
	// default assignment? (.ref = .ref)

	operator reference () const { return *(this->ref); }

	reference
	get(void) const { return *(this->ref); }

	pointer
	get_pointer(void) const { return this->ref; }

};	// end struct reference_wrapper

//-----------------------------------------------------------------------------
// generators/constructors

template<typename T>
inline
reference_wrapper<T>
ref(T& t) {
	return reference_wrapper<T>(t);
}

template<typename T>
inline
reference_wrapper<T const>
cref(T const& t) {
	return reference_wrapper<T const>(t);
}

//-----------------------------------------------------------------------------
// NOTE: could use type_traits

template<typename T> 
struct is_reference_wrapper {	// : public false_value
	enum { value = false };
};

template<typename T> 
struct is_reference_wrapper<reference_wrapper<T> > {	// : public true_value
	enum { value = true };
};

//-----------------------------------------------------------------------------
template<typename T> 
struct unwrap_reference {
	typedef	T					type;
};

template<typename T> 
struct unwrap_reference<reference_wrapper<T> > {
	typedef	typename reference_wrapper<T>::type	type;
};

//=============================================================================
}	// end namespace util

#endif	// __UTIL_REFERENCE_WRAPPER_H__

