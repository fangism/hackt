// "hash_map_of_ptr_template_methods.h"

#ifndef __HASH_MAP_OF_PTR_TEMPLATE_METHODS_H__
#define __HASH_MAP_OF_PTR_TEMPLATE_METHODS_H__

#include "hash_map_of_ptr.h"

//=============================================================================
// class hash_map_of_ptr method definitions

/// copy constructor with no transfer of ownership semantics
template <class K, class T>
hash_map_of_ptr<K,T>::hash_map_of_ptr(const hash_map_of_ptr<K,T>& l) : parent(l), own(0) {
}

template <class K, class T>
hash_map_of_ptr<K,T>::~hash_map_of_ptr() {
	if (own) {
		iterator i;
		for (i=begin(); i!=end(); i++)
			if ((*i).second)
				delete (*i).second;
	}
	clear();		// may not be necessary
}

/**
	Similar to hash_map<>::operator[], except this provides const
	semantics by checking that the entry referenced already exists, 
	and thus is never actually inserted as a new entry.  
	\param k the key into the hash_map.
	\return const copy of the indexed pointer element.
 */
template <class K, class T>
const T*
hash_map_of_ptr<K,T>::operator [] (const K& k) const {
	const_iterator i = find(k);
	return(i != end()) ? i->second : NULL;
}

//=============================================================================
// class hash_map_of_const_ptr method definitions

/// copy constructor
template <class K, class T>
hash_map_of_const_ptr<K,T>::hash_map_of_const_ptr(
	const hash_map_of_const_ptr<K,T>& l) :
		parent(l) {
}

template <class K, class T>
hash_map_of_const_ptr<K,T>::~hash_map_of_const_ptr() {
}

/**
	Similar to hash_map<>::operator[], except this provides const
	semantics by checking that the entry referenced already exists, 
	and thus is never actually inserted as a new entry.  
	\param k the key into the hash_map.
	\return const copy of the indexed pointer element.
 */
template <class K, class T>
const T*
hash_map_of_const_ptr<K,T>::operator [] (const K& k) const {
	const_iterator i = find(k);
	return(i != end()) ? i->second : NULL;
}

//=============================================================================

#endif	// __HASH_MAP_OF_PTR_TEMPLATE_METHODS_H__

