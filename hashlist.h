// "hashlist.h"
// a list with near-constant time lookup
// needed for when data needs to be kept in an ordered list
// with rapid acesss to its elements by a key

#ifndef	__HASHLIST_H__
#define	__HASHLIST_H__

#include <list>
using std::list;

#include "hash_map.h"		// NOT the system <hash_map>

// make own namespace?
//=============================================================================
/**
	Derived from standard list<> but maintains a hash of pointers for
	constant time associative access.  
	Interface is limited to a few access methods.  
	Most useful when T is a pointer-class.  
*/
template <class K, class T>
class hashlist : protected list<T> {
protected:
	typedef list<T>					parent;
public:
	typedef	typename parent::iterator		iterator;
	typedef	typename parent::const_iterator		const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;
protected:
	typedef	hash_map<K,T*> 				map_type;

protected:
	/// maps a key to an iterator in the list
	map_type					itermap;
public:
	hashlist() : parent(), itermap() { }
virtual	~hashlist() { }

// we limit the interface to only a few methods
T*	append(const K&, const T&);		///< like push_back()
const T*	operator[] (const K& k) const;
T*	operator[] (const K& k);
// no pop_back();
using	list<T>::begin;
using	list<T>::end;
using	list<T>::empty;
};	// end class hashlist

//=============================================================================

#endif	// __HASHLIST_H__

