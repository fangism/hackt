// "hashlist.h"
// a list with near-constant time lookup
// needed for when data needs to be kept in an ordered list
// with rapid acesss to its elements by a key

#ifndef	__HASHLIST_H__
#define	__HASHLIST_H__

#include "list_of_ptr.h"	// includes <list>
using std::list;

// different locations of hash_map, ARGH!
#if	(defined(__GNUC__) && (__GNUC__ >= 3))
		// works on Mac OS X gcc-3.3, linux gcc-3.2
#include <ext/hash_map>
using __gnu_cxx::hash_map;

#elif	(defined(__GNUC__) && (__GNUC__ <= 2))
		// works on FreeBSD gcc-2.95.3
#include <hash_map>
using std::hash_map;

#else		// default -- give up
DIE DIE DIE!!!
#endif


//=============================================================================
/**
	Derived from standard list<> but maintains a hash of pointers for
	constant time associative access.  
	Interface is limited to a few access methods.  
*/
template <class K, class T>
class hashlist : virtual protected list<T> {
public:
	typedef	typename list<T>::iterator		iterator;
	typedef	typename list<T>::const_iterator	const_iterator;
protected:
	typedef	hash_map<K,T*> 			map_type;

protected:
	/// maps a key to an iterator in the list
	map_type		itermap;
public:
	hashlist() : list<T>(), itermap() { }
virtual	~hashlist() { }

// we limit the interface to only a few methods
T*	append(const K&, const T&);		///< like push_back()
const T*	operator[] (const K& k) const;
T*	operator[] (const K& k);
// no pop_back();
using	list<T>::begin;
using	list<T>::end;

};

//=============================================================================
/**
	Class for storing a constant-time access list of owned pointers
	to objects.  
 */
template <class K, class T>
class hashlist_of_ptr : private list_of_ptr<T>, public hashlist<K,T*> {
public:
	typedef	typename list<T*>::iterator		iterator;
	typedef	typename list<T*>::const_iterator	const_iterator;
public:
	hashlist_of_ptr();		// not inlined
virtual	~hashlist_of_ptr();		// not inlined

using list_of_ptr<T>::begin;
using list_of_ptr<T>::end;

};

//=============================================================================

#endif	// __HASHLIST_H__

