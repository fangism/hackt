// "multikey.h"
// multidimensional key class

#ifndef	__MULTIKEY_H__
#define	__MULTIKEY_H__

#include <assert.h>
#include <iostream>

#ifndef	USE_STL_ALGORITHM
#define	USE_STL_ALGORITHM		1
#endif

#if USE_STL_ALGORITHM
#include <algorithm>
#endif

#include "multikey_fwd.h"

/***
	Later be able to compare between keys of different dimensions.  
***/

namespace MULTIKEY_NAMESPACE {
using namespace std;

//=============================================================================
/**
	Just a wrapper class for a fixed-size array.  
	Useful for emulating a multidimensional map using a flat map
	with a multidimensional key.  
	Saves the author from the trouble of writing a multidimensional
	iterator.  
	We choose to use a plain static array, because we don't need the 
	overhead from vector or valarray.  
	Perhaps add another field for default value?
 */
template <size_t D, class K, K init>
class multikey {
	template <size_t, class C, C>
	friend class multikey;
public:
	K indices[D];

public:
	/**
		Default constructor.  
		Requires key type K to be assignable and copiable.  
	 */
	multikey(const K i = init) { fill(indices, &indices[D], i); }

	/**
		Copy construtor compatible with other dimensions.  
		If this is larger than argument, fill remaining
		dimensions with 0.  
	 */
	template <size_t D2, K init2>
	multikey(const multikey<D2,K,init2>& k, const K i = init) {
// depends on <algorithm>
		if (D <= D2) {
			copy(k.indices, &k.indices[D], indices);
		} else {
			copy(k.indices, &k.indices[D2], indices);
			fill(&indices[D2], &indices[D], i);
		}
	}

	/**
		Safe indexing with array-bound check.  
		indices is public, so one can always access it directly...
	 */
	K& operator [] (const size_t i) {
		assert(i < D);
		return indices[i];
	}

	/**
		Const version of array indexing.  
	 */
	const K& operator [] (const size_t i) const {
		assert(i < D);
		return indices[i];
	}
};	// end class multikey

//-----------------------------------------------------------------------------
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k) {
	register size_t i = 0;
	for ( ; i<D; i++)
		o << '[' << k.indices[i] << ']';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator < (const multikey<D,K>& l, const multikey<D,K>& r) {
#if USE_STL_ALGORITHM
	return lexicographical_compare(&l.indices[0], &l.indices[D],
		&r.indices[0], &r.indices[D]);
#else
	register size_t i = 0;
	for ( ; i<D; i++) {
		register const size_t x = l.indices[i], y = r.indices[i];
		if (x != y)
			return (x < y);
	}
	// else all equal
	return false;
#endif	// USE_STL_ALGORITHM
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator < (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return lexicographical_compare(&l.indices[0], &l.indices[D1],
		&r.indices[0], &r.indices[D2]);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator > (const multikey<D,K>& l, const multikey<D,K>& r) {
#if USE_STL_ALGORITHM
	return lexicographical_compare(&r.indices[0], &r.indices[D],
		&l.indices[0], &l.indices[D]);
#else
	register size_t i = 0;
	for ( ; i<D; i++) {
		register const size_t x = l.indices[i], y = r.indices[i];
		if (x != y)
			return (x > y);
	}
	// else all equal
	return false;
#endif
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator > (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return lexicographical_compare(&r.indices[0], &r.indices[D2],
		&l.indices[0], &l.indices[D1]);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator == (const multikey<D,K>& l, const multikey<D,K>& r) {
#if USE_STL_ALGORITHM
	return equals(&l.indices[0], &l.indices[D], &r.indices[0]);
#else
	register size_t i = 0;
	for ( ; i<D; i++) {
		register const size_t x = l.indices[i], y = r.indices[i];
		if (!equal(x,y))
			return false;
	}
	// else all equal
	return true;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator != (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l == r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator <= (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l > r);
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator <= (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return !(l > r);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator >= (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l < r);
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator >= (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return !(l < r);
}
#endif

//-----------------------------------------------------------------------------

}	// end namespace MULTIKEY_NAMESPACE

//=============================================================================


#endif	//	__MULTIKEY_H__

