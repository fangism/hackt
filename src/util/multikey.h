/**
	\file "multikey.h"
	Multidimensional key class, use to emulate true multiple dimensions
	with a standard map class.
	$Id: multikey.h,v 1.8 2004/11/02 07:52:15 fang Exp $
 */

#ifndef	__MULTIKEY_H__
#define	__MULTIKEY_H__

#include <assert.h>
#include <iostream>
#include <algorithm>		// for transform
#include <functional>
// #include <numeric>

#ifndef	USE_STL_ALGORITHM
#define	USE_STL_ALGORITHM		1
#endif

#include "multikey_fwd.h"

/***
	Later be able to compare between keys of different dimensions.  
***/

namespace MULTIKEY_NAMESPACE {
using namespace std;

//=============================================================================
/**
	Abstract interface for an N-dimensional key.  
 */
template <class K>
class multikey_base {
public:
	typedef	K*				iterator;
	typedef	const K*			const_iterator;
	typedef	K&				reference;
	typedef	const K&			const_reference;
public:
	static const size_t			LIMIT = 4;
public:
virtual	~multikey_base() { }
virtual	size_t dimensions(void) const = 0;
	size_t size(void) const { return this->dimensions(); }
virtual	K default_value(void) const = 0;

virtual	iterator begin(void) = 0;
virtual	const_iterator begin(void) const = 0;
virtual	iterator end(void) = 0;
virtual	const_iterator end(void) const = 0;
	reference front(void) { return *begin(); }
	const_reference front(void) const { return *begin(); }
	reference back(void) {
		iterator e = end();
		return *(--e);
	}
	const_reference back(void) const {
		const_iterator e = end();
		return *(--e);
	}

virtual	multikey_base<K>& operator = (const multikey_base<K>& k) = 0;

virtual	reference operator [] (const size_t i) = 0;
virtual	const_reference operator [] (const size_t i) const = 0;

static	multikey_base<K>* make_multikey(const size_t d);
};	// end class multikey_base

//=============================================================================
/**
	Just a wrapper class for a fixed-size array.  
	Useful for emulating a multidimensional map using a flat map
	with a multidimensional key.  
	Saves the author from the trouble of writing a multidimensional
	iterator.  
	We choose to use a plain static array, because we don't need the 
	overhead from vector or valarray.  
	Perhaps add another field for default value?
 */
template <size_t D, class K, K init>
class multikey : virtual public multikey_base<K> {
	template <size_t, class C, C>
	friend class multikey;
public:
	typedef	multikey_base<K>			base_type;
	typedef	multikey<D,K>				this_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
	typedef	typename base_type::reference		reference;
	typedef	typename base_type::const_reference	const_reference;
public:
	K indices[D];

public:
	/**
		Default constructor.  
		Requires key type K to be assignable and copiable.  
	 */
	multikey(const K i = init) { fill(indices, &indices[D], i); }

	/**
		Copy constructor compatible with other dimensions.  
		If this is larger than argument, fill remaining
		dimensions with 0.  
	 */
	template <size_t D2, K init2>
	multikey(const multikey<D2,K,init2>& k, const K i = init) {
		// depends on <algorithm>
		if (D <= D2) {
			copy(k.indices, &k.indices[D], indices);
		} else {
			copy(k.indices, &k.indices[D2], indices);
			fill(&indices[D2], &indices[D], i);
		}
	}

#if 0
	multikey(const multikey_base<K>& k) {
		// depends on <algorithm>
		const size_t D2 = k.dimensions();
		if (D <= D2) {
			copy(k.indices, &k.indices[D], indices);
		} else {
			copy(k.indices, &k.indices[D2], indices);
			fill(&indices[D2], &indices[D], init);
		}
	}
#endif

	/**
		\param S is sequence type container.
		\param s is const reference to sequence object.
	 */
	template <template <class> class S>
	explicit
	multikey(const S<K>& s, const K i = init)
	{
		const size_t sz = s.size();
		if (D < sz) {
			size_t i = 0;
			typename S<K>::const_iterator iter = s.begin();
			for ( ; i<sz; i++)
				indices[i] = *iter;
		} else {
			copy(s.begin(), s.end(), indices);
			fill(&indices[sz], &indices[D], i);
		}
	}

	size_t
	dimensions(void) const { return D; }

	K
	default_value(void) const { return init; }

	iterator
	begin(void) { return &indices[0]; }

	const_iterator
	begin(void) const { return &indices[0]; }

	iterator
	end(void) { return &indices[D]; }

	const_iterator
	end(void) const { return &indices[D]; }

	/**
		Safe indexing with array-bound check.  
		indices is public, so one can always access it directly...
	 */
	reference
	operator [] (const size_t i) {
		assert(i < D);
		return indices[i];
	}

	/**
		Const version of array indexing.  
	 */
	const_reference
	operator [] (const size_t i) const {
		assert(i < D);
		return indices[i];
	}

	/**
		Generic assignment operation.
	 */
	multikey_base<K>&
	operator = (const multikey_base<K>& s) {
		const size_t sz = s.dimensions();
		if (D < sz) {
			size_t i = 0;
			const_iterator iter = s.begin();
			for ( ; i<sz; i++)
				indices[i] = *iter;
		} else {
			copy(s.begin(), s.end(), indices);
			fill(&indices[sz], &indices[D], init);
		}
		return *this;
	}

	this_type&
	operator = (const this_type& s) {
		copy(s.begin(), s.end(), this->begin());
		return *this;
	}

	/**
		Helper class for finding index extremities in sets.  
	 */
	struct accumulate_extremities {
		typedef pair<this_type, this_type>	key_pair;
		this_type		min, max;

		accumulate_extremities() : min(), max() { }
		explicit accumulate_extremities(const multikey<D,K>& i) :
			min(i), max(i) { }
	private:
		// has trouble finding std::min/max in stl_algobase... grrr...
		static 
		K mymin(const K& a, const K& b) { return (a<b)?a:b; }
		static 
		K mymax(const K& a, const K& b) { return (a>b)?a:b; }
	public:
		void
		operator () (const multikey<D,K>& k) {
			transform(min.begin(), min.end(), k.begin(), 
				min.begin(), ptr_fun(mymin));
			transform(max.begin(), max.end(), k.begin(), 
				max.begin(), ptr_fun(mymax));
		}

		key_pair
		operator () (const key_pair& a, const multikey<D,K>& b) {
			key_pair ret;
			transform(a.first.begin(), a.first.end(), b.begin(), 
				ret.first.begin(), ptr_fun(mymin));
			transform(a.second.begin(), a.second.end(), b.begin(), 
				ret.second.begin(), ptr_fun(mymax));
			return ret;
		}

		template <class T>
		void
		operator () (const pair<const multikey<D,K>, T>& p) {
			this->operator()(p.first);
		}

		template <class T>
		key_pair
		operator () (const key_pair& a,
				const pair<const multikey<D,K>, T>& p) {
			return this->operator()(a, p.first);
		}

	};	// end struct accumulate_extremities
};	// end class multikey

//-----------------------------------------------------------------------------
template <class K>
multikey_base<K>*
multikey_base<K>::make_multikey(const size_t d) {
	assert(d > 0 && d <= LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1: return new multikey<1,K>();
		case 2: return new multikey<2,K>();
		case 3: return new multikey<3,K>();
		case 4: return new multikey<4,K>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
template <class K>
ostream&
operator << (ostream& o, const multikey_base<K>& k) {
	typename multikey_base<K>::const_iterator i = k.begin();
	const typename multikey_base<K>::const_iterator e = k.end();
	for ( ; i!=e; i++)
		o << '[' << *i << ']';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k) {
	register size_t i = 0;
	for ( ; i<D; i++)
		o << '[' << k.indices[i] << ']';
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator < (const multikey<D,K>& l, const multikey<D,K>& r) {
#if USE_STL_ALGORITHM
	return lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
#else
	register size_t i = 0;
	for ( ; i<D; i++) {
		register const size_t x = l.indices[i], y = r.indices[i];
		if (x != y)
			return (x < y);
	}
	// else all equal
	return false;
#endif	// USE_STL_ALGORITHM
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator < (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator > (const multikey<D,K>& l, const multikey<D,K>& r) {
#if USE_STL_ALGORITHM
	return lexicographical_compare(r.begin(), r.end(), l.begin(), l.end());
#else
	register size_t i = 0;
	for ( ; i<D; i++) {
		register const size_t x = l.indices[i], y = r.indices[i];
		if (x != y)
			return (x > y);
	}
	// else all equal
	return false;
#endif
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator > (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return lexicographical_compare(r.begin(), r.end(), l.begin(), l.end());
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator == (const multikey<D,K>& l, const multikey<D,K>& r) {
#if USE_STL_ALGORITHM
	return equal(l.begin(), l.end(), r.begin());
#else
	register size_t i = 0;
	for ( ; i<D; i++) {
		register const size_t x = l.indices[i], y = r.indices[i];
		if (!equal(x,y))
			return false;
	}
	// else all equal
	return true;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dimensions must match!
 */
template <class K>
bool
operator == (const multikey_base<K>& l, const multikey_base<K>& r) {
	if (l.dimensions() != r.dimensions())
		return false;
#if USE_STL_ALGORITHM
	return equal(l.begin(), l.end(), r.begin());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator != (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l == r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class K>
bool
operator != (const multikey_base<K>& l, const multikey_base<K>& r) {
	return !(l == r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator <= (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l > r);
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator <= (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return !(l > r);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator >= (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l < r);
}

#if USE_STL_ALGORITHM
template <size_t D1, size_t D2, class K>
bool
operator >= (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return !(l < r);
}
#endif

//=============================================================================
/**
	Interface for generating cyclic keys.  
 */
template <class K>
class multikey_generator_base : virtual public multikey_base<K> {
public:
	typedef	typename multikey_base<K>::iterator		iterator;
	typedef	typename multikey_base<K>::const_iterator	const_iterator;
public:
virtual	~multikey_generator_base() { }

virtual	void validate(void) const = 0;
virtual	void initialize(void) = 0;

virtual	size_t size(void) const = 0;

virtual	iterator begin(void) = 0;
virtual	const_iterator begin(void) const = 0;
virtual	iterator end(void) = 0;
virtual	const_iterator end(void) const = 0;

virtual multikey_base<K>& get_lower_corner(void) = 0;
virtual const multikey_base<K>& get_lower_corner(void) const = 0;
virtual multikey_base<K>& get_upper_corner(void) = 0;
virtual const multikey_base<K>& get_upper_corner(void) const = 0;

virtual	multikey_base<K>& operator ++ (int) = 0;

static	multikey_generator_base<K>*
		make_multikey_generator(const size_t d);
};	// end class multikey_generator_base

//-----------------------------------------------------------------------------
/**
	Only works for integer-like keys.  
	Extension of a standard multikey, with knowledge of bounds.  
 */
template <size_t D, class K>
class multikey_generator : public multikey<D,K>, 
		public multikey_generator_base<K> {
public:
	typedef	multikey<D,K>				base_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
// protected:
public:		// for sake of laziness and convenience
	/** vector of lower bounds */
	base_type		lower_corner;
	/** vector of upper bounds */
	base_type		upper_corner;
public:
	multikey_generator() : base_type(), lower_corner(), upper_corner() { }
	multikey_generator(const multikey<D,K>& l, const multikey<D,K>& u) :
		base_type(), lower_corner(l), upper_corner(u) { }
	// use default destructor

	/**
		Make sure bounds are sane.  
	 */
	void
	validate(void) const {
		const_iterator min = lower_corner.begin();
		const_iterator max = upper_corner.begin();
		const const_iterator min_end = lower_corner.end();
		for ( ; min != min_end; min++, max++) {
			assert(*min <= *max);
		}
	}

	void
	initialize(void) {
		validate();
		copy(lower_corner.begin(), lower_corner.end(), this->begin());
	}

	size_t
	size(void) const { return base_type::dimensions(); }

	iterator
	begin(void) { return base_type::begin(); }

	const_iterator
	begin(void) const { return base_type::begin(); }

	iterator
	end(void) { return base_type::end(); }

	const_iterator
	end(void) const { return base_type::end(); }

	multikey_base<K>&
	get_lower_corner(void) { return lower_corner; }

	const multikey_base<K>&
	get_lower_corner(void) const { return lower_corner; }

	multikey_base<K>&
	get_upper_corner(void) { return upper_corner; }

	const multikey_base<K>&
	get_upper_corner(void) const { return upper_corner; }

	// can be used directly as key, no need to convert
	multikey_base<K>&
	operator ++ (int) {
#if 0
		iterator inc = --(base_type::end());
		const const_iterator msp = --(base_type::begin());
		const_iterator min = --(lower_corner.end());
		const_iterator max = --(upper_corner.end());
#else
		iterator inc = &this->indices[D-1];
		const const_iterator msp = &this->indices[-1];
		const_iterator min = &lower_corner.indices[D-1];
		const_iterator max = &upper_corner.indices[D-1];
#endif
		for ( ; inc != msp; inc--, min--, max--) {
			if (*inc >= *max)
				*inc = *min;
			else {
				(*inc)++;
				break;
			}
		}
		return *this;
	}

	// all other methods inherited

};	// end class multikey_generator

//-----------------------------------------------------------------------------
template <class K>
multikey_generator_base<K>*
multikey_generator_base<K>::make_multikey_generator(const size_t d) {
	assert(d > 0 && d <= multikey_base<K>::LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1: return new multikey_generator<1,K>();
		case 2: return new multikey_generator<2,K>();
		case 3: return new multikey_generator<3,K>();
		case 4: return new multikey_generator<4,K>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
#if 0
template <class K>
ostream&
operator << (ostream& o, const multikey_generator_base<K>& k) {
	const multikey_base<K>* mk =
		dynamic_cast<const multikey_base<K>*>(&k);
	assert(mk);
	return o << *mk;
}
#endif

//=============================================================================
}	// end namespace MULTIKEY_NAMESPACE



#endif	//	__MULTIKEY_H__

