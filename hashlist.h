// "hashlist.h"
// a list with near-constant time lookup
// needed for when data needs to be kept in an ordered list
// with rapid acesss to its elements by a key

#ifndef	__HASHLIST_H__
#define	__HASHLIST_H__

#include "list_of_ptr.h"	// includes <list>

#ifdef  __APPLE__		// different location with gcc-3.3 on OS X
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
#endif

using namespace std;
// using std::list;
// using std::hash_map;

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

