/**
	\file "util/guile_STL.h"
	Interfaces for translating back-and-forth between
	certain containers and scheme SCM types.  
	$Id: guile_STL.h,v 1.6 2007/06/10 03:50:12 fang Exp $
 */

#ifndef	__UTIL_GUILE_STL_H__
#define	__UTIL_GUILE_STL_H__

#if 0
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif
#endif

#include "util/libguile.h"
#ifdef	HAVE_LIBGUILE_H
// include the rest of this file

#include <functional>
#include <string>
#include <utility>
// #include <tr1/tuple>
#include <list>
#include <vector>
#include <valarray>
// maybe even include "gmpxx.h"
#include <iterator>
#include "util/boolean_types.h"
#include "util/stacktrace.h"		// for debugging

/**
	Define to 1 to force use of guile-1.8 API.
	Would rather let configure detect per used function.  
	The 1.6 API is missing many convenient interfaces.  
	We'll configure-wrap these later...
 */
#define	FORCE_GUILE_API_1_8			0
/// Mutually exclusive with other FORCE_GUILE macros
#define FORCE_GUILE_API_1_6			0

namespace util {
namespace guile {
using std::string;
using std::pair;
using std::unary_function;
using std::list;
using std::valarray;

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
// scm_builder specializations for scm!

/**
	No transformation necessary!
 */
template <>
struct scm_builder<SCM> : public unary_function<SCM, SCM> {
	SCM
	operator () (const argument_type& s) {
		return s;
	}
};

template <>
struct scm_extractor<SCM> {
	good_bool
	operator () (const SCM& s, SCM& t) {
		t = s;
		return good_bool(true);
	}
};

//=============================================================================
// scm_builder specializations for fundamental types

// const char*
template <>
struct scm_builder<const char*> : public unary_function<const char*, SCM> {
	SCM
	operator () (const argument_type& s) {
		STACKTRACE_VERBOSE;
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_LOCALE_STRING)
		return scm_from_locale_string(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_MAKFROM0STR)
		return scm_makfrom0str(s);
#else
#error	"Missing SCM from const char* constructor."
#endif
	}
};	// end struct scm_builder<string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_extractor<const char*> {
	good_bool
	operator () (const SCM& s, const char*& t) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_LOCALE_STRING)
		t = scm_to_locale_string(s);
#else
		t = SCM_STRING_CHARS(s);
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<const char*>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 	Explicitly decay-array-to-pointer. 
 */
inline
SCM
make_scm(const char s[]) {
	return scm_builder<const char*>()(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// strings
template <>
struct scm_builder<string> : public unary_function<string, SCM> {
	SCM
	operator () (const argument_type& s) {
		STACKTRACE_VERBOSE;
		return make_scm(s.c_str());
	}
};	// end struct scm_builder<string>

/**
	May throw exception via guile's dynamic unwind mechanism.
 */
template <>
struct scm_extractor<string> {
	good_bool
	operator () (const SCM& s, string& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_LOCALE_STRING)
		i = scm_to_locale_string(s);	// got error handling?
#else	// unchecked by configure, 1.6 API?
		i = SCM_STRING_CHARS(s);
// #error	"Need substitute for scm_to_local_string."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<string>

//-----------------------------------------------------------------------------
// numerical types

template <>
struct scm_builder<bool> : public unary_function<bool, SCM> {
	SCM
	operator () (const argument_type s) {
		return s ? SCM_BOOL_T : SCM_BOOL_F;
	}
};	// end struct scm_builder<bool>

/**
	\pre scm argument 's' MUST be boolean!  
 */
template <>
struct scm_extractor<bool> {
	good_bool
	operator () (const SCM& s, bool& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_BOOL)
		i = scm_to_bool(s);	// got error handling?
#else	// unchecked, 1.6 API?
		i = SCM_NFALSEP(s);
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<bool>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<char> : public unary_function<char, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_CHAR)
		return scm_from_char(s);
// #elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_CHAR2NUM)
//		return scm_char2num(s);
#else
		return SCM_MAKE_CHAR(s);
#endif
	}
};	// end struct scm_builder<char>

template <>
struct scm_extractor<char> {
	good_bool
	operator () (const SCM& s, char& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_CHAR)
		i = scm_to_char(s);	// got error handling?
#else	// assume 1.6 API?
		i = SCM_CHAR(s);
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<char>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<unsigned char> : public unary_function<unsigned char, SCM> {
	SCM
	operator () (const argument_type s) {
// assume uchar and char interfaces are consistent
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_CHAR)
		return scm_from_uchar(s);
// #elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_CHAR2NUM)
//		return scm_uchar2num(s);
#else
		return SCM_MAKE_CHAR(s);	// no UCHAR macro
#endif
	}
};	// end struct scm_builder<unsigned char>

template <>
struct scm_extractor<unsigned char> {
	good_bool
	operator () (const SCM& s, unsigned char& i) {
// assume uchar and char interfaces are consistent
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_CHAR)
		i = scm_to_uchar(s);	// got error handling?
#else	// assume 1.6 API?
		i = SCM_CHAR(s);
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<unsigned char>

//-----------------------------------------------------------------------------
template <>
struct scm_builder<short> : public unary_function<short, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_SHORT)
		return scm_from_short(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_SHORT2NUM)
		return scm_short2num(s);
#else
#error	"Missing short to SCM constructor interface."
#endif
	}
};	// end struct scm_builder<short>

template <>
struct scm_extractor<short> {
	good_bool
	operator () (const SCM& s, short& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_SHORT)
		i = scm_to_short(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2SHORT)
		i = scm_num2short(s, 0, "unknown caller");
		// got error handling? -- expects caller position, fake it...
#else
#error	"Missing SCM to short converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<short>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<unsigned short> : public unary_function<unsigned short, SCM> {
	SCM
	operator () (const argument_type s) {
// assume unsigned operations comes with signed counterparts
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_SHORT)
		return scm_from_ushort(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_SHORT2NUM)
		return scm_ushort2num(s);
#else
#error	"Missing unsigned short to SCM constructor interface."
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
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2SHORT)
		i = scm_num2ushort(s, 0, "unknown caller");
		// got error handling? -- expects caller position, fake it...
#else
#error	"Missing SCM to short converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<unsigned short>

//-----------------------------------------------------------------------------
template <>
struct scm_builder<int> : public unary_function<int, SCM> {
	SCM
	operator () (const argument_type s) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_INT)
		return scm_from_int(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_INT2NUM)
		return scm_int2num(s);
#else
#error	"Missing int to SCM constructor."
#endif
	}
};	// end struct scm_builder<int>

// TODO: finish me
template <>
struct scm_extractor<int> {
	good_bool
	operator () (const SCM& s, int& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_INT)
		i = scm_to_int(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2INT)
		i = scm_num2int(s, 0, "unknown caller");
#else
#error	"Missing SCM to int converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<int>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<unsigned int> : public unary_function<unsigned int, SCM> {
	SCM
	operator () (const argument_type s) {
// assuming unsigned interface is consistent with signed
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_INT)
		return scm_from_uint(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_INT2NUM)
		return scm_uint2num(s);
#else
#error	"Missing unsigned int to SCM constructor."
#endif
	}
};	// end struct scm_builder<unsigned int>

template <>
struct scm_extractor<unsigned int> {
	good_bool
	operator () (const SCM& s, unsigned int& i) {
// assuming unsigned interface is consistent with signed
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_INT)
		i = scm_to_uint(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2INT)
		i = scm_num2uint(s, 0, "unknown caller");
#else
#error	"Missing SCM to unsigned int converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<unsigned int>

//-----------------------------------------------------------------------------
template <>
struct scm_builder<long> : public unary_function<long, SCM> {
	SCM
	operator () (const argument_type& s) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_LONG)
		return scm_from_long(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_LONG2NUM)
		return scm_long2num(s);
#else
#error	"Missing long to SCM constructor."
#endif
	}
};	// end struct scm_builder<long>

template <>
struct scm_extractor<long> {
	good_bool
	operator () (const SCM& s, long& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_LONG)
		i = scm_to_long(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2LONG)
		i = scm_num2long(s, 0, "unknown caller");
#else
#error	"Missing SCM to long converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<long>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct scm_builder<unsigned long> : public unary_function<unsigned long, SCM> {
	SCM
	operator () (const argument_type& s) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_LONG)
		return scm_from_ulong(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_LONG2NUM)
		return scm_ulong2num(s);
#else
#error	"Missing unsigned long to SCM constructor."
#endif
	}
};	// end struct scm_builder<unsigned long>

// TODO: finish me
template <>
struct scm_extractor<unsigned long> {
	good_bool
	operator () (const SCM& s, unsigned long& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_LONG)
		i = scm_to_ulong(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2LONG)
		i = scm_num2ulong(s, 0, "unknown caller");
#else
#error	"Missing SCM to long converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<unsigned long>

//-----------------------------------------------------------------------------
#if	defined(SIZEOF_LONG_LONG) && SIZEOF_LONG_LONG
template <>
struct scm_builder<long long> :
		public unary_function<long long, SCM> {
	SCM
	operator () (const argument_type& s) {
// assuming signed/unsigned interface is consistent
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_LONG_LONG)
		return scm_from_long_long(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_LONG_LONG2NUM)
		return scm_long_long2num(s);
#else
#error	"Missing long long to SCM constructor."
#endif
	}
};	// end struct scm_builder<long long>

template <>
struct scm_extractor<long long> {
	good_bool
	operator () (const SCM& s, long long& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_LONG_LONG)
		i = scm_to_long_long(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2LONG_LONG)
		i = scm_num2long_long(s, 0, "unknown caller");
#else
#error	"Missing SCM to long long converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<long long>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO: let's not worry about long long yet, as it's a GNU extension...

template <>
struct scm_builder<unsigned long long> :
		public unary_function<unsigned long long, SCM> {
	SCM
	operator () (const argument_type& s) {
// assuming signed/unsigned interface is consistent
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_LONG_LONG)
		return scm_from_ulong_long(s);
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_LONG_LONG2NUM)
		return scm_ulong_long2num(s);
#else
#error	"Missing unsigned long long to SCM constructor."
#endif
	}
};	// end struct scm_builder<unsigned long long>

template <>
struct scm_extractor<unsigned long long> {
	good_bool
	operator () (const SCM& s, unsigned long long& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_LONG_LONG)
		i = scm_to_ulong_long(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2LONG_LONG)
		i = scm_num2ulong_long(s, 0, "unknown caller");
#else
#error	"Missing SCM to unsigned long long converter."
#endif
		return good_bool(true);
	}
};	// end struct scm_extractor<unsigned long long>

#endif	// SIZEOF_LONG_LONG

//-----------------------------------------------------------------------------
#if	SIZEOF_FLOAT
/**
	guile-1.8 only uses double-precision, no single-precision interface.
 */
template <>
struct scm_builder<float> :
		public unary_function<float, SCM> {
	SCM
	operator () (const argument_type& s) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_DOUBLE)
		return scm_from_double(s);	// convert up to double
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_FLOAT2NUM)
		return scm_float2num(s);
#else
#error	"Missing float to SCM constructor."
#endif
	}
};	// end struct scm_builder<unsigned long>

/**
	guile-1.8 only uses double-precision, no single-precision interface.
 */
template <>
struct scm_extractor<float> {
	good_bool
	operator () (const SCM& s, float& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_DOUBLE)
		i = scm_to_double(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2FLOAT)
		i = scm_num2float(s, 0, "unknown caller");
#else
#error	"Missing SCM to float converter."
#endif
		return good_bool(true);
	}
};
#endif	// SIZEOF_FLOAT

//-----------------------------------------------------------------------------
#if	SIZEOF_DOUBLE
template <>
struct scm_builder<double> :
		public unary_function<double, SCM> {
	SCM
	operator () (const argument_type& s) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_FROM_DOUBLE)
		return scm_from_double(s);	// convert up to double
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_DOUBLE2NUM)
		return scm_double2num(s);
#else
#error	"Missing double to SCM constructor."
#endif
	}
};	// end struct scm_builder<unsigned long>

template <>
struct scm_extractor<double> {
	good_bool
	operator () (const SCM& s, double& i) {
#if FORCE_GUILE_API_1_8 || defined(HAVE_SCM_TO_DOUBLE)
		i = scm_to_double(s);	// got error handling?
#elif FORCE_GUILE_API_1_6 || defined(HAVE_SCM_NUM2DOUBLE)
		i = scm_num2double(s, 0, "unknown caller");
#else
#error	"Missing SCM to double converter."
#endif
		return good_bool(true);
	}
};
#endif	// SIZEOF_DOUBLE

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

	/**
		Default to constructing an empty list. 
		Warning: Ignore old-cast expanded from SCM_PACK(x)
	 */
	scm_list_insert_iterator() : list(SCM_LIST0) { }

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

/// overload that defaults to initializing with empty list
inline
scm_list_insert_iterator
scm_list_inserter(void) {
	return scm_list_insert_iterator();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	List construct from any container with a begin/end interface, 
	such as vector.  
 */
template <class L>
struct scm_list_builder : public unary_function<L, SCM> {
	typedef	scm_list_builder<L>		this_type;
	/**
		Start with empty list and accumulate.
		Construct backwards to preserve order.  
	 */
	SCM
	operator () (const typename this_type::argument_type& l) {
		STACKTRACE_VERBOSE;
		return (*copy(l.rbegin(), l.rend(),
			scm_list_inserter())).list;
	}
};	// end struct scm_list_builder

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor that constructs an SCM list.
	Specialization for valarray, damn pseudo-container...
 */
template <class V>
struct scm_list_builder<valarray<V> > :
		public unary_function<valarray<V>, SCM> {
	typedef	scm_list_builder<valarray<V> >		this_type;
	/**
		Same as container, but using indexing for begin/end.  
	 */
	SCM
	operator () (const typename this_type::argument_type& l) {
		STACKTRACE_VERBOSE;
		return (*std::reverse_copy(&l[0], &l[l.size()], 
			scm_list_inserter())).list;
	}
};	// end struct scm_list_builder<valarray>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// helper functions

/**
	Helper function to dispatch scm_list_builder functor, 
	using template argument deduction on type. 
 */
template <class L>
inline
SCM
make_scm_list(const L& l) {
	return scm_list_builder<L>()(l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	std::list specialization, for the love of STL.  
	TODO: will this work with template template parameter matching of 4.2?
	Should work now, with extra allocator template parameter.  
 */
template <class T, class A>
struct scm_builder<list<T, A> > : public scm_list_builder<list<T, A> > {
	typedef	scm_list_builder<list<T, A> >	parent_type;
	using parent_type::operator();
};	// end struct scm_builder<list>

//-----------------------------------------------------------------------------
// tuples

//-----------------------------------------------------------------------------
// vector, array, valarray

// scm_vector_builder
// make_scm_vector

//-----------------------------------------------------------------------------
// queues, stacks

//-----------------------------------------------------------------------------
// streams (ice-9 streams)

//-----------------------------------------------------------------------------
// maps (associative)

//=============================================================================
}	// end namespace guile
}	// end namespace util

#endif	// HAVE_LIBGUILE_H
// otherwise skip entire file
#endif	// __UTIL_GUILE_STL_H__

