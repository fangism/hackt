/**
	\file "util/bitset.cc"
	$Id: bitset.cc,v 1.6 2007/08/29 18:56:44 fang Exp $
 */

#ifndef	__UTIL_BITSET_CC__
#define	__UTIL_BITSET_CC__

#include "util/bitset.hh"
#include <ostream>
#include "util/numeric/ctz.hh"
// #include "util/numeric/ffs.hh"


// working around suse-linux g++-3.3's libstdc++ defects
namespace std {
#if defined(HAVE_STD_BITSET_FIND_FIRST)
#if !defined(HAVE_STD_BITSET_FIND_FIRST_IN_LIB)
/**
	This supplies a missing definition in the library.
	Since we want 0-indexed result, we invoke ctz() 
	instead of ffs(); ffs() -1 would also be correct.  
	Result is undefined if there are no set bits.  
 */
size_t
_Base_bitset<1>::_M_do_find_first(size_t __not_found) const {
	if (this->_M_w) {
		return util::numeric::ctz(this->_M_w);
	} else {
		return __not_found;
	}
}
#endif	// HAVE_STD_BITSET_FIND_FIRST_IN_LIB
#endif	// HAVE_STD_BITSET_FIND_FIRST
}	// end namespace std

namespace util {
//=============================================================================
#if SIZEOF_UINT64_T
/**
	Why not use iomanip's std::hex?
 */
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
#endif	// SIZEOF_UINT64_T

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITSET_CC__

