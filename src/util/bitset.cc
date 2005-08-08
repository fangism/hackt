/**
	\file "util/bitset.cc"
	$Id: bitset.cc,v 1.2 2005/08/08 16:51:13 fang Exp $
 */

#ifndef	__UTIL_BITSET_CC__
#define	__UTIL_BITSET_CC__

#include "util/bitset.h"
#include <ostream>

namespace util {
//=============================================================================
std::ostream&
print_bits_hex<uint64>::operator () (std::ostream& o, const uint64& t) const {
	// need another specialization for 64b int
	return o << reinterpret_cast<void*>(size_t(t)) << ' '
		<< reinterpret_cast<void*>(size_t(t >> 32));
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITSET_CC__

