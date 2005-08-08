/**
	\file "util/bitset.tcc"
	$Id: bitset.tcc,v 1.2 2005/08/08 16:51:14 fang Exp $
 */

#ifndef	__UTIL_BITSET_TCC__
#define	__UTIL_BITSET_TCC__

#include "util/bitset.h"
#include <ostream>

namespace util {
//=============================================================================
template <class T>
std::ostream&
print_bits_hex<T>::operator () (std::ostream& o, const T& t) const {
	// need another specialization for 64b int
	return o << reinterpret_cast<void*>(size_t(t));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t NB>
std::ostream&
print_bits_hex<std::bitset<NB> >::operator () (
		std::ostream& o, const arg_type& t) const {
	return o << t;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITSET_TCC__

