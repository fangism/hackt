// "hash_map_of_ptr.h"
// useful template for hash_map<K,T*>
// the alternative is to instantiate hash_map<auto_ptr<T>>...

// NOTE: assumes EXCLUSIVE ownwership of the pointers contained!
// any violation thereof will require something more powerful,
// such as reference counting.  

#ifndef __HASH_MAP_OF_PTR_H__
#define __HASH_MAP_OF_PTR_H__

#include "hash_map.h"		// wraps around <hash_map>

//=============================================================================
/// A hash_map of exclusively owned pointers, based on std::hash_map.  

/**
	The members of this class are pair<K,T*>, with K being the key type
	and T is the contained type.  The pointers should be exclusive, i.e., 
	whereas other copies of the same pointer may read or write
	the object, only this hash_map has responsibility for de-allocating
	the objects' memory.  This is generally useful for 
	look-up tables of owned references.  
 */
template <class K, class T>
class hash_map_of_ptr : public hash_map<K,T*> {
private:
	typedef hash_map<K,T*>				parent;
public:
	typedef	typename parent::iterator		iterator;
	typedef	typename parent::const_iterator		const_iterator;
protected:
	int			own;		///< ownership flag
public:

/// The default constructor just creates an empty hash_map.  
explicit hash_map_of_ptr() : parent(), own(1) { }
// copy constructor copies pointers without transferring ownership
	hash_map_of_ptr(const hash_map_of_ptr<K,T>& l);

/// The destructor frees memory to non-NULL pointers in the hash_map.  
virtual	~hash_map_of_ptr();		// don't want to inline this

/**
	Re-use parent's index operator.  Explicitly written here to 
	disambiguate the const version below, making both versions available
	to the public.  
	\param k the index key.
	\return the indexed pointer element.
 */
T*& operator [] (const K& k) {
	return parent::operator[](k);
}

const T* operator [] (const K& k) const;

// non-essential add-on methods
};	// end of class hash_map_of_ptr

//=============================================================================
/// A hash_map of read-only pointers, based on std::hash_map.  

/**
	The members of this class are pair<K, const T*>, 
	with K being the key type and T is the contained type.  
	The purpose for this class is to modify the semantics of the 
	[] operator, which normally allocates an index entry when
	an element is not found.  
	This version provides const semantics for the const [] operator.  
 */
template <class K, class T>
class hash_map_of_const_ptr : public hash_map<K, const T*> {
private:
	typedef hash_map<K, const T*>			parent;
public:
	typedef	typename parent::iterator		iterator;
	typedef	typename parent::const_iterator		const_iterator;
public:

/// The default constructor just creates an empty hash_map.  
explicit hash_map_of_const_ptr() : parent() { }
// copy constructor copies pointers without transferring ownership
	hash_map_of_const_ptr(const hash_map_of_const_ptr<K,T>& l);

/// The destructor frees memory to non-NULL pointers in the hash_map.  
virtual	~hash_map_of_const_ptr();		// don't want to inline this

/**
	Re-use parent's index operator.  Explicitly written here to 
	disambiguate the const version below, making both versions available
	to the public.  
	\param k the index key.
	\return the indexed pointer element.
 */
const T*& operator [] (const K& k) {
	return parent::operator[](k);
}

const T* operator [] (const K& k) const;

// non-essential add-on methods
};	// end of class hash_map_of_const_ptr

//=============================================================================

#endif	// __HASH_MAP_OF_PTR_H__

