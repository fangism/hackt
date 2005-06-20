/**
	\file "util/IO_utils.tcc"
	Template function definitions from "IO_utils.h".
	$Id: IO_utils.tcc,v 1.9.12.1 2005/06/20 21:53:15 fang Exp $
 */

#ifndef __UTIL_IO_UTILS_TCC__
#define __UTIL_IO_UTILS_TCC__

#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <utility>		// for std::pair

// only needed for functional for_each where call_traits are needed.  
// #include "util/binders.h"

#include "util/IO_utils.h"

namespace util {
using std::ostream;
using std::istream;
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
	typedef S	sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	typedef	typename sequence_type::const_iterator	const_iterator;
	write_value(f, l.size());
#if 0
	for_each(l.begin(), l.end(), 
		bind1st_argval(ptr_fun(write_value<T>), f)
		// will need to pass first argument by value-reference
	);
#else
	// explicit for-loop
	const_iterator i = l.begin();
	const const_iterator e = l.end();
	for ( ; i!=e; i++)
		write_value<value_type>(f, *i);
#endif
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
	typedef	S	array_type;
	typedef	typename array_type::value_type	value_type;
	write_value(f, s.size());
	size_t i = 0;
	for ( ; i < s.size(); i++) {
		write_value<value_type>(f, s[i]);
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
	iterator i = l.begin();
	for ( ; j < size; j++, i++)
		read_value<value_type>(f, *i);
	// if sizes were asserted equal
	INVARIANT(i == l.end());
#endif
}

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
	typedef	S		sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	size_t size;
	read_value(f, size);
	l.resize(size);
	size_t j = 0;
	for ( ; j < size; j++) {
#if 0
		// doesn't work with vector<bool> because of specialized
		// implementation.
		read_value<value_type>(f, l[j]);
#else
		value_type temp;
		read_value<value_type>(f, temp);
		l[j] = temp;
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
	typedef S		sequence_type;
	typedef	typename sequence_type::value_type	value_type;
	size_t size;
	read_value(f, size);
	size_t i = 0;
	for ( ; i < size; i++) {
		value_type t;
		read_value(f, t);
		l.push_back(t);
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
write_key_value_pair(ostream& f, const pair<const K, T>& p) {
	write_value(f, p.first);
	write_value(f, p.second);
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
read_key_value_pair(istream& f, pair<K, T>& p) {
	read_value(f, p.first);
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
	const_iterator i = m.begin();
	const const_iterator e = m.end();
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

#endif	// __UTIL_IO_UTILS_TCC__

