// "hashlist_template_methods.h"

#ifndef	__HASHLIST_TEMPLATE_METHODS_H__
#define	__HASHLIST_TEMPLATE_METHODS_H__

#include <iostream>
#include <string>

#include "hashlist.h"
#include "list_of_ptr_template_methods.h"

//=============================================================================
// template specializations

#if 1			// enable if compiler can't figure it out
namespace
#ifdef  __APPLE__
	__gnu_cxx
#else
	std
#endif
{
/**
	Explicit template specialization of hash of a string class, 
	which just uses the internal char* representation as a wrapper.
 */
template <>   
struct hash<string> {
	size_t operator() (const string& x) const {
		return hash<const char*>()(x.c_str());
	}
};	// end hash<>
};	// end namespace
#endif


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
	return itermap[k];
}

//=============================================================================
// class methods for hashlist_of_ptr<K,T>

/// Standard default constructor, sets ownership flag to true.
template <class K, class T>
hashlist_of_ptr<K,T>::hashlist_of_ptr<K,T>() : 
	list_of_ptr<T>(), hashlist<K,T*>() { }


//-----------------------------------------------------------------------------
/**
	Default destructor.  Since it is virtual, its parents' destructors
	will be properly invoked.  Defined here to prevent inlining.  
 */
template <class K, class T>
hashlist_of_ptr<K,T>::~hashlist_of_ptr<K,T>() { }

//=============================================================================

#endif	// __HASHLIST_TEMPLATE_METHODS_H__

