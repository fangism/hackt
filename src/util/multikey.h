/**
	\file "multikey.h"
	Multidimensional key class, use to emulate true multiple dimensions
	with a standard map class.
	$Id: multikey.h,v 1.18.4.2 2005/01/20 22:00:15 fang Exp $
 */

#ifndef	__MULTIKEY_H__
#define	__MULTIKEY_H__

#include "macros.h"
#include "multikey_fwd.h"

#include "STL/reverse_iterator.h"
#include <valarray>
#include <iosfwd>

#define	BASE_MULTIKEY_TEMPLATE_SIGNATURE				\
template <class K>

#define	MULTIKEY_TEMPLATE_SIGNATURE					\
template <size_t D, class K>

#define MULTIKEY_GENERIC_TEMPLATE_SIGNATURE				\
template <class K>

#define	BASE_MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE			\
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
/**
	Abstract interface for an N-dimensional key.  
 */
BASE_MULTIKEY_TEMPLATE_SIGNATURE
class multikey_base {
public:
	typedef	K				value_type;
	typedef	K*				iterator;
	typedef	const K*			const_iterator;
	typedef	std::reverse_iterator<iterator>	reverse_iterator;
	typedef	std::reverse_iterator<const_iterator>
						const_reverse_iterator;
	typedef	K&				reference;
	typedef	const K&			const_reference;
public:
	static const size_t			LIMIT = 4;
public:
virtual	~multikey_base() { }
virtual	size_t
	dimensions(void) const = 0;

	size_t
	size(void) const { return this->dimensions(); }

virtual	K
	default_value(void) const = 0;

virtual	iterator
	begin(void) = 0;

virtual	const_iterator
	begin(void) const = 0;

virtual	iterator
	end(void) = 0;

virtual	const_iterator
	end(void) const = 0;

virtual	reverse_iterator
	rbegin(void) = 0;

virtual	const_reverse_iterator
	rbegin(void) const = 0;

virtual	reverse_iterator
	rend(void) = 0;

virtual	const_reverse_iterator
	rend(void) const = 0;

	reference
	front(void) { return *begin(); }

	const_reference
	front(void) const { return *begin(); }

	reference
	back(void) {
		iterator e = end();
		return *(--e);
	}

	const_reference
	back(void) const {
		const_iterator e = end();
		return *(--e);
	}

#if 0
virtual	multikey_base<K>&
	operator = (const multikey_base<K>& k) = 0;
#endif

virtual	reference
	operator [] (const size_t i) = 0;

virtual	const_reference
	operator [] (const size_t i) const = 0;

static	multikey_base<K>*
	make_multikey(const size_t d);

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
MULTIKEY_TEMPLATE_SIGNATURE
class multikey : virtual public multikey_base<K> {
	template <size_t, class C>
	friend class multikey;
public:
	typedef	K					value_type;
	typedef	multikey_base<K>			base_type;
	typedef	multikey<D,K>				this_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
	typedef	typename base_type::reverse_iterator	reverse_iterator;
	typedef	typename base_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	typename base_type::reference		reference;
	typedef	typename base_type::const_reference	const_reference;

#if 0
public:
	static const this_type				ones;
#endif
public:
	K indices[D];

public:
	/**
		Default constructor.  
		Requires key type K to be assignable and copiable.  
		\param i the default value with which to fill indices.  
			Plain-old-data types will default to 0.
	 */
	multikey(const K i = K());

	/**
		Copy constructor compatible with other dimensions.  
		If this is larger than argument, fill remaining
		dimensions with 0.  
	 */
	template <size_t D2>
	multikey(const multikey<D2,K>& k, const K i = K());

#if 0
	multikey(const multikey_base<K>& k);
#endif

	/**
		\param S is sequence type container.
		\param s is const reference to sequence object.
	 */
	template <template <class> class S>
	explicit
	multikey(const S<K>& s, const K i = K());

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

	/**
		Generic assignment operation.
	 */
	multikey_base<K>&
	operator = (const multikey_base<K>& s);

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
class multikey_generic : virtual public multikey_base<K>, public valarray<K> {
protected:
	typedef	multikey_base<K>			interface_type;
	typedef	valarray<K>				impl_type;
	typedef	multikey_generic<K>			this_type;
public:
	typedef	typename interface_type::value_type	value_type;
	typedef	typename interface_type::iterator	iterator;
	typedef	typename interface_type::const_iterator	const_iterator;
	typedef	typename interface_type::reverse_iterator
							reverse_iterator;
	typedef	typename interface_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	K&					reference;
	typedef	const K&				const_reference;
public:
	multikey_generic() : interface_type(), impl_type() { }

	multikey_generic(const impl_type& m) :
		interface_type(), impl_type(m) { }

	multikey_generic(const size_t d, const size_t i = 0) :
		interface_type(), impl_type(i, d) { }
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

	multikey_base<K>&
	operator = (const multikey_base<K>& k);

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

BASE_MULTIKEY_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const multikey_base<K>& k);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k);
#endif

//=============================================================================
/**
	Interface for generating cyclic keys.  
 */
template <class K>
class multikey_generator_base : virtual public multikey_base<K> {
public:
	typedef	K					value_type;
	typedef	multikey_base<K>			base_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
	typedef	typename base_type::reverse_iterator	reverse_iterator;
	typedef	typename base_type::const_reverse_iterator
							const_reverse_iterator;
public:
virtual	~multikey_generator_base() { }

virtual	void
	validate(void) const = 0;

virtual	void
	initialize(void) = 0;

virtual	size_t
	size(void) const = 0;

virtual	iterator
	begin(void) = 0;

virtual	const_iterator
	begin(void) const = 0;

virtual	iterator
	end(void) = 0;

virtual	const_iterator
	end(void) const = 0;

virtual	reverse_iterator
	rbegin(void) = 0;

virtual	const_reverse_iterator
	rbegin(void) const = 0;

virtual	reverse_iterator
	rend(void) = 0;

virtual	const_reverse_iterator
	rend(void) const = 0;


virtual multikey_base<K>&
	get_lower_corner(void) = 0;

virtual const multikey_base<K>&
	get_lower_corner(void) const = 0;

virtual multikey_base<K>&
	get_upper_corner(void) = 0;

virtual const multikey_base<K>&
	get_upper_corner(void) const = 0;

virtual	multikey_base<K>&
	operator ++ (int) = 0;

	static
	multikey_generator_base<K>*
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
	typedef	K					value_type;
	typedef	multikey_generator_base<K>		interface_type;
	typedef	multikey<D,K>				base_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
	typedef	typename base_type::reverse_iterator	reverse_iterator;
	typedef	typename base_type::const_reverse_iterator
							const_reverse_iterator;
// protected:
public:		// for sake of laziness and convenience
	/** vector of lower bounds */
	base_type		lower_corner;
	/** vector of upper bounds */
	base_type		upper_corner;
public:
	/// default constructor
	multikey_generator() : base_type(), interface_type(), 
		lower_corner(), upper_corner() { }

	// construct from a pair of keys
	multikey_generator(const base_type& l, const base_type& u) :
		base_type(), interface_type(), 
		lower_corner(l), upper_corner(u) { }

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
	operator ++ (int);

	// all other methods inherited

};	// end class multikey_generator

//-----------------------------------------------------------------------------
/**
	Key-generator with run-time dimensions.  
 */
template <class K>
class multikey_generator_generic : public multikey_generic<K>, 
		public multikey_generator_base<K> {
public:
	typedef	K					value_type;
	typedef	multikey_generator_base<K>		interface_type;
	typedef	multikey_generic<K>			base_type;
	typedef	typename base_type::iterator		iterator;
	typedef	typename base_type::const_iterator	const_iterator;
	typedef	typename base_type::reverse_iterator	reverse_iterator;
	typedef	typename base_type::const_reverse_iterator
							const_reverse_iterator;
// protected:
public:		// for sake of laziness and convenience
	/** vector of lower bounds */
	base_type		lower_corner;
	/** vector of upper bounds */
	base_type		upper_corner;
public:
	/// default constructor
	multikey_generator_generic() : base_type(), interface_type(), 
			lower_corner(), upper_corner() { }

	multikey_generator_generic(const size_t d) :
			base_type(d), interface_type(), 
			lower_corner(d), upper_corner(d) { }

	// construct from a pair of keys
	multikey_generator_generic(const base_type& l, const base_type& u) :
			base_type(l.dimensions()), interface_type(), 
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

	multikey_base<K>&
	get_lower_corner(void) { return lower_corner; }

	const multikey_base<K>&
	get_lower_corner(void) const { return lower_corner; }

	multikey_base<K>&
	get_upper_corner(void) { return upper_corner; }

	const multikey_base<K>&
	get_upper_corner(void) const { return upper_corner; }

	K&
	operator [] (size_t i) { return base_type::operator[](i); }

	const K&
	operator [] (size_t i) const { return base_type::operator[](i); }

	// can be used directly as key, no need to convert
	multikey_base<K>&
	operator ++ (int);

	// all other methods inherited

};	// end class multikey_generator_generic

//-----------------------------------------------------------------------------
#if 0
template <class K>
ostream&
operator << (ostream& o, const multikey_generator_base<K>& k);
#endif

//=============================================================================
}	// end namespace MULTIKEY_NAMESPACE



#endif	//	__MULTIKEY_H__

