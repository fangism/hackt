/**
	\file "util/bitset.cc"
	$Id: bitset.cc,v 1.4 2007/02/22 01:09:14 fang Exp $
 */

#ifndef	__UTIL_BITSET_CC__
#define	__UTIL_BITSET_CC__

#include "util/bitset.h"
#include <ostream>

namespace util {
//=============================================================================
#if SIZEOF_UINT64_T
std::ostream&
print_bits_hex<uint64>::operator () (std::ostream& o, const uint64& t) const {
	// need another specialization for 64b int
#if (SIZEOF_SIZE_T == SIZEOF_VOIDP)
	return o << reinterpret_cast<void*>(size_t(t));
#elif (SIZEOF_SIZE_T == SIZEOF_VOIDP *2)
	return o << reinterpret_cast<void*>(size_t(t)) << ' '
		<< reinterpret_cast<void*>(size_t(t >> 32));
#else
#error	"What is sizeof(void*) vs. sizeof(size_t)?"
#endif
}
#endif

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITSET_CC__

