/**
	\file "util/guile_STL.h"
	Interfaces for translating back-and-forth between
	certain containers and scheme SCM types.  
	$Id: guile_STL.h,v 1.1 2007/03/13 04:04:40 fang Exp $
 */

#ifndef	__UTIL_GUILE_STL_H__
#define	__UTIL_GUILE_STL_H__

#if 0
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif
#endif

#include "util/libguile.h"
#include <functional>
#include <string>
#include <utility>
// #include <tr1/tuple>
#include <list>
#include <vector>
// maybe even include "gmpxx.h"
#include <iterator>
#include "util/boolean_types.h"
#include "util/stacktrace.h"		// for debugging

/**
	Define to 1 to force use of guile-1.8 API.
	The 1.6 API is missing many convenient interfaces.  
	We'll configure-wrap these later...
 */
#define	FORCE_GUILE_API_1_8			1
// alternative: FORCE_GUILE_API_1_6

namespace util {
namespace guile {
using std::string;
using std::pair;
using std::unary_function;
using std::list;

// TODO:
// goals of library: orthogonal design
// linear amount of code, exponential combinations
//=============================================================================
/**
	Template policy for converting from C++
	to SCM type, using intuitive conversions.  
	Implementation should be recursive.  
	Primary template is undefined, so we must supply the necessary
	partial specializations.  
	Rationale: for new non-STL types, user only need to provide
	single specialization for this to work.  
	Compound containers should "just work."
 */
template <class>
struct scm_builder;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Primary template for extracting data from SCM.  
	Allow throw?
 */
template <class>
struct scm_extractor;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function that uses template argument deduction to
	do the right thing.  
	TODO: supply alternative overload with caller/position argument, 
		for useful diagnostic messages.
	\param T is the type to convert to SCM
	\param t is the value/object to be converted
	\return the garbage-collected SCM object.  
 */
template <class T>
SCM
make_scm(const T& t) {
	// TODO: possible protection to prevent gc from reclaiming too early?
	return scm_builder<T>()(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for extracting SCM to C++ types.  
	We take the target type as a parameter to facilitate 
	template argument type deduction.  
	\param T is the type to convert from SCM.
	\param t is the value/object to be written.
		If T is a container, container should be initially empty.
	\return good if conversion was successful.
 */
template <class T>
good_bool
extract_scm(const SCM& s, T& t) {
	return scm_extractor<T>()(s, t);
}

//=============================================================================
// scm_builder specializations for fundamental types

// strings
template <>
struct scm_builder<string> : public unary_function<string, SCM> {
	SCM
	operator () (const argument_type& s) {
		STACKTRACE_VERBOSE;
#if FORCE_GUILE_API_1_8
		return scm_from_locale_string(s.c_str());
#else
		return scm_makfrom0str(s.c_str());
#endif
	}
};	// end struct scm_builder<string>

// TODO: finish me
template <>
struct scm_extractor<string>;

//-----------------------------------------------------------------------------
// numerical types

template <>
struct scm_builder<bool> : public unary_function<bool, SCM> {
	SCM
	operator () (const argument_type s) {
		return s ? SCM_BOOL_T : SCM_BOOL_F;
	}
};	// end struct scm_builder<bool>

// TODO: finish me
template <>
struct scm_extractor<bool> {
	good_bool
	operator () (const SCM& s, bool& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_bool(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<bool>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<char> : public unary_function<char, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8
		return scm_from_char(s);
#else
		return scm_char2num(s);
#endif
	}
};	// end struct scm_builder<char>

// TODO: finish me
template <>
struct scm_extractor<char> {
	good_bool
	operator () (const SCM& s, char& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_char(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<char>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<unsigned char> : public unary_function<unsigned char, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8
		return scm_from_uchar(s);
#else
		return scm_uchar2num(s);
#endif
	}
};	// end struct scm_builder<unsigned char>

// TODO: finish me
template <>
struct scm_extractor<unsigned char> {
	good_bool
	operator () (const SCM& s, unsigned char& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_uchar(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<unsigned char>

//-----------------------------------------------------------------------------
template <>
struct scm_builder<short> : public unary_function<short, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8
		return scm_from_short(s);
#else
		return scm_short2num(s);
#endif
	}
};	// end struct scm_builder<short>

// TODO: finish me
template <>
struct scm_extractor<short> {
	good_bool
	operator () (const SCM& s, short& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_short(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<short>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<unsigned short> : public unary_function<unsigned short, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8
		return scm_from_ushort(s);
#else
		return scm_ushort2num(s);
#endif
	}
};	// end struct scm_builder<unsigned short>

// TODO: finish me
template <>
struct scm_extractor<unsigned short> {
	good_bool
	operator () (const SCM& s, unsigned short& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_ushort(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<unsigned short>

//-----------------------------------------------------------------------------
template <>
struct scm_builder<int> : public unary_function<int, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8
		return scm_from_int(s);
#else
		return scm_int2num(s);
#endif
	}
};	// end struct scm_builder<int>

// TODO: finish me
template <>
struct scm_extractor<int> {
	good_bool
	operator () (const SCM& s, int& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_int(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<int>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<unsigned int> : public unary_function<unsigned int, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8
		return scm_from_uint(s);
#else
		return scm_uint2num(s);
#endif
	}
};	// end struct scm_builder<unsigned int>

// TODO: finish me
template <>
struct scm_extractor<unsigned int> {
	good_bool
	operator () (const SCM& s, unsigned int& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_uint(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<unsigned int>

//-----------------------------------------------------------------------------
template <>
struct scm_builder<unsigned long> : public unary_function<unsigned long, SCM> {
	SCM
	operator () (const argument_type& s) {
#if FORCE_GUILE_API_1_8
		return scm_from_ulong(s);
#else
		return scm_ulong2num(s);
#endif
	}
};	// end struct scm_builder<unsigned long>

// TODO: finish me
template <>
struct scm_extractor<unsigned long> {
	good_bool
	operator () (const SCM& s, unsigned long& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_ulong(s);	// got error handling?
		return good_bool(true);
#endif
	}
};	// end struct scm_extractor<unsigned long>

//-----------------------------------------------------------------------------
#ifdef	SIZEOF_LONG_LONG
template <>
struct scm_builder<unsigned long long> :
		public unary_function<unsigned long long, SCM> {
	SCM
	operator () (const argument_type& s) {
#if FORCE_GUILE_API_1_8
		return scm_from_ulong_long(s);
#else
		return scm_ulonglong2num(s);
#endif
	}
};	// end struct scm_builder<unsigned long>

// TODO: finish me
template <>
struct scm_extractor<unsigned long long> {
	good_bool
	operator () (const SCM& s, unsigned long long& i) {
#if FORCE_GUILE_API_1_8
		i = scm_to_ulong_long(s);	// got error handling?
		return good_bool(true);
#endif
	}
};
#endif	// SIZEOF_LONG_LONG

//=============================================================================
// scm_builder specializations for STL containers

// pairs
template <class F, class S>
struct scm_builder<pair<F, S> > : public unary_function<pair<F, S>, SCM> {
	typedef	scm_builder<pair<F, S> >		this_type;
	SCM
	operator () (const typename this_type::argument_type& a) {
		STACKTRACE_VERBOSE;
		return scm_cons(make_scm(a.first), make_scm(a.second));
	}
};	// end struct scm_builder<pair>

template <class F, class S>
struct scm_extractor<pair<F, S> >;

//-----------------------------------------------------------------------------
/**
	Stationary iterator that prepends using cons.
 */
struct scm_list_insert_iterator :
	public std::iterator<std::output_iterator_tag, void, void, void, void> {
	typedef	scm_list_insert_iterator	this_type;
	/**
		NOTE: this makes a COPY of the container
		This is over-written with each assignment.
	 */
	SCM					list;

	explicit
	scm_list_insert_iterator(const SCM& _l) : list(_l) { }

	/**
		Prepend operation, over-writing list <= (const head list)
	 */
	this_type&
	operator=(const SCM& head) {
		list = scm_cons(head, list);
		return *this;
	}

	/**
		Overload for any other non-SCM type.  
	 */
	template <class T>
	this_type&
	operator=(const T& head) {
		STACKTRACE_VERBOSE;
		list = scm_cons(make_scm(head), list);
		return *this;
	}

	/// Simply returns *this.
	this_type&
	operator*() { return *this; }

	/// Simply returns *this.  (This %iterator does not "move".)
	this_type&
	operator++() { return *this; }

	/// Simply returns *this.  (This %iterator does not "move".)
	this_type
	operator++(int) { return *this; }

};	// end struct list_insert_iterator

/// helper function
inline
scm_list_insert_iterator
scm_list_inserter(const SCM& l) {
	return scm_list_insert_iterator(l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	std::list specialization, for the love of STL.  
	TODO: will this work with template template parameter matching of 4.2?
	Should work now, with extra allocator template parameter.  
 */
template <class T, class A>
struct scm_builder<list<T, A> > : public unary_function<list<T, A>, SCM> {
	typedef	scm_builder<list<T, A> >		this_type;
	/**
		Construct backwards to preserve order.  
	 */
	SCM
	operator () (const typename this_type::argument_type& l) {
	//	typedef	typename list<T>::const_iterator	const_iterator;
		STACKTRACE_VERBOSE;
		// start with empty list and accumulate
		return (*copy(l.rbegin(), l.rend(),
			scm_list_inserter(SCM_LIST0))).list;
		// ignore old-cast expanded from SCM_PACK(x)
	}
};	// end struct scm_builder<list>

//-----------------------------------------------------------------------------
// tuples

//-----------------------------------------------------------------------------
// vector, array, valarray

//-----------------------------------------------------------------------------
// queues, stacks

//-----------------------------------------------------------------------------
// maps (associative)

//=============================================================================
}	// end namespace guile
}	// end namespace util

#endif	// __UTIL_GUILE_STL_H__

