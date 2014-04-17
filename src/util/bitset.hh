/**
	\file "util/bitset.hh"
	Extended bitset.
	Also contains wrapper functors for other bit-operations.  
	$Id: bitset.hh,v 1.6 2007/02/22 01:09:14 fang Exp $
 */

#ifndef	__UTIL_BITSET_HH__
#define	__UTIL_BITSET_HH__

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
private:
	// need to expose implementation details, unfortunately
	// _GLIBCXX word type is unsigned long
	typedef size_t			impl_word_type;
	// normal iterator
	typedef impl_word_type*		word_iterator;
	typedef const impl_word_type*	const_word_iterator;
	typedef std::reverse_iterator<word_iterator>
					reverse_word_iterator;
	typedef std::reverse_iterator<const_word_iterator>
					const_reverse_word_iterator;

public:
	typedef	std::bitset<NB>		parent_type;
	enum {
		num_bits = NB,
		num_words = (((NB-1) >> 3) / sizeof(impl_word_type)) +1,
		bits_per_word = sizeof(impl_word_type) << 3
	};

	/**
		Most likely unsigned long.  
	 */
//	typedef	typename parent_type::_WordT	word_type;	// libstdc++
//	typedef	typename parent_type::__storage_type	word_type;	// libc++
	typedef	impl_word_type		word_type;

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

private:
	// need to access underlying representation (words)
	const_word_iterator
	word_begin(void) const {
		return reinterpret_cast<const impl_word_type*>(this);
	}

	const_word_iterator
	word_end(void) const {
		return &this->word_begin()[num_words];
	}

	const_reverse_word_iterator
	word_rbegin(void) const {
		return const_reverse_word_iterator(this->word_end());
	}

	const_reverse_word_iterator
	word_rend(void) const {
		return const_reverse_word_iterator(this->word_begin());
	}

	// not bound checked
	size_t
	__which_word(const size_t b) const {
		return b / bits_per_word;
		// rely on compiler strength reduction
		// or use numeric::divide_by_constant
	}

	// not bound checked
	size_t
	__which_bit(const size_t b) const {
		return b % bits_per_word;
		// rely on compiler strength reduction
		// or use numeric::modulo_by_constant to mask
	}

	size_t
	__get_word(const size_t w) const {
		return *(this->word_begin() +w);
	}

	size_t
	__find_next_from_whole_word(const size_t w) const;

public:

	bool
	all(void) const {
		// return this->count() == this->size();
		return this->to_ulong() == ~(-1UL << NB);
	}

	// TODO: test for SGI extensions and work-around
	size_t
	find_first(void) const
#if defined(HAVE_STD_BITSET_FIND_FIRST)
	{
		return parent_type::_Find_first();
	}
#else
	;
#endif

	// TODO: test for SGI extensions and work-around
	size_t
	find_next(const size_t __prev ) const
#if defined(HAVE_STD_BITSET_FIND_NEXT)
	{
		return parent_type::_Find_next(__prev);
	}
#else
	;
#endif


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
		return t.to_ulong() +1 == (1UL << NB);
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

#endif	// __UTIL_BITSET_HH__

