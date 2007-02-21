/**
	\file "util/bitset.h"
	Extended bitset.
	Also contains wrapper functors for other bit-operations.  
	$Id: bitset.h,v 1.5 2007/02/21 17:00:26 fang Exp $
 */

#ifndef	__UTIL_BITSET_H__
#define	__UTIL_BITSET_H__

#include <iosfwd>
#include "config.h"		// detect bitset facilities (SGI extensions)
#include <bitset>
#include "util/inttypes.h"

namespace util {
//=============================================================================
/**
	bitset with extended interface and functionality.
	TODO: add arithmetic, and relational operations.  
	TODO: add efficient built-in bit manipulation operations.  
	TODO: add unchecked bit-reference operations.  
 */
template <size_t NB>
class bitset : public std::bitset<NB> {
public:
	typedef	std::bitset<NB>		parent_type;
	/**
		Most likely unsigned long.  
	 */
//	typedef	typename parent_type::_WordT	word_type;
	typedef	size_t			word_type;

	bitset() : parent_type() { }

	bitset(const parent_type& p) : parent_type(p) { }

	// implicit allowed
	bitset(const word_type __val) : parent_type(__val) { }

	template <class CharT, class Traits, class Alloc>
	explicit
	bitset(const std::basic_string<CharT,Traits,Alloc>& __s, 
		const size_t __pos = 0) : parent_type(__s, __pos) { }

	template <class CharT, class Traits, class Alloc>
	explicit
	bitset(const std::basic_string<CharT,Traits,Alloc>& __s, 
		const size_t __pos, const size_t __n) : 
		parent_type(__s, __pos, __n) { }

	bool
	all(void) const {
		// return this->count() == this->size();
		return this->to_ulong() == ~(-1UL << NB);
	}

	// TODO: test for SGI extensions and work-around
	size_t
	find_first() const {
#if defined(HAVE_STD_BITSET_FIND_FIRST)
		return parent_type::_Find_first();
#else
#error "TODO: write our own find_first(), using __builtin_clzl()."
#endif
	}

	// TODO: test for SGI extensions and work-around
	size_t
	find_next(size_t __prev ) const {
#if defined(HAVE_STD_BITSET_FIND_NEXT)
		return parent_type::_Find_next(__prev);
#else
#error "TODO: write our own find_next()."
#endif
	}


};	// end class bitset

//=============================================================================
/**
	Compare != zero.
	Same functionality as std::bitset::any()
 */
template <class T>
struct any_bits {
	bool
	operator () (const T& t) const { return t; }
};	// end struct any_bits

/**
	Specialized comparison against 0 for std::bitset.
 */
template <size_t NB>
struct any_bits<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	bool
	operator () (const arg_type& t) const { return t.any(); }
};	// end struct any_bits

/**
	Specialization for util::bitset.
 */
template <size_t NB>
struct any_bits<bitset<NB> > {
	typedef	bitset<NB>		arg_type;

	bool
	operator () (const arg_type& t) const { return t.any(); }
};	// end struct any_bits

//-----------------------------------------------------------------------------
/**
	Compare != -1.
 */
template <class T>
struct all_bits {
	bool
	operator () (const T& t) const { return !any_bits<T>()(~t); }
};	// end struct all_bits

/**
	Specialized comparison against 0 for std::bitset.
 */
template <size_t NB>
struct all_bits<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	bool
	operator () (const arg_type& t) const {
#if 0
		return (~t).any();
#else
		return t.to_ulong() +1 == (1 << NB);
#endif
	}
};	// end struct all_bits

template <size_t NB>
struct all_bits<bitset<NB> > {
	typedef	bitset<NB>		arg_type;

	bool
	operator () (const arg_type& t) const {
#if 0
		return (~t).any();
#else
		return t.to_ulong() +1 == (1 << NB);
#endif
	}
};	// end struct all_bits

template <>
struct all_bits<bitset<sizeof(size_t) << 3> > {
	typedef	bitset<sizeof(size_t) << 3>		arg_type;

	bool
	operator () (const arg_type& t) const {
		// don't shift, avoid warning
		return !(t.to_ulong() +1);
	}
};	// end struct all_bits

//-----------------------------------------------------------------------------
/**
	Sets the first available bit.  
	Generalized for built-in integer types.  
 */
template <class T>
struct set_any_bit {
	/**
		\pre There is at least one unset bit.  
	 */
	T
	operator () (const T& t) const {
		return t | (t +1);
	}
};	// end struct set_any_bit

/**
	Sets the first available bit.  
	Specialized for std::bitset.
 */
template <size_t NB>
struct set_any_bit<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	/**
		\pre There is at least one unset bit.  
	 */
	arg_type
	operator () (const arg_type& t) const {
#if defined(HAVE_STD_BITSET_FIND_FIRST)
		arg_type temp(~t);
		temp[temp._Find_first()] = 0;
		return ~temp;
#else
		return arg_type(t.to_ulong() | (t.to_ulong() +1));
#endif
	}
};	// end struct all_bits


template <size_t NB>
struct set_any_bit<bitset<NB> > {
	typedef	bitset<NB>		arg_type;

	/**
		\pre There is at least one unset bit.  
	 */
	arg_type
	operator () (const arg_type& t) const {
#if defined(HAVE_STD_BITSET_FIND_FIRST)
		arg_type temp(~t);
		temp[temp.find_first()] = 0;
		return ~temp;
#else
		return arg_type(t.to_ulong() | (t.to_ulong() +1));
#endif
	}
};	// end struct all_bits

//-----------------------------------------------------------------------------
template <class T>
struct print_bits_hex {
	std::ostream&
	operator () (std::ostream&, const T&) const;
};	// end class print_bits_hex

#ifdef	HAVE_UINT64_TYPE
template <>
struct print_bits_hex<uint64> {
	std::ostream&
	operator () (std::ostream&, const uint64&) const;
};	// end class print_bits_hex
#endif

/**
	Currently prints binary, not hex...
 */
template <size_t NB>
struct print_bits_hex<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	std::ostream&
	operator () (std::ostream&, const arg_type&) const;
};	// end class print_bits_hex

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITSET_H__

