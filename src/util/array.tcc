/**
	\file "util/array.tcc"
	$Id: array.tcc,v 1.1 2010/04/19 02:46:10 fang Exp $
 */

#ifndef	__UTIL_ARRAY_TCC__
#define	__UTIL_ARRAY_TCC__

#include "util/array.hh"
#include <iostream>
#include <iterator>
#include <algorithm>

namespace util {
using std::ostream;
using std::ostream_iterator;
using std::istream;

//=============================================================================
// class array method definitions

/**
	Initialize-fill.
 */
template <class T, size_t S>
array<T,S>::array(const T& t) {
	std::fill(this->_value, this->_value +S, t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Safe copy-construction from statically sized array of same type.
 */
template <class T, size_t S>
array<T,S>::array(const T t[S]) {
	std::copy(t, t+S, this->_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T, size_t S>
ostream&
operator << (ostream& o, const array<T,S>& a) {
	typedef	typename array<T,S>::const_iterator	const_iterator;
	const_iterator j(a.begin());
	const const_iterator l(a.end() -1);
	copy(j, l, ostream_iterator<T>(o, ", "));
	return o << a.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T, size_t S>
istream&
operator >> (istream& i, array<T,S>& a) {
	typedef	typename array<T,S>::iterator	iterator;
	iterator j(a.begin());
	const iterator e(a.end());
	do {
		i >> *j;
		++j;
	} while (j != e);
	return i;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ARRAY_TCC__

