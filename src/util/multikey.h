/**
	\file "multikey.h"
	Multidimensional key class, use to emulate true multiple dimensions
	with a standard map class.
	$Id: multikey.h,v 1.19.10.4 2005/02/06 21:31:07 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_H__
#define	__UTIL_MULTIKEY_H__

#include "macros.h"
#include "multikey_fwd.h"

#include "STL/reverse_iterator.h"
#include <valarray>
#include <iosfwd>

#define	MULTIKEY_TEMPLATE_SIGNATURE					\
template <size_t D, class K>

#define MULTIKEY_GENERIC_TEMPLATE_SIGNATURE				\
template <class K>

#define	MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE				\
template <size_t D, class K>

#define	MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE			\
template <class K>

/***
	Later be able to compare between keys of different dimensions.  
***/
namespace MULTIKEY_NAMESPACE {
using std::valarray;
using std::pair;
using std::ostream;
using std::istream;

//=============================================================================
template <class K>
struct multikey_traits {
	typedef	K					value_type;
	typedef	K&					reference;
	typedef	const K&				const_reference;
	typedef	K*					iterator;
	typedef	const K*				const_iterator;
	typedef	std::reverse_iterator<iterator>		reverse_iterator;
	typedef	std::reverse_iterator<const_iterator>	const_reverse_iterator;
};	// end struct multikey_traits

//=============================================================================
/**
	Default underlying implementation of multikey.
	Intended for dimensions > 1.
 */
MULTIKEY_TEMPLATE_SIGNATURE
class multikey_implementation_base {
private:
	typedef	multikey_implementation_base<D,K>	this_type;
protected:
	typedef	multikey_traits<K>			traits_type;
public:
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::reference		reference;
	typedef	typename traits_type::const_reference	const_reference;
	typedef	typename traits_type::iterator		iterator;
	typedef	typename traits_type::const_iterator	const_iterator;
	typedef	typename traits_type::reverse_iterator	reverse_iterator;
	typedef	typename traits_type::const_reverse_iterator
							const_reverse_iterator;

protected:
	K	indices[D];

public:
	multikey_implementation_base() {
		std::fill(this->begin(), this->end(), K());
	}

	multikey_implementation_base(const this_type& k) {
		std::copy(k.begin(), k.end(), this->begin());
	}

	size_t
	dimensions(void) const { return D; }

	K
	default_value(void) const { return K(); }

	iterator
	begin(void) { return &indices[0]; }

	const_iterator
	begin(void) const { return &indices[0]; }

	iterator
	end(void) { return &indices[D]; }

	const_iterator
	end(void) const { return &indices[D]; }

	reverse_iterator
	rbegin(void) { return reverse_iterator(&indices[D]); }

	const_reverse_iterator
	rbegin(void) const { return const_reverse_iterator(&indices[D]); }

	reverse_iterator
	rend(void) { return reverse_iterator(&indices[0]); }

	const_reverse_iterator
	rend(void) const { return const_reverse_iterator(&indices[0]); }

	reference
	front(void) { return *begin(); }

	const_reference
	front(void) const { return *begin(); }

	reference
	back(void) { return *rbegin(); }

	const_reference
	back(void) const { return *rbegin(); }

	/**
		Safe indexing with array-bound check.  
		indices is public, so one can always access it directly...
	 */
	reference
	operator [] (const size_t i) {
		INVARIANT(i < D);
		return indices[i];
	}

	/**
		Const version of array indexing.  
	 */
	const_reference
	operator [] (const size_t i) const {
		INVARIANT(i < D);
		return indices[i];
	}

};	// end class multikey_implementation_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization of size 1 multikey, reduced to single element.  
 */
template <class K>
class multikey_implementation_base<1,K> {
private:
	typedef	multikey_implementation_base<1,K>	this_type;
protected:
	typedef	multikey_traits<K>			traits_type;
public:
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::reference		reference;
	typedef	typename traits_type::const_reference	const_reference;
	typedef	typename traits_type::iterator		iterator;
	typedef	typename traits_type::const_iterator	const_iterator;
	typedef	typename traits_type::reverse_iterator	reverse_iterator;
	typedef	typename traits_type::const_reverse_iterator
							const_reverse_iterator;

protected:
	K	index;

public:
	multikey_implementation_base() : index(0) { }

	multikey_implementation_base(const this_type& k) : index(k.index) { }

	size_t
	dimensions(void) const { return 1; }

	K
	default_value(void) const { return K(); }

	iterator
	begin(void) { return &index; }

	const_iterator
	begin(void) const { return &index; }

	iterator
	end(void) { return &index +1; }

	const_iterator
	end(void) const { return &index +1; }

	reverse_iterator
	rbegin(void) { return reverse_iterator(&index +1); }

	const_reverse_iterator
	rbegin(void) const { return const_reverse_iterator(&index +1); }

	reverse_iterator
	rend(void) { return reverse_iterator(&index); }

	const_reverse_iterator
	rend(void) const { return const_reverse_iterator(&index); }

	reference
	front(void) { return index; }

	const_reference
	front(void) const { return index; }

	reference
	back(void) { return index; }

	const_reference
	back(void) const { return index; }

	/**
		Convenient implicit conversion to plain scalar type.  
		Non-const reference.  
	 */
	operator K& () { return index; }

	/**
		Convenient implicit conversion to plain scalar type.  
		Const reference.  
	 */
	operator const K& () const { return index; }

	/**
		Safe indexing with array-bound check.  
		indices is public, so one can always access it directly...
		\pre i must be 0.
	 */
	reference
	operator [] (const size_t i) {
		INVARIANT(!i);
		return index;
	}

	/**
		Const version of array indexing.  
		\pre i must be 0.
	 */
	const_reference
	operator [] (const size_t i) const {
		INVARIANT(!i);
		return index;
	}

};	// end class multikey_implementation_base specialization

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
MULTIKEY_TEMPLATE_SIGNATURE
class multikey : public multikey_implementation_base<D,K> {
	template <size_t, class C>
	friend class multikey;

	typedef	multikey<D,K>				this_type;
	typedef	multikey_implementation_base<D,K>	impl_type;
public:
	typedef	typename impl_type::value_type		value_type;
	typedef	typename impl_type::reference		reference;
	typedef	typename impl_type::const_reference	const_reference;
	typedef	typename impl_type::iterator		iterator;
	typedef	typename impl_type::const_iterator	const_iterator;
	typedef	typename impl_type::reverse_iterator	reverse_iterator;
	typedef	typename impl_type::const_reverse_iterator
							const_reverse_iterator;

#if 0
public:
	static const this_type				ones;
#endif

public:
	/**
		Default constructor.  
		Requires key type K to be assignable and copiable.  
		\param i the default value with which to fill indices.  
			Plain-old-data types will default to 0.
	 */
	explicit
	multikey(const K i = K());

	explicit
	multikey(const multikey_generic<K>& k);

	/// copy-constructor
	multikey(const this_type& k) : impl_type(k) { }

	/**
		Copy constructor compatible with other dimensions.  
		If this is larger than argument, fill remaining
		dimensions with 0.  
	 */
	template <size_t D2>
	multikey(const multikey<D2,K>& k, const K i = K());

	/**
		\param S is sequence type container.
		\param s is const reference to sequence object.
	 */
	template <template <class> class S>
	explicit
	multikey(const S<K>& s, const K i = K());

	this_type&
	operator = (const this_type& s);

	this_type&
	operator += (const this_type& k);

	this_type&
	operator -= (const this_type& k);

	this_type
	operator + (const this_type& k) const {
		this_type ret(*this);
		return ret += k;
	}

	this_type
	operator - (const this_type& k) const {
		this_type ret(*this);
		return ret -= k;
	}

	/**
		Helper class for finding index extremities in sets.  
	 */
	struct accumulate_extremities {
		typedef pair<this_type, this_type>	key_pair;
		this_type		min, max;

		accumulate_extremities() : min(), max() { }

		explicit
		accumulate_extremities(const multikey<D,K>& i) :
			min(i), max(i) { }
	private:
		// has trouble finding std::min/max in stl_algobase... grrr...
		static 
		K mymin(const K& a, const K& b);
		static 
		K mymax(const K& a, const K& b);
	public:
		void
		operator () (const multikey<D,K>& k);

		key_pair
		operator () (const key_pair& a, const multikey<D,K>& b);

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

public:
	// IO methods
	/**
		Do not implement from a virtual base.
		We must statically know the dimension of the key
		at-compile time.
	 */
	ostream&
	write(ostream& o) const;

	/**
		Do not implement from a virtual base.
		We must statically know the dimension of the key
		at-compile time.
	 */
	istream&
	read(istream& f);

};	// end class multikey

//-----------------------------------------------------------------------------
#if 0
MULTIKEY_TEMPLATE_SIGNATURE
const multikey<D,K>
multikey<D,K>::ones = multikey<D,K,1>();
#endif

//=============================================================================
/**
	Generic variable-dimension multikey, implemented as a valarray.
	\param K the key type, usually integer-like.
 */
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
class multikey_generic : public valarray<K> {
protected:
	typedef	valarray<K>				impl_type;
	typedef	multikey_generic<K>			this_type;
public:
	typedef	K*					iterator;
	typedef	const K*				const_iterator;
	typedef	std::reverse_iterator<iterator>		reverse_iterator;
	typedef	std::reverse_iterator<const_iterator>
							const_reverse_iterator;
	typedef	K&					reference;
	typedef	const K&				const_reference;
public:
	multikey_generic() : impl_type() { }

	multikey_generic(const impl_type& m) : impl_type(m) { }

	multikey_generic(const size_t d, const K i = 0) : impl_type(i, d) { }
	// valarray constructor: 1st param is fill-value, 2nd param is size

	template <size_t D>
	multikey_generic(const multikey<D,K>& m);

	~multikey_generic() { }

	using impl_type::size;

	size_t
	dimensions(void) const { return size(); }

	K
	default_value(void) const { return 0; }

	iterator
	begin(void) { return &impl_type::operator[](0); }

	const_iterator
	begin(void) const { return &impl_type::operator[](0); }

	iterator
	end(void) { return &impl_type::operator[](size()); }

	const_iterator
	end(void) const { return &impl_type::operator[](size()); }

	reverse_iterator
	rbegin(void)
		{ return reverse_iterator(&impl_type::operator[](size())); }

	const_reverse_iterator
	rbegin(void) const {
		return const_reverse_iterator(&impl_type::operator[](size()));
	}

	reverse_iterator
	rend(void) { return reverse_iterator(&impl_type::operator[](0)); }

	const_reverse_iterator
	rend(void) const {
		return const_reverse_iterator(&impl_type::operator[](0));
	}

	reference
	front(void) { return *begin(); }

	const_reference
	front(void) const { return *begin(); }

	reference
	back(void) { return *rbegin(); }

	const_reference
	back(void) const { return *rbegin(); }

	reference
	operator [] (const size_t i) {
		return impl_type::operator[](i);
	}

	const_reference
	operator [] (const size_t i) const {
		return impl_type::operator[](i);
	}

	bool
	operator == (const this_type&) const;

	bool
	operator < (const this_type&) const;

	bool
	operator > (const this_type& m) const {
		return m < *this;
	}

	bool
	operator != (const this_type& m) const {
		return !(*this == m);
	}

	bool
	operator <= (const this_type& m) const {
		return !(*this > m);
	}

	bool
	operator >= (const this_type& m) const {
		return !(*this < m);
	}

	this_type
	operator + (const this_type& m) const;

	this_type
	operator - (const this_type& m) const;

	ostream&
	write(ostream&) const;

	istream&
	read(istream&);

};	// end class multikey_generic

//=============================================================================
MULTIKEY_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const multikey<D,K>& k);

MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const multikey_generic<K>& k);

//=============================================================================
/**
	Only works for integer-like keys.  
	Extension of a standard multikey, with knowledge of bounds.  
 */
template <size_t D, class K>
class multikey_generator : public multikey<D,K> {
public:
	typedef	K					value_type;
	typedef	multikey<D,K>				base_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
	typedef	typename base_type::reverse_iterator	reverse_iterator;
	typedef	typename base_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	base_type				corner_type;
// protected:
public:		// for sake of laziness and convenience
	/** vector of lower bounds */
	corner_type		lower_corner;
	/** vector of upper bounds */
	corner_type		upper_corner;
public:
	/// default constructor
	multikey_generator() : base_type(), lower_corner(), upper_corner() { }

	// construct from a pair of keys
	multikey_generator(const base_type& l, const base_type& u) :
		base_type(), lower_corner(l), upper_corner(u) { }

	/// copy from a sequence of pairs
	template <template <class> class L, template <class, class> class P>
	explicit
	multikey_generator(const L<P<K,K> >& l);

	/**
		\param LP is a list-of-pairs-like class.  
	 */
	template <class LP>
	explicit
	multikey_generator(const LP& l);

	// use default destructor

	/**
		Make sure bounds are sane.  
	 */
	void
	validate(void) const;

	void
	initialize(void);

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

	reverse_iterator
	rbegin(void) { return base_type::rbegin(); }

	const_reverse_iterator
	rbegin(void) const { return base_type::rbegin(); }

	reverse_iterator
	rend(void) { return base_type::rend(); }

	const_reverse_iterator
	rend(void) const { return base_type::rend(); }

	corner_type&
	get_lower_corner(void) { return lower_corner; }

	const corner_type&
	get_lower_corner(void) const { return lower_corner; }

	corner_type&
	get_upper_corner(void) { return upper_corner; }

	const corner_type&
	get_upper_corner(void) const { return upper_corner; }

	// can be used directly as key, no need to convert
	corner_type&
	operator ++ (int);

	// all other methods inherited

};	// end class multikey_generator

//-----------------------------------------------------------------------------
/**
	Key-generator with run-time dimensions.  
 */
template <class K>
class multikey_generator_generic : public multikey_generic<K> {
public:
	typedef	K					value_type;
	typedef	multikey_generic<K>			base_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
	typedef	typename base_type::reverse_iterator	reverse_iterator;
	typedef	typename base_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	base_type				corner_type;
// protected:
public:		// for sake of laziness and convenience
	/** vector of lower bounds */
	corner_type		lower_corner;
	/** vector of upper bounds */
	corner_type		upper_corner;
public:
	/// default constructor
	multikey_generator_generic() :
		base_type(), lower_corner(), upper_corner() { }

	multikey_generator_generic(const size_t d) :
		base_type(d), lower_corner(d), upper_corner(d) { }

	// construct from a pair of keys
	multikey_generator_generic(const base_type& l, const base_type& u) :
			base_type(l.dimensions()), 
			lower_corner(l), upper_corner(u) {
		INVARIANT(l.dimensions() == u.dimensions());
	}

	/// copy from a sequence of pairs
	template <template <class> class L, template <class, class> class P>
	explicit
	multikey_generator_generic(const L<P<K,K> >& l);

	/**
		\param LP is a list-of-pairs-like class.  
	 */
	template <class LP>
	explicit
	multikey_generator_generic(const LP& l);

	// use default destructor

	/**
		Make sure bounds are sane.  
	 */
	void
	validate(void) const;

	void
	initialize(void);

	size_t
	size(void) const { return base_type::dimensions(); }

	size_t
	dimensions(void) const { return base_type::dimensions(); }

	value_type
	default_value(void) const { return value_type(); }

	iterator
	begin(void) { return base_type::begin(); }

	const_iterator
	begin(void) const { return base_type::begin(); }

	iterator
	end(void) { return base_type::end(); }

	const_iterator
	end(void) const { return base_type::end(); }

	reverse_iterator
	rbegin(void) { return base_type::rbegin(); }

	const_reverse_iterator
	rbegin(void) const { return base_type::rbegin(); }

	reverse_iterator
	rend(void) { return base_type::rend(); }

	const_reverse_iterator
	rend(void) const { return base_type::rend(); }

	corner_type&
	get_lower_corner(void) { return lower_corner; }

	const corner_type&
	get_lower_corner(void) const { return lower_corner; }

	corner_type&
	get_upper_corner(void) { return upper_corner; }

	const corner_type&
	get_upper_corner(void) const { return upper_corner; }

	K&
	operator [] (size_t i) { return base_type::operator[](i); }

	const K&
	operator [] (size_t i) const { return base_type::operator[](i); }

	// can be used directly as key, no need to convert
	corner_type&
	operator ++ (int);

	// all other methods inherited

};	// end class multikey_generator_generic

//=============================================================================
}	// end namespace MULTIKEY_NAMESPACE



#endif	// __UTIL_MULTIKEY_H__

