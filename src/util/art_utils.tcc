/**
	\file "art_utils.tcc"
	Template function definitions from "art_utils.h".
	$Id: art_utils.tcc,v 1.5 2004/11/02 07:52:11 fang Exp $
 */

#ifndef __ART_UTILS_TCC__
#define __ART_UTILS_TCC__

#include <iostream>
#include <algorithm>

#include "art_utils.h"

namespace util {
using namespace std;
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
inline
void    write_value(ostream& f, const T& v) {
	f.write((const char*) &v, sizeof(T));
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
inline
void    read_value(istream& f, T& v) {
	f.read((char*) &v, sizeof(T));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for writing sequence of binary values
	of a list (or any sequence) to output stream.  
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param S a sequence that has concepts: size, forward iterator.
	\param T a writable value.
	\param f the output stream.
	\param l the sequence of data values.
 */
template <template <class> class S, class T>
void
write_sequence(ostream& f, const S<T>& l) {
	f.write_value(f, l.size());
	for_each(l.begin(), l.end(), 
		bind1st(ptr_fun(write_value<T>), f)
		// will need to pass first argument by value-reference
	);
	// alternative, explicit for-loop
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for loading sequence of binary values
	of a list (or any sequence) from input stream, *IN-PLACE*.  
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param S a sequence that has concepts: size, forward iterator.
	\param T a writable value.
	\param f the input stream.
	\param l the sequence of data values to which to load values in-place.
 */
template <template <class> class S, class T>
void
read_sequence_in_place(istream& f, S<T>& l) {
	size_t size;
	read_value(f, size);
	for_each(l.begin(), l.end(), 
		bind1st(ptr_fun(read_value<T>), f)
		// will need to pass first argument by value-reference
	);
	// alternative, explicit for-loop
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic function for loading sequence of binary values
	of a list (or any sequence) from input stream, 
	appending to end of sequence.  
	For pointers, you will need a persistent object manager 
	for memory reconstruction: see "persistent_object_manager.h".
	\param S a sequence that has concepts: size, forward iterator.
	\param T a writable value.
	\param f the input stream.
	\param l the sequence of data values to which to append values.
 */
template <template <class> class S, class T>
void
read_sequence_back_insert(istream& f, S<T>& l) {
	size_t size;
	read_value(f, size);
	size_t i = 0;
	for ( ; i < size; i++) {
		T t;
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
read_key_value_pair(ostream& f, pair<K, T>& p) {
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
	\param K the map's key type.
	\param T the map's value type.
	\param m the map to write-out.
 */
template <template <class, class> class M, class K, class T>
void
write_map(ostream& f, const M<K,T>& m) {
	assert(f.good());
	write_value(f, m.size());
	for_each(m.begin(), m.end(), 
		bind1st(ptr_fun(write_key_value_pair), f)
	);
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
template <template <class, class> class M, class K, class T>
void
read_map(istream& f, const M<K,T>& m) {
	assert(f.good());
	size_t size;
	read_value(f, size);
	size_t i = 0;
	for( ; i < size; i++) {
		pair<K, T> p;
		read_key_value_pair(f, p);
		m[p.first] = p.second;
	}
}


//=============================================================================

}	// end namespace util

#endif	// __ART_UTILS_TCC__

