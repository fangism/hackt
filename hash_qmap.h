// "hash_qmap.h"

#ifndef	__HASH_QMAP_H__
#define	__HASH_QMAP_H__

#include "hash_map.h"			// version independent
// using directive should be in the below namespace...

#ifndef	HASH_QMAP_NAMESPACE
#define	HASH_QMAP_NAMESPACE		fang
#endif

namespace HASH_QMAP_NAMESPACE {

/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
 */
template <class K, class T>
class hash_qmap : public hash_map<K,T> {
private:  
	typedef hash_map<K,T>				parent;
public:
	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
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
	T& operator [] (const K& k) { return parent::operator[](k); }

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
	T operator [] (const K& k) const {
		const_iterator i = find(k);	// uses find() const;
		return (i != this->end()) ? i->second : T();
		// if T is a pointer class, should be equivalent to NULL
		// or whatever the default constructor is
	}

};	// end class hash_qmap

}	// end namespace

#endif	//	__HASH_QMAP_H__

