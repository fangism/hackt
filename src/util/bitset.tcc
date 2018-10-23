/**
	\file "util/bitset.tcc"
	$Id: bitset.tcc,v 1.3 2006/04/27 00:16:38 fang Exp $
 */

#ifndef	__UTIL_BITSET_TCC__
#define	__UTIL_BITSET_TCC__

#include <ostream>
#include <algorithm>
#include <functional>
#include "util/bitset.hh"
#if !defined(HAVE_STD_BITSET_FIND_FIRST) || !defined(HAVE_STD_BITSET_FIND_NEXT) || 1
#include "util/numeric/ctz.hh"
#endif
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
/**
	Start searching from ith word for the first set bit.
	\return bit position of set bit if found, else NB (max).
 */
template <size_t NB>
size_t
bitset<NB>::__find_next_from_whole_word(const size_t w) const {
	const const_word_iterator b(this->word_begin()), e(this->word_end());
	const const_word_iterator
		f(std::find_if(b +w, e,
			bind2nd(std::not_equal_to<word_type>(), word_type(0))));
	if (f != e) {
		// found a non-zero word
		return size_t(std::distance(b, f)) * bits_per_word
			+size_t(numeric::ctz(*f));
	} else {
		return NB;	// not found
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !defined(HAVE_STD_BITSET_FIND_FIRST)
template <size_t NB>
size_t
bitset<NB>::find_first(void) const {
	return __find_next_from_whole_word(0);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !defined(HAVE_STD_BITSET_FIND_NEXT)
/**
	Based on GLIBCXX _Base_bitset<>::_M_do_find_next()
 */
template <size_t NB>
size_t
bitset<NB>::find_next(const size_t __prev) const {
	size_t prev = __prev +1;	// inclusive bound
	if (prev >= num_words * bits_per_word) {
		return NB;	// not found
	}
	size_t i = __which_word(prev);	// starting word to search
	word_type w = this->__get_word(i);
	// mask off bits below bound
	w &= (~word_type(0) << __which_bit(prev));
	if (w != 0) {
		// next set bit is in same word
		return i * bits_per_word + numeric::ctz(w);
	}
	// check subsequent words
	return __find_next_from_whole_word(i+1);
}
#endif

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

