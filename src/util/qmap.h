/**
	\file "qmap.h"
	Queryable map with non-modifying (const) lookup method.  
	Non-modifying lookup guarantees that no extraneous empty
	elements are added by lookup.  
 */

#ifndef	__QMAP_H__
#define	__QMAP_H__

#include <map>
#include "qmap_fwd.h"		// forward declarations only

namespace QMAP_NAMESPACE {
using std::map;
//-----------------------------------------------------------------------------
/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
 */
template <class K, class T>
class qmap : public map<K,T> {
private:
	typedef	map<K,T>			parent;
public:
        typedef typename parent::key_type		key_type;
        typedef typename parent::mapped_type		mapped_type;   
        typedef typename parent::value_type		value_type;
        typedef typename parent::key_compare		key_compare;
	typedef typename parent::allocator_type		allocator_type;

	typedef typename parent::reference		reference;
	typedef typename parent::const_reference	const_reference;
	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
	typedef typename parent::reverse_iterator	reverse_iterator;
	typedef typename parent::const_reverse_iterator	const_reverse_iterator; 
	typedef typename parent::size_type		size_type;
	typedef typename parent::difference_type	difference_type;
	typedef typename parent::pointer		pointer;
	typedef typename parent::const_pointer		const_pointer;
	typedef typename parent::allocator_type		allocator_type;

public:
	/** Default constructor. */
	qmap() : map<K,T>() { }

	/** Default destructor. */
	~qmap() { }

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
	T& operator [] (const K& k) { return map<K,T>::operator[](k); }

	/**
		Constant query associative lookup.  
		Since it's constant, it won't modify the map
		by making a blank entry at the given key, 
		which is what map<K,T> does.  
		\param k the key used to lookup.  
		\return read-only COPY of the object if found, 
			else a freshly constructed object, 
			such as a wrapper NULL pointer.  
	 */
	T operator [] (const K& k) const {
		const_iterator i = find(k);	// uses find() const;
		return (i != this->end()) ? i->second : T();
		// if T is a pointer class, should be equivalent to NULL
		// or whatever the default constructor is
	}

	/**
		For all entries whose value is the default, remove them.  
	 */
	void clean(void) {
		const T def;	// default value
#if 0
		// won't work because of pair<> b.s.
		remove_if(index_map.begin(), index_map.end(),
			unary_compose(     
				bind2nd(equal_to<T>(), def),       
				_Select2nd<typename map_type::value_type>()
				// some error involving operator =
				// with const first type.
			)
		);
#else
		iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; ) {
			if (i->second == def) {
				iterator j = i;
				j++;
				this->erase(i);
				i = j;
			} else {
				i++;
			}
		}
#endif
	}

};	// end class qmap

//-----------------------------------------------------------------------------

#ifndef NULL
#define NULL    0
#endif

/**
	Specialization for qmap that contains bare pointers.  
	Since pointers, by default, are not initialized, 
	this ensures that NULL is returned for pointers not found.  
	This class could specialize map directly, 
	and need not only apply to qmap.
	NOTE: that this make it impossible to distinguish between
	a pointer whose value is actually NULL and a pointer that doesn't 
	exist in the map.  
	The clean() function removes NULL pointers from the hash-map.  
 */
template <class K, class T>
class qmap<K,T*> : public map<K,T*> {
private:
	typedef map<K,T*>				parent;
public:
	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
public:
	// use default constructor and destructors

	T*& operator [] (const K& k) { return parent::operator[](k); }

	T* operator [] (const K& k) const {
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

};      // and class qmap specialization

//-----------------------------------------------------------------------------
}	// end namespace QMAP_NAMESPACE

#endif	//	__QMAP_H__

