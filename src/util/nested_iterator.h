/**
	\file "nested_iterator.h"
	Iterator for container of containers that gives the 
	abstraction of an iterator of the inner container.  

	\todo Specify all pre-conditions, post-conditions, and semantics.  

	$Id: nested_iterator.h,v 1.4 2004/12/05 05:07:25 fang Exp $
 */

#ifndef	__NESTED_ITERATOR_H__
#define	__NESTED_ITERATOR_H__

#include <utility>
#include <iterator>
#include <numeric>

// for branch prediction
#include "likely.h"

#define	DEBUG_NESTED_ITERATOR	0

#if DEBUG_NESTED_ITERATOR
#include <iostream>
#endif


namespace util {
using std::iterator;
using std::accumulate;
using std::pair;

#if DEBUG_NESTED_ITERATOR
#include "using_ostream.h"
#endif

//=============================================================================
/**
	Iterator for a container of containers.  
	We derive from std::pair to leverage its comparison
	and assignment operators.  

	Inspired by:
	http://www.moderncppdesign.com/publications/compound_iterators.html

	Note, however, that this is not the same as a compound_iterator, 
	described in the above reference.  
	Compound_iterators allow one to iterate in orthogonal directions, 
	such as along a column or row.  

	This gives the abstraction of iterating over a huge container
	of the inner container type.  
	This is designed to work with list-like, vector- or array-like 
	containers.  

	One thing to remark is that random access is only done in 
	pseudo-constant time because the size of the inner containers
	is not known statically, they may be lists of arbitrary
	dynamic size.  To achieve true constant time random access, 
	the inner container must satisfy the fixed-size concept
	as defined and used in "reserved_vector.h".  
	For such containers, one can use the faster, specialized, 
	nested_reserved_iterator.  
	
	\param OuterIter the iterator type of the outer container.  
	\param InnerIter the iterator type of the inner container.

	Requirements: the InnerIter type is an iterator for 
		the inner-container type contained by OuterIter.
	Example: any A<B<T> > where A, B are {list, vector}.

	Consider deriving from public:
	iterator<InnerIter::iterator_category, 
		InnerIter::value_type, 
		InnerIter::difference_type, 
		InnerIter::pointer, 
		InnerIter::reference>
 */
template <class OuterIter, class InnerIter>
class nested_iterator :
	// implementation: inner and outer iterators
	private pair<OuterIter, InnerIter>, 
	// abstraction: should look and feel like an InnerIter iterator type
	public iterator<typename InnerIter::iterator_category, 
		typename InnerIter::value_type, 
		typename InnerIter::difference_type, 
		typename InnerIter::pointer, 
		typename InnerIter::reference> {
// allow the entire family to reference each other's private members directly
template <class, class>
friend class nested_iterator;

private:
	typedef	pair<OuterIter, InnerIter>		base_type;
	typedef	iterator<typename InnerIter::iterator_category, 
			typename InnerIter::value_type, 
			typename InnerIter::difference_type, 
			typename InnerIter::pointer, 
			typename InnerIter::reference>	interface_type;
	typedef	OuterIter				outer_iterator_type;
	typedef	InnerIter				inner_iterator_type;
public:
	/**
		If inner contained is random-accessible, then the
		nested iterator will "inherit" the random_access_tag.
	 */
	typedef	typename interface_type::iterator_category
							iterator_category;
	typedef	typename interface_type::difference_type
							difference_type;
	typedef	typename interface_type::reference	reference;
	typedef	typename interface_type::pointer	pointer;
	typedef	typename interface_type::value_type	value_type;
// concept requirements?
public:
	/// default empty constructor
	nested_iterator() : base_type(), interface_type() { }

	/// pair-wise iterator constructor
	nested_iterator(const OuterIter& o, const InnerIter& i) :
		base_type(o, i), interface_type() { }

	// default copy constructor, need not be explicit
	// nested_iterator(const nested_iterator& i) : base_type(i) { }

	/// to allow const_iterator construction
	template <class AnotherOuterIter, class AnotherInnerIter>
	inline
	nested_iterator(
		const nested_iterator<AnotherOuterIter, AnotherInnerIter>& i) :
			base_type(i.first, i.second), interface_type() { }

	// public const accessors
	const OuterIter&
	outer_iterator(void) const {
		return this->first;
	}

	const InnerIter&
	inner_iterator(void) const {
		return this->second;
	}

	// default destructor

	/// dereference operator
	reference
	operator * () const { return *this->second; }

	/// indirect reference operator
	pointer
	operator -> () const { return &*this->second; }

	/// prefix iterator advance (forward iterator concept)
	// will this work for reverse iterator?
	nested_iterator&
	operator ++ () {
		this->second++;
		if (UNLIKELY(this->second == this->first->end())) {
			// what if first already at the last chunk?
			// need a sentinel, unfortunately
			this->first++;
			this->second = this->first->begin();
			// note: reverse iterators will want rbegin()!
		}
		return *this;
	}

	/// postfix iterator advance (forward iterator concept)
	nested_iterator
	operator ++ (int) {
		nested_iterator ret(*this);
		++(*this);
		return ret;
	}

	/// prefix iterator regression (bidirectional iterator concept)
	// will this work for reverse iterator?
	nested_iterator&
	operator -- () {
		if (UNLIKELY(this->second == this->first->begin())) {
			// what if first is already at the first chunk?
			// is ok, because it's comparable to begin()
			this->first--;
			this->second = --this->first->end();
		} else {
			this->second--;
		}
		return *this;
	}

	/// postfix iterator regression (bidirectional iterator requirement)
	nested_iterator
	operator -- (int) {
		nested_iterator ret(*this);
		--(*this);
		return ret;
	}

	reference
	operator [] (const difference_type& n) const {
		if (n > 0) {
			const difference_type d = 
				distance(this->second, this->first->end());
			if (n >= d) {
				// need to advance first
				register difference_type m = n -d;
				outer_iterator_type o = this->first +1;
				register difference_type s = o->size();
				while (m >= s) {
					m -= s;
					o++;
					s = o->size();
				}
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = o->begin();
				advance(ret, m);
				return *ret;
				// return o->begin()[m];	// same
			} else {
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = this->second;
				advance(ret, n);
				return *ret;
				// return second[n];		// same
			}
		} else if (n < 0) {
			const difference_type d = 
				distance(this->first->begin(), this->second);
			if (d + n > 0) {
				register difference_type m = n -d;
				outer_iterator_type o = this->first;
				o--;	// move back one first
				register difference_type s = o->size();
				while (m > s) {
					m -= s;
					o--;
					s = o->size();
				}
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = o->end();
				advance(ret, -m);
				return *ret;
				// return o->end()[-m];
			} else {
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = this->second;
				advance(ret, n);
				return *ret;
				// return second[n];
			}
		}
		// else NULL operation
	}

	nested_iterator&
	operator += (const difference_type& n) {
		if (n > 0) {
			const difference_type d = 
				distance(this->second, this->first->end());
			if (n >= d) {
				// need to advance first
				register difference_type m = n -d;
				this->first++;
				register difference_type s =
					this->first->size();
				while (m >= s) {
					m -= s;
					this->first++;
					s = this->first->size();
				}
				this->second = this->first->begin();
				advance(this->second, m);
				// second = first->begin() +m;
			} else {
				// if second is random_access_iterator
				// this will be fast
				advance(this->second, n);	// second += n;
			}
		} else {
			(*this) -= -n;		// too lazy to copy
		}
		return *this;
	}

	nested_iterator
	operator + (const difference_type& n) const {
		nested_iterator ret(*this);
		advance(ret, n);		// ret += n;
		return ret;
	}

	nested_iterator&
	operator -= (const difference_type& n) {
		if (n > 0) {
			const difference_type d = 
				distance(this->first->begin(), this->second);
			if (n > d) {
				register difference_type m = n -d;
				this->first--;
				register difference_type s =
					this->first->size();
				while (m > s) {
					m -= s;
					this->first--;
					s = this->first->size();
				}
				this->second = this->first->end();
				advance(this->second, -m);
				// second = first->end() -m;
			} else {
				advance(this->second, -n);	// second -= n;
			}
		} else {
			(*this) += -n;		// too lazy to copy
		}
		return *this;
	}

	nested_iterator
	operator - (const difference_type& n) const {
		nested_iterator ret(*this);
		advance(ret, -n);		// ret -= n;
		return ret;
	}

};	// end class nested_iterator

//=============================================================================
/**
	Reverse nested iterator.  
	As much as I hate copy/pasting the above code, 
	this is needed because we need to substitute begin() with rbegin(), 
	and end(), with rend().  
	That would've been trivial if there were a tag to detect 
	the "forward direction" of the iterators, 
	in which case, a specialization using the direction tag
	would suffice to select between [r]begin and [r]end.  

	\param OuterIter a reverse iterator of the outer container type.  
	\param Inner Iter a reverse iterator of the inner container type.  

	NOTE: if forward and reverse iterators are mixed, shame on you!
		The behavior for such a blatant error is undefined.  
		Value sequences of nested containers
		are likely to appear shuffled.  

	NOTE: this abers from the standard reverse_iterator implementation
		in that the internal representation points to the 
		element referenced, and not one-past.  
		This creates an unforatunate burdern on the user
		to guarantee that the container has a 
		one-efore-the-beginning placeholder or head-sentinel.  
		We will revisit this in the future and consider 
		reimplenting it to conform to the standard.  

	Other ideas: wrap around a forward iterator, via containership.  
	Fear of inheritance: afraid that base class will instantiate
		incorrect version, even if it is unused.  
 */
template <class OuterIter, class InnerIter>
class nested_reverse_iterator :
	// implementation: inner and outer iterators
	private pair<OuterIter, InnerIter>, 
	// abstraction: should look and feel like an InnerIter iterator type
	// would really just like to use reverse_iterator<T>
	public iterator<typename InnerIter::iterator_category, 
		typename InnerIter::value_type, 
		typename InnerIter::difference_type, 
		typename InnerIter::pointer, 
		typename InnerIter::reference> {
// allow the entire family to reference each other's private members directly
template <class, class>
friend class nested_reverse_iterator;

private:
	typedef	pair<OuterIter, InnerIter>		base_type;
	typedef	iterator<typename InnerIter::iterator_category, 
			typename InnerIter::value_type, 
			typename InnerIter::difference_type, 
			typename InnerIter::pointer, 
			typename InnerIter::reference>	interface_type;
	typedef	OuterIter				outer_iterator_type;
	typedef	InnerIter				inner_iterator_type;
public:
	/**
		If inner contained is random-accessible, then the
		nested iterator will "inherit" the random_access_tag.
	 */
	typedef	typename interface_type::iterator_category
							iterator_category;
	typedef	typename interface_type::difference_type
							difference_type;
	typedef	typename interface_type::reference	reference;
	typedef	typename interface_type::pointer	pointer;
	typedef	typename interface_type::value_type	value_type;
// concept requirements?
public:
	/// default empty constructor
	nested_reverse_iterator() : base_type(), interface_type() { }

	/// pair-wise iterator constructor
	nested_reverse_iterator(const OuterIter& o, const InnerIter& i) :
		base_type(o, i), interface_type() { }

	// default copy constructor, need not be explicit
	// nested_iterator(const nested_iterator& i) : base_type(i) { }

	/// to allow const_iterator construction
	template <class AnotherOuterIter, class AnotherInnerIter>
	inline
	nested_reverse_iterator(
		const nested_reverse_iterator<AnotherOuterIter, AnotherInnerIter>& i) :
			base_type(i.first, i.second), interface_type() { }

	// public const accessors
	const OuterIter&
	outer_iterator(void) const {
		return this->first;
	}

	const InnerIter&
	inner_iterator(void) const {
		return this->second;
	}

	// default destructor

	/// dereference operator
	reference
	operator * () const { return *this->second; }

	/// indirect reference operator
	pointer
	operator -> () const { return &*this->second; }

	/// prefix iterator advance (forward iterator concept)
	// will this work for reverse iterator?
	nested_reverse_iterator&
	operator ++ () {
		this->second++;
		if (UNLIKELY(this->second == this->first->rend())) {
			// what if first already at the last chunk?
			// need a sentinel, unfortunately
			this->first++;
			this->second = this->first->rbegin();
			// note: reverse iterators will want rbegin()!
		}
		return *this;
	}

	/// postfix iterator advance (forward iterator concept)
	nested_reverse_iterator
	operator ++ (int) {
		nested_reverse_iterator ret(*this);
		++(*this);
		return ret;
	}

	/// prefix iterator regression (bidirectional iterator concept)
	// will this work for reverse iterator?
	nested_reverse_iterator&
	operator -- () {
		if (UNLIKELY(this->second == this->first->rbegin())) {
			// what if first is already at the first chunk?
			// is ok, because it's comparable to begin()
			this->first--;
			this->second = --this->first->rend();
		} else {
			this->second--;
		}
		return *this;
	}

	/// postfix iterator regression (bidirectional iterator requirement)
	nested_reverse_iterator
	operator -- (int) {
		nested_reverse_iterator ret(*this);
		--(*this);
		return ret;
	}

	reference
	operator [] (const difference_type& n) const {
		if (n > 0) {
			const difference_type d = 
				distance(this->second, this->first->rend());
			if (n >= d) {
				// need to advance first
				register difference_type m = n -d;
				outer_iterator_type o = this->first +1;
				register difference_type s = o->size();
				while (m >= s) {
					m -= s;
					o++;
					s = o->size();
				}
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = o->rbegin();
				advance(ret, m);
				return *ret;
				// return o->rbegin()[m];	// same
			} else {
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = this->second;
				advance(ret, n);
				return *ret;
				// return second[n];		// same
			}
		} else if (n < 0) {
			const difference_type d = 
				distance(this->first->rbegin(), this->second);
			if (d + n > 0) {
				register difference_type m = n -d;
				outer_iterator_type o = this->first;
				o--;	// move back one first
				register difference_type s = o->size();
				while (m > s) {
					m -= s;
					o--;
					s = o->size();
				}
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = o->rend();
				advance(ret, -m);
				return *ret;
				// return o->rend()[-m];
			} else {
				// inner_iterator_type may not be random_access
				// register object?
				inner_iterator_type ret = this->second;
				advance(ret, n);
				return *ret;
				// return second[n];
			}
		}
		// else NULL operation
	}

	nested_reverse_iterator&
	operator += (const difference_type& n) {
		if (n > 0) {
			const difference_type d = 
				distance(this->second, this->first->rend());
			if (n >= d) {
				// need to advance first
				register difference_type m = n -d;
				this->first++;
				register difference_type s =
					this->first->size();
				while (m >= s) {
					m -= s;
					this->first++;
					s = this->first->size();
				}
				this->second = this->first->rbegin();
				advance(this->second, m);
				// second = first->rbegin() +m;
			} else {
				// if second is random_access_iterator
				// this will be fast
				advance(this->second, n);	// second += n;
			}
		} else {
			(*this) -= -n;		// too lazy to copy
		}
		return *this;
	}

	nested_reverse_iterator
	operator + (const difference_type& n) const {
		nested_reverse_iterator ret(*this);
		advance(ret, n);		// ret += n;
		return ret;
	}

	nested_reverse_iterator&
	operator -= (const difference_type& n) {
		if (n > 0) {
			const difference_type d = 
				distance(this->first->rbegin(), this->second);
			if (n > d) {
				register difference_type m = n -d;
				this->first--;
				register difference_type s =
					this->first->size();
				while (m > s) {
					m -= s;
					this->first--;
					s = this->first->size();
				}
				this->second = this->first->rend();
				advance(this->second, -m);
				// second = first->rend() -m;
			} else {
				advance(this->second, -n);	// second -= n;
			}
		} else {
			(*this) += -n;		// too lazy to copy
		}
		return *this;
	}

	nested_reverse_iterator
	operator - (const difference_type& n) const {
		nested_reverse_iterator ret(*this);
		advance(ret, -n);		// ret -= n;
		return ret;
	}

};	// end class nested_reverse_iterator

//=============================================================================
// standard iterator operators

/***
	Quote from gcc developer Gabriel Dos Reis, 
	taken from <bits/stl_iterator.h> gcc-3.3 standard headers:

	Note: In what follows, the left- and right-hand-side iterators are
	allowed to vary in types (conceptually in cv-qualification) so that
	comparaison between cv-qualified and non-cv-qualified iterators be
	valid.  However, the greedy and unfriendly operators in std::rel_ops
	will make overload resolution ambiguous (when in scope) if we don't
	provide overloads whose operands are of the same type.  Can someone
	remind me what generic programming is about? -- Gaby
***/

/**
	Equality between two nested iterators.  
	Required for forward iterator concept.
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator == (const nested_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() == r.outer_iterator()) &&
		(l.inner_iterator() == r.inner_iterator());
}

template <class OuterIter, class InnerIter>
inline
bool
operator == (const nested_iterator<OuterIter, InnerIter>& l, 
		const nested_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() == r.outer_iterator()) &&
		(l.inner_iterator() == r.inner_iterator());
}

// nested_revers_iterator versions ... damn replication
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator == (const nested_reverse_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_reverse_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() == r.outer_iterator()) &&
		(l.inner_iterator() == r.inner_iterator());
}

template <class OuterIter, class InnerIter>
inline
bool
operator == (const nested_reverse_iterator<OuterIter, InnerIter>& l, 
		const nested_reverse_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() == r.outer_iterator()) &&
		(l.inner_iterator() == r.inner_iterator());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Inequality between two nested iterators.  
	Required for forward iterator concept.
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator != (const nested_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() != r.outer_iterator()) ||
		(l.inner_iterator() != r.inner_iterator());
}

template <class OuterIter, class InnerIter>
inline
bool
operator != (const nested_iterator<OuterIter, InnerIter>& l, 
		const nested_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() != r.outer_iterator()) ||
		(l.inner_iterator() != r.inner_iterator());
}

template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator != (const nested_reverse_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_reverse_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() != r.outer_iterator()) ||
		(l.inner_iterator() != r.inner_iterator());
}

template <class OuterIter, class InnerIter>
inline
bool
operator != (const nested_reverse_iterator<OuterIter, InnerIter>& l, 
		const nested_reverse_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() != r.outer_iterator()) ||
		(l.inner_iterator() != r.inner_iterator());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Strictly-less ordering relation between two iterators.  
	Required for random-access iterator concept.
	Only makes sense to compare inner iterators when outer
	iterators are equal.
	The outer iterator must be random-access to be comparable.  
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator < (const nested_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() < r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator < (const nested_iterator<OuterIter, InnerIter>& l, 
		const nested_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() < r.inner_iterator()));
}

template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator < (const nested_reverse_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_reverse_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() < r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator < (const nested_reverse_iterator<OuterIter, InnerIter>& l, 
		const nested_reverse_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() < r.inner_iterator()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Strictly-greater ordering relation between two iterators.  
	Required for random-access iterator concept.
	Only makes sense to compare inner iterators when outer
	iterators are equal.
	The outer iterator must be random-access to be comparable.  
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator > (const nested_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() > r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator > (const nested_iterator<OuterIter, InnerIter>& l, 
		const nested_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() > r.inner_iterator()));
}

template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator > (const nested_reverse_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_reverse_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() > r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator > (const nested_reverse_iterator<OuterIter, InnerIter>& l, 
		const nested_reverse_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() > r.inner_iterator()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Strictly-less ordering relation between two iterators.  
	Required for random-access iterator concept.
	Only makes sense to compare inner iterators when outer
	iterators are equal.
	The outer iterator must be random-access to be comparable.  
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator <= (const nested_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() <= r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator <= (const nested_iterator<OuterIter, InnerIter>& l, 
		const nested_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() <= r.inner_iterator()));
}

template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator <= (const nested_reverse_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_reverse_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() <= r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator <= (const nested_reverse_iterator<OuterIter, InnerIter>& l, 
		const nested_reverse_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() < r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() <= r.inner_iterator()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Strictly-greater ordering relation between two iterators.  
	Required for random-access iterator concept.
	Only makes sense to compare inner iterators when outer
	iterators are equal.
	The outer iterator must be random-access to be comparable.  
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator >= (const nested_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() >= r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator >= (const nested_iterator<OuterIter, InnerIter>& l, 
		const nested_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() >= r.inner_iterator()));
}

template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
bool
operator >= (const nested_reverse_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_reverse_iterator<RightOuterIter, RightInnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() >= r.inner_iterator()));
}

template <class OuterIter, class InnerIter>
inline
bool
operator >= (const nested_reverse_iterator<OuterIter, InnerIter>& l, 
		const nested_reverse_iterator<OuterIter, InnerIter>& r) {
	return (l.outer_iterator() > r.outer_iterator()) ||
		((l.outer_iterator() == r.outer_iterator()) &&
			(l.inner_iterator() >= r.inner_iterator()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for accumulating sizes of structures.  
	\param Iter iterator whose value type implements a size(void) method.
 */
template <class Iter>
struct size_accumulator {
	typedef	typename Iter::difference_type	difference_type;
	typedef	typename Iter::value_type	value_type;
	difference_type
	operator() (const difference_type& s, const value_type& v) const {
		return s +v.size();
	}
};	// end struct size_accumulator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Computing iterator distances.  
	\pre if outer iterator type is not random access type, 
		then l.outer_iterator() must be forward-reachable 
		from r.outer_iterator().  This is because the 
		distance() function only searches forward.
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
typename nested_iterator<LeftOuterIter, LeftInnerIter>::difference_type
operator - (const nested_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_iterator<RightOuterIter, RightInnerIter>& r) {
	typedef	typename nested_iterator<LeftOuterIter, LeftInnerIter>::difference_type return_type;
	typedef	typename LeftOuterIter::difference_type outer_difference_type;
#if DEBUG_NESTED_ITERATOR
	cerr << "nested_iterator::operator - (): " << endl;
#endif
	const RightOuterIter& roi = r.outer_iterator();
	// this distance() is specialized for random-access iterators
	const outer_difference_type outer_diff =
		distance(roi, l.outer_iterator());
	if (!outer_diff) {
#if DEBUG_NESTED_ITERATOR
		cerr << "outer iterators match." << endl;
#endif
		return distance(r.inner_iterator(), l.inner_iterator());
	} else if (LIKELY(outer_diff > 0)) {
#if DEBUG_NESTED_ITERATOR
		cerr << "outer iterators mismatch." << endl;
#endif
		const return_type tail =
			distance(l.outer_iterator()->begin(), l.inner_iterator());
			// l.inner_iterator() -l.outer_iterator()->begin();
		const return_type head =
			distance(r.inner_iterator(), roi->end());
			// roi->end() -r.inner_iterator() -1;
		RightOuterIter roi_1(roi);
		roi_1++;
		return accumulate(roi_1, l.outer_iterator(), head +tail, 
			size_accumulator<RightOuterIter>());
	} else {
		// this case can be detected for only
		// dandom access outer iterators
		// uncommon case: outer_diff < 0
		return -(r -l);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Computing reverse iterator distances.  
	\pre if outer iterator type is not random access type, 
		then l.outer_iterator() must be forward-reachable 
		from r.outer_iterator().  This is because the 
		distance() function only searches forward.
 */
template <class LeftOuterIter, class LeftInnerIter, 
	class RightOuterIter, class RightInnerIter>
inline
typename nested_reverse_iterator<LeftOuterIter, LeftInnerIter>::difference_type
operator - (const nested_reverse_iterator<LeftOuterIter, LeftInnerIter>& l, 
		const nested_reverse_iterator<RightOuterIter, RightInnerIter>& r) {
	typedef	typename nested_reverse_iterator<LeftOuterIter, LeftInnerIter>::difference_type return_type;
	typedef	typename LeftOuterIter::difference_type outer_difference_type;
#if DEBUG_NESTED_ITERATOR
	cerr << "nested_reverse_iterator::operator - (): " << endl;
#endif
	const RightOuterIter& roi = r.outer_iterator();
	// this distance() is specialized for random-access iterators
	const outer_difference_type outer_diff =
		distance(roi, l.outer_iterator());
	if (!outer_diff) {
#if DEBUG_NESTED_ITERATOR
		cerr << "outer reverse_iterators match." << endl;
#endif
		return distance(r.inner_iterator(), l.inner_iterator());
	} else if (LIKELY(outer_diff > 0)) {
#if DEBUG_NESTED_ITERATOR
		cerr << "outer reverse_iterators mismatch." << endl;
#endif
		const return_type tail =
			distance(l.outer_iterator()->rbegin(), l.inner_iterator());
		const return_type head =
			distance(r.inner_iterator(), roi->rend());
		RightOuterIter roi_1(roi);
		roi_1++;
		return accumulate(roi_1, l.outer_iterator(), head +tail, 
			size_accumulator<RightOuterIter>());
	} else {
		// this case can be detected for only
		// dandom access outer iterators
		// uncommon case: outer_diff < 0
		return -(r -l);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Commutative iterator and difference_type addition.  
	Uses operator+(difference_type), which is specialized based
	on underlying iterator type.  
 */
template <class OuterIter, class InnerIter>
inline
nested_iterator<OuterIter, InnerIter>
operator + (const typename nested_iterator<OuterIter, InnerIter>::difference_type n, 
		const nested_iterator<OuterIter, InnerIter>& i) {
	return i + n;
}

template <class OuterIter, class InnerIter>
inline
nested_reverse_iterator<OuterIter, InnerIter>
operator + (const typename nested_reverse_iterator<OuterIter, InnerIter>::difference_type n, 
		const nested_reverse_iterator<OuterIter, InnerIter>& i) {
	return i + n;
}

//=============================================================================
}	// end namespace util

#endif	// __NESTED_ITERATOR_H__

