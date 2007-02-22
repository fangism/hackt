/**
	\file "util/bitset.cc"
	$Id: bitset.cc,v 1.5 2007/02/22 22:27:39 fang Exp $
 */

#ifndef	__UTIL_BITSET_CC__
#define	__UTIL_BITSET_CC__

#include "util/bitset.h"
#include <ostream>


// working around suse-linux g++-3.3's libstdc++ defects
namespace std {
#if defined(HAVE_STD_BITSET_FIND_FIRST)
#if !defined(HAVE_STD_BITSET_FIND_FIRST_IN_LIB)
size_t
_Base_bitset<1>::_M_do_find_first(size_t __not_found) const {
	if (this->_M_w)
#if defined(HAVE_BUILTIN_CTZL)
		return __builtin_ctzl(this->_M_w);
#elif defined(HAVE_BUILTIN_CTZ)
		return __builtin_ctz(this->_M_w);
#elif defined(HAVE_BUILTIN_FFSL)
		return __builtin_ffsl(this->_M_w) -1;
#elif defined(HAVE_BUILTIN_FFS)
		return __builtin_ffs(this->_M_w) -1;
#else
#error	"TODO: use util::numeric lookup tables to compute"
#endif
	else
		return __not_found;
}
#endif
#endif

}

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

