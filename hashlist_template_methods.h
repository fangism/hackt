// "hashlist_template_methods.h"

#ifndef	__HASHLIST_TEMPLATE_METHODS_H__
#define	__HASHLIST_TEMPLATE_METHODS_H__

#include <iostream>

#include "hashlist.h"

// phased out pointer specializations
// #include "list_of_ptr_template_methods.h"	// obsolete

//=============================================================================
// class methods for hashlist<K,T>

/**
	This is the replacement for push_back(), which now maintains
	a hash map from some key to individual objects in the list
	for constant time associative access.  
	\param k the key with with to hash the entry.  
	\param t the object to be stored in the list, sequentially.
	\return pointer to what was originally at the hashed location, 
		which will be NULL if it was empty before
 */
template <class K, class T>
T*
hashlist<K,T>::
append(const K& k, const T& x) {
	iterator i;
	T* old;
	push_back(x);
	i = end();
	i--;
	old = itermap[k];
	itermap[k] = &(*i);
	return old;
}

//-----------------------------------------------------------------------------
/**
	Associative index operator.  
	\param k the hash key.
	\return an address to stored object in list, 
		will be NULL if not found
 */
template <class K, class T>
inline
T*
hashlist<K,T>::operator [] (const K& k) {
	return itermap[k];
}

//-----------------------------------------------------------------------------
/**
	Associative index operator with constant (non-modify) semantics.  
	\param k the hash key.
	\return an address to stored object in list, 
		will be NULL if not found
 */
template <class K, class T>
inline
const T*
hashlist<K,T>::operator [] (const K& k) const {
	typename map_type::const_iterator i = itermap.find(k);
	return (i != itermap.end()) ? i->second : NULL;
//	return itermap[k];		// discards qualifiers
}

//=============================================================================

#endif	// __HASHLIST_TEMPLATE_METHODS_H__

