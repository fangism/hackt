/**
	\file "hash_qmap.h"
	Queryable hash_map that doesn't add empty elements on lookup.  
	$Id: hash_qmap.h,v 1.5 2004/11/02 07:52:14 fang Exp $
 */

#ifndef	__HASH_QMAP_H__
#define	__HASH_QMAP_H__

#ifndef	NULL
#define	NULL	0
#endif

#include "hash_qmap_fwd.h"		// forward declarations only

namespace HASH_QMAP_NAMESPACE {
using namespace HASH_MAP_NAMESPACE;

//-----------------------------------------------------------------------------
/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
	\param K the key type.
	\param T the mapped data type.
	\param H the hashing function.
	\param E the equal_to comparator function.
	\param A the allocator.  
 */
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap : public hash_map<K,T,H,E,A> {
private:
	typedef hash_map<K,T,H,E,A>			parent;
public:
	/// key type is K
	typedef typename parent::key_type		key_type;
	/// data type is T
	typedef typename parent::data_type		data_type;
	/// mapped type is T
	typedef typename parent::mapped_type		mapped_type;
	/// value_type is pair<const K, T>
	typedef typename parent::value_type		value_type;

	typedef typename parent::hasher			hasher;
	typedef typename parent::key_equal		key_equal;
	typedef typename parent::allocator_type		allocator_type;

	/// this is not T&, depends on implementation type (hashtable)
	typedef typename parent::reference		reference;
	/// this is not const T&, depends on implementation type (hashtable)
	typedef typename parent::const_reference	const_reference;

	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
	typedef typename parent::size_type		size_type;
	typedef typename parent::difference_type	difference_type;
	typedef typename parent::pointer		pointer;
	typedef typename parent::const_pointer		const_pointer;
	typedef typename parent::allocator_type		allocator_type;

	// note: no reverse iterator

public:
	/** Default constructor. */
	hash_qmap() : parent() { }

	/** Default destructor. */
	~hash_qmap() { }

	/**
		Just wrapper to the parent's lookup operation, 
		which modifies the map even when querying for a
		non-existent entry.  
		Explicitly written here so the const version below
		doesn't overshadow the parent's non-const version.  
		\param k the key used to lookup.  
		\return modifiable reference to the object if found, 
			else a freshly constructed object.  
	 */
	mapped_type&
	operator [] (const key_type& k) { return parent::operator[](k); }

	/**
		Constant query associative lookup.  
		Since it's constant, it won't modify the map
		by making a blank entry at the given key, 
		which is what map<K,T> does.  
		IMPORTANT: just using the [] operator on a map
		will default to the non-const version.  
		To disambiguate, thus forcing use of this const-preserving
		lookup, one must explicitly static_cast the invoking object.
		e.g. static_cast<const map_type&>(map_name)[key]
		\param k the key used to lookup.  
		\return COPY of the object if found, 
			else a freshly constructed object.  
			Can't use a reference because might return
			reference to a temporary.  
	 */
	mapped_type
	operator [] (const key_type& k) const {
		const_iterator i = find(k);	// uses find() const;
		return (i != this->end()) ? i->second : mapped_type();
		// if T is a pointer class, should be equivalent to NULL
		// or whatever the default constructor is
	}

};	// end class hash_qmap

//-----------------------------------------------------------------------------
/**
	Specialization for hash_qmap that contains bare pointers.  
	Since pointers, by default, are not initialized, 
	this ensures that NULL is returned for pointers not found.  
	This class could specialize hash_map directly, 
	and need not only apply to hash_qmap.
	NOTE: that this make it impossible to distinguish between
	a pointer whose value is actually NULL and a pointer that doesn't 
	exist in the map.  
	The clean() function removes NULL pointers from the hash-map.  
 */
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap<K,T*,H,E,A> : public hash_map<K,T*,H,E,A> {
private:
	typedef hash_map<K,T*,H,E,A>			parent;
public:
	/// key type is K
	typedef typename parent::key_type		key_type;
	/// data type is T
	typedef typename parent::data_type		data_type;
	/// mapped type is T
	typedef typename parent::mapped_type		mapped_type;
	/// value_type is pair<const K, T>
	typedef typename parent::value_type		value_type;

	typedef typename parent::hasher			hasher;
	typedef typename parent::key_equal		key_equal;
	typedef typename parent::allocator_type		allocator_type;

	typedef typename parent::reference		reference;
	typedef typename parent::const_reference	const_reference;
	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
	typedef typename parent::size_type		size_type;
	typedef typename parent::difference_type	difference_type;
	typedef typename parent::pointer		pointer;
	typedef typename parent::const_pointer		const_pointer;
	typedef typename parent::allocator_type		allocator_type;

	// note: no reverse iterator

public:
	// use default constructor and destructors

	/**
		\return *reference* to pointer (T*&) with key K.  
	 */
	mapped_type&
	operator [] (const key_type& k) { return parent::operator[](k); }

	/**
		\return *copy* of pointer (T*) with key K.  
	 */
	mapped_type
	operator [] (const key_type& k) const {
		const_iterator i = find(k);	// uses find() const;
		return (i != this->end()) ? i->second : NULL;
		// if T is a pointer class, should be equivalent to NULL
		// or whatever the default constructor is
	}

	/**
		For all entries whose value is the default, remove them.
	 */
	void clean(void) {
		iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; ) {
			if (i->second == NULL) {
				iterator j = i;
				j++;
				this->erase(i);
				i = j;
			} else {
				i++;
			}
		}
	}
};	// and class hash_qmap specialization

//-----------------------------------------------------------------------------

}	// end namespace HASH_QMAP_NAMESPACE

#endif	//	__HASH_QMAP_H__

