/**
	\file "util/IO_utils.tcc"
	Template function definitions from "IO_utils.hh".
	Consider renaming this file to value_read/writer...
	$Id: IO_utils.tcc,v 1.19 2009/08/28 20:45:22 fang Exp $
 */

#ifndef __UTIL_IO_UTILS_TCC__
#define __UTIL_IO_UTILS_TCC__

// optional predefine: STACKTRACE_IOS
// to enable debugging for a particular translation unit

#ifndef	STACKTRACE_IOS
#define	STACKTRACE_IOS			0
#endif

#include "util/IO_utils.hh"

#ifndef	EXTERN_TEMPLATE_UTIL_IO_UTILS

#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>		// for std::pair
#include <vector>		// for vector<bool>
#include "util/macros.h"	// for INVARIANT

#if	STACKTRACE_IOS
#include "util/stacktrace.hh"
	#define	STACKTRACE_IO(x)	STACKTRACE(x)
	#define	STACKTRACE_IO_BRIEF	STACKTRACE_BRIEF
	#define	STACKTRACE_IO_VERBOSE	STACKTRACE_VERBOSE
	#define	STACKTRACE_IO_PRINT(x)	STACKTRACE_INDENT_PRINT(x)
#else
	#define	STACKTRACE_IO(x)
	#define	STACKTRACE_IO_BRIEF
	#define	STACKTRACE_IO_VERBOSE
	#define	STACKTRACE_IO_PRINT(x)
#endif

namespace util {
#if STACKTRACE_IOS
#include "util/using_ostream.hh"
#endif
using std::ostream;
using std::istream;
using std::vector;
//=============================================================================
// automatic template instantiation suppression

#if 0
// breaks -pedantic
// these are instantiated in "IO_utils.cc"
extern template void write_value(ostream&, const char&);
extern template void read_value(istream&, char&);
#endif

//=============================================================================
/**
	Default implementation for binary object writing.  
	TODO: convert ALL invocations of read/write_value to 
	the functor for proper specialization.  
 */
template <class T>
void
value_writer<T>::operator () (const T& t) {
	write_value(os, t);
}

//-----------------------------------------------------------------------------
/**
	Default implementation for binary object reading.
 */
template <class T>
void
value_reader<T>::operator () (T& t) {
	read_value(is, t);
}

//=============================================================================
/**
	Generic function for writing binary values of plain-old-data (POD)
	to output stream.
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param f the output stream.
	\param v the data value.
 */
template <class T>
// inline
void
write_value(ostream& f, const T& v) {
	STACKTRACE_IO_VERBOSE;
	f.write(reinterpret_cast<const char*>(&v), sizeof(T));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for writing binary values of plain-old-data (POD)
	to output stream.
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param f the output stream.
	\param v the data value.
 */
template <class T>
// inline
void
read_value(istream& f, T& v) {
	STACKTRACE_IO_VERBOSE;
	f.read(reinterpret_cast<char*>(&v), sizeof(T));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for writing sequence of binary values
	of a list (or any sequence) to output stream.  
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param S a sequence that has concepts: size, forward iterator.
	\param f the output stream.
	\param l the sequence of data values.
 */
template <class S>
void
write_sequence(ostream& f, const S& l) {
	STACKTRACE_IO_VERBOSE;
	typedef S	sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	typedef	typename sequence_type::const_iterator	const_iterator;
	write_value(f, l.size());
	STACKTRACE_IO_PRINT("sequence.tellp = " << f.tellp() << endl);
	STACKTRACE_IO_PRINT("sequence.size = " << l.size() << endl);
#if 0
	for_each(l.begin(), l.end(), 
		bind1st_argval(ptr_fun(write_value<T>), f)
		// will need to pass first argument by value-reference
	);
#else
	// explicit for-loop
	const_iterator i(l.begin());
	const const_iterator e(l.end());
	value_writer<value_type> w(f);
	for ( ; i!=e; i++)
		w(*i);
		// write_value<value_type>(f, *i);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes an iterator range.
	Useful if size is taken care of elsewhere.
	\param f the output stream
	\param i the begin iterator
	\param e the end iterator
 */
template <class Iter>
void
write_range(ostream& f, Iter i, const Iter e) {
	typedef	typename std::iterator_traits<Iter>::value_type	value_type;
	value_writer<value_type> w(f);
	while (i!=e) {
		w(*i);
		++i;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for writing value arrays.  
	Use this on classes that do not satisfy the iterator concept
	(such as valarray) nor have a begin() or end() method, 
	but only provide random access, array-like, using operator [].
	Won't work on raw arrays because they have no size() method.  
	This is complemented by read_sequence_resize.  
	\param S a sequence that meets array concept requirements.  
	\param f the output stream.
	\param s the sequence of data values.
 */
template <class S>
void
write_array(ostream& f, const S& s) {
	STACKTRACE_IO_VERBOSE;
	typedef	S	array_type;
	typedef	typename array_type::value_type	value_type;
	write_value(f, s.size());
	size_t i = 0;
	value_writer<value_type> w(f);
	for ( ; i < s.size(); i++) {
		w(s[i]);
		// write_value<value_type>(f, s[i]);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Read a sequence of values in-place over a pre-sized range.  
	\param f input stream
	\param i begin iterator
	\param e end iterator
 */
template <class Iter>
void
read_range(istream& f, Iter i, const Iter e) {
	typedef	typename std::iterator_traits<Iter>::value_type	value_type;
	value_reader<value_type> r(f);
	while (i!=e) {
		r(*i);
		++i;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for loading sequence of binary values
	of a list (or any sequence) from input stream, *IN-PLACE*.  
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param S a sequence that has concepts: size, forward iterator.
	\param f the input stream.
	\param l the sequence of data values to which to load values in-place.
 */
template <class S>
void
read_sequence_in_place(istream& f, S& l) {
	STACKTRACE_IO_VERBOSE;
	typedef S	sequence_type;
	typedef	typename sequence_type::iterator	iterator;
	typedef	typename sequence_type::value_type	value_type;
	size_t size;
	read_value(f, size);
	INVARIANT(l.size() == size);	// or >= ?
#if 0
	for_each(l.begin(), l.end(), 
		bind1st_argval(ptr_fun(read_value<T>), f)
		// will need to pass first argument by value-reference
	);
#else
	// alternative, explicit for-loop
	size_t j = 0;
	iterator i(l.begin());
	value_reader<value_type> r(f);
	for ( ; j < size; j++, i++)
		r(*i);
		// read_value<value_type>(f, *i);
	// if sizes were asserted equal
	INVARIANT(i == l.end());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses placement construction to pre-allocate a structure
	with enough entries first, rather than calling resize().  
	This is particularly useful and specialized for valarray.  
	(In fact MUST be valarray because of the assumptions about
	the pointer to value_type being equivalent to the iterator type.)
	This complements write_sequence.  
	\pre sequence l has not already been initialized!
		This is potentially dangerous if attempting to
		overwrite a sequence without calling it's dtor first.  
		To be safe, we destroy first.  
 */
template <class S>
void
read_sequence_prealloc(istream& f, S& l) {
	STACKTRACE_IO_VERBOSE;
	typedef S	sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	typedef	value_type*				iterator;
	size_t size;
	read_value(f, size);
	INVARIANT(!l.size());
	l.~sequence_type();
	// placement construction is safe if this has not been initialized
	new (&l) sequence_type(size);
	size_t j = 0;
	iterator i(&l[0]);
	value_reader<value_type> r(f);
	for ( ; j < size; j++, i++)
		r(*i);
		// read_value<value_type>(f, *i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace detail {
	template <class S>
	struct read_sequence_resize_impl {
		/// since valarray doesn't have reference...
		typedef	typename S::value_type&		reference;

		template <class R>
		void
		operator () (R reader, reference ref) const {
			reader(ref);
		}
	};

	template <class Alloc>
	struct read_sequence_resize_impl<vector<bool, Alloc> > {
		typedef	typename vector<bool, Alloc>::value_type
						value_type;
		/// This is bit_reference
		typedef	typename vector<bool, Alloc>::reference
						reference;

		template <class R>
		void
		operator () (R reader, reference ref) const {
			value_type temp;
			reader(temp);
			ref = temp;
		}
	};
}	// end namespace detail

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant of sequence reading resizes the container argument
	with the read size, and proceeds to read in values in-places.  
	Requires random access instead of iteration.  
	Works on vectors and valarrays.  
	This complements write_array().
	\param S a sequence that has concepts: size, forward iterator.
	\param f the input stream.
	\param l the sequence of data values to which to load values in-place.
 */
template <class S>
void
read_sequence_resize(istream& f, S& l) {
	STACKTRACE_IO_VERBOSE;
	typedef	S		sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	size_t size;
	read_value(f, size);
	STACKTRACE_IO_PRINT("sequence.tellg = " << f.tellg() << endl);
	STACKTRACE_IO_PRINT("sequence.size = " << size << endl);
	l.resize(size);
	size_t j = 0;
	value_reader<value_type> r(f);
	for ( ; j < size; j++) {
#if 0
		// doesn't work with vector<bool> because of specialized
		// implementation.
		read_value<value_type>(f, l[j]);
#else
		detail::read_sequence_resize_impl<S>()(r, l[j]);
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for loading sequence of binary values
	of a list (or any sequence) from input stream, 
	appending to end of sequence.  
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param S a sequence that has concepts: size, forward iterator.
	\param f the input stream.
	\param l the sequence of data values to which to append values.
 */
template <class S>
void
read_sequence_back_insert(istream& f, S& l) {
	STACKTRACE_IO_VERBOSE;
	typedef S		sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	size_t size;
	read_value(f, size);
	size_t i = 0;
	value_reader<value_type> r(f);
	for ( ; i < size; i++) {
		value_type t;
		r(t);
		l.push_back(t);
	}
	// any way to std::copy directly?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for loading sequence of values
	of a set from input stream, inserting into set.
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param S a sequence that has concepts: size.
	\param f the input stream.
	\param l the set of data values to which to insert values.
 */
template <class S>
void
read_sequence_set_insert(istream& f, S& l) {
	STACKTRACE_IO_VERBOSE;
	typedef S		sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	size_t size;
	read_value(f, size);
	size_t i = 0;
	value_reader<value_type> r(f);
	for ( ; i < size; i++) {
		value_type t;
		r(t);
		l.insert(t);
	}
	// any way to std::copy directly?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic helper function for writing key-value pairs from maps
	to binary output stream.  
	\param K the key type.
	\param T the value type.
	\param p the pair referenced in a map.  
 */
template <class K, class T>
void
write_pair(ostream& f, const pair<K, T>& p) {
	STACKTRACE_IO_VERBOSE;
	write_value(f, p.first);
	STACKTRACE_IO_PRINT("pair.tellp = " << f.tellp() << endl);
	write_value(f, p.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic helper function for writing key-value pairs from maps
	to binary output stream.  
	Const-ness of K?
	\param K the key type.
	\param T the value type.
	\param p the pair referenced in a map.  
 */
template <class K, class T>
void
read_pair(istream& f, pair<K, T>& p) {
	STACKTRACE_IO_VERBOSE;
	read_value(f, p.first);
	STACKTRACE_IO_PRINT("pair.tellg = " << f.tellg() << endl);
	read_value(f, p.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic template function for writing sequence of map
	key-value pairs to binary output.  
	Map or map-like container requires concepts: forward iterator, 
		size, copy-constructible elements.
	Writes out values one pair at a time.
	\param M the map type.
	\param m the map to write-out.
 */
template <class M>
void
write_map(ostream& f, const M& m) {
	typedef	M	map_type;
	typedef	typename map_type::key_type	key_type;
	typedef	typename map_type::mapped_type	mapped_type;
	typedef	typename map_type::const_iterator	const_iterator;
	INVARIANT(f.good());
	write_value(f, m.size());
#if 0
	for_each(m.begin(), m.end(), 
		bind1st_argval(ptr_fun(write_key_value_pair<K,T>), f)
	);
#else
	const_iterator i(m.begin());
	const const_iterator e(m.end());
	for ( ; i!=e; i++)
		write_key_value_pair<key_type,mapped_type>(f, *i);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic template function for reading sequence of map
	key-value pairs from binary input.  
	Map or map-like container requires concepts: forward iterator, 
		size, copy-constructible elements.
	Load in values one pair at a time.
	\param M the map type.
	\param K the map's key type.
	\param T the map's value type.
	\param m the map to read-in.
 */
template <class M>
void
read_map(istream& f, M& m) {
	typedef	M	map_type;
	typedef	typename map_type::key_type	key_type;
	typedef	typename map_type::mapped_type	mapped_type;
	typedef	typename map_type::const_iterator	const_iterator;
	typedef	pair<key_type, mapped_type>	pair_type;
	// NOTE: pair_type removed const on key_type
	INVARIANT(f.good());
	// INVARIANT(m.empty()); // ?
	size_t size;
	read_value(f, size);
	size_t i = 0;
	for( ; i < size; i++) {
		pair_type p;
		read_key_value_pair<key_type,mapped_type>(f, p);
		m[p.first] = p.second;
	}
}

//=============================================================================

}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_IO_UTILS
#endif	// __UTIL_IO_UTILS_TCC__

