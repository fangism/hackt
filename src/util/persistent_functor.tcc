/**
	\file "util/persistent_functor.tcc"
	This is a terrible name for a file...
	$Id: persistent_functor.tcc,v 1.6 2008/10/05 23:00:38 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_FUNCTOR_TCC__
#define	__UTIL_PERSISTENT_FUNCTOR_TCC__

#include <iostream>
#include <functional>
#include "util/persistent_functor.h"
#include "util/macros.h"
#include "util/size_t.h"
#include "util/IO_utils.h"
// #include "util/persistent_object_manager.h"

namespace util {
using std::ostream;
using std::istream;
using std::for_each;

//=============================================================================
template <class T>
void
write_persistent_sequence(const persistent_object_manager& m, 
		ostream& o, const T& t) {
	typedef	typename T::value_type		value_type;
	const size_t s = t.size();
	write_value(o, s);
	for_each(t.begin(), t.end(), persistent_writer_ref(m, o));
}

//-----------------------------------------------------------------------------
/**
	Suitable for containers that lack begin/end interface, like valarray.
 */
template <class T>
void
write_persistent_array(const persistent_object_manager& m, 
		ostream& o, const T& t) {
	typedef	typename T::value_type		value_type;
	const size_t s = t.size();
	write_value(o, s);
	size_t i = 0;
	for ( ; i<s; ++i) {
		persistent_writer_ref(m, o)(t[i]);
	}
}

//=============================================================================
/**
	Reading in-place uses a modifying iterator to visit
	already allocated positions.  
	\pre there are at least as many positions as needed.
 */
template <class T>
void
read_persistent_sequence_in_place(const persistent_object_manager& m, 
		istream& i, T& t) {
	size_t s = 0;
	read_value(i, s);
	INVARIANT(t.size() >= s);
	typedef	typename T::value_type		value_type;
	typedef	typename T::iterator		iterator;
	iterator j(t.begin());
	size_t k = 0;
	for ( ; k < s; ++k, ++j) {
		persistent_loader_ref(m, i)(*j);
	}
}

//-----------------------------------------------------------------------------
#if 0
/**
	NOTE: this is appropriate pretty much for an uninitialized
	valarray.  
	No one in his right mind would make a valarray of a persistent type.  
 */
template <class T>
read_persistent_sequence_prealloc(const persistent_object_manager& m, 
		istream& i, T& t) {
	size_t s = 0;
	read_value(i, s);
	INVARIANT(t.size() >= s);
	typedef	typename T::value_type		value_type;
	typedef	value_type*			iterator;
	iterator j(&t[0]);
	size_t k = 0;
	for ( ; k < s; ++k, ++j) {
		persistent_loader_ref(m, i)(*j);
	}
}
#endif

//-----------------------------------------------------------------------------
/**
	For vector-like loading, resizing first, then loading in-place.  
	Reading in-place uses a modifying iterator to visit
	already allocated positions.  
	\pre there are at least as many positions as needed.
 */
template <class T>
void
read_persistent_sequence_resize(const persistent_object_manager& m, 
		istream& i, T& t) {
	typedef	typename T::value_type		value_type;
	size_t s;
	read_value(i, s);
	t.resize(s);
	size_t k = 0;
	for ( ; k < s; ++k) {
		persistent_loader_ref(m, i)(t[k]);
	}
}

//-----------------------------------------------------------------------------
/**
	Creates a default element at the back, and then loads object in place.  
 */
template <class T>
void
read_persistent_sequence_back_insert(const persistent_object_manager& m, 
		istream& i, T& t) {
	typedef	typename T::value_type		value_type;
	size_t s;
	read_value(i, s);
	size_t k = 0;
	for ( ; k < s; ++k) {
		value_type v;
		t.push_back(v);
		persistent_loader_ref(m, i)(t.back());
	}
}

//-----------------------------------------------------------------------------
/**
	Inserts elements into a set.  
 */
template <class T>
void
read_persistent_set_insert(const persistent_object_manager& m, 
		istream& i, T& t) {
	typedef	typename T::value_type		value_type;
	size_t s;
	read_value(i, s);
	size_t k = 0;
	for ( ; k < s; ++k) {
		value_type v;
		persistent_loader_ref(m, i)(v);
		t.insert(v);
	}
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_PERSISTENT_FUNCTOR_TCC__

