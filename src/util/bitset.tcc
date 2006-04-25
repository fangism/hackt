/**
	\file "util/bitset.tcc"
	$Id: bitset.tcc,v 1.2.68.1 2006/04/25 18:24:46 fang Exp $
 */

#ifndef	__UTIL_BITSET_TCC__
#define	__UTIL_BITSET_TCC__

#include "util/bitset.h"
#include <ostream>
// #include <iomanip>

namespace util {
using std::ios_base;

//=============================================================================
/**
	Prints out hexadecimal like pointer.  
 */
template <class T>
std::ostream&
print_bits_hex<T>::operator () (std::ostream& o, const T& t) const {
	// need another specialization for 64b int
#if 1
	return o << reinterpret_cast<void*>(size_t(t));
#else
/**
	NOTE: this modifies the ios_base::basefield as a side-effect, 
		but this is only used for diagnostics, so we don't care. 
**/
	const ios_base::fmtflags save =
		o.setf(ios_base::hex | ios_base::showbase, 
			ios_base::basefield | ios_base::showbase);
	// o << std::setw(sizeof(size_t));
	// width only applies to next operation
	o << t;
	o.setf(save);
	return o;
#endif
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

