// "map_of_ptr.h"
// useful template for map<K,T*>
// the alternative is to instantiate map<auto_ptr<T>>...

// NOTE: assumes EXCLUSIVE ownwership of the pointers contained!
// any violation thereof will require something more powerful,
// such as reference counting.  

#ifndef __MAP_OF_PTR_H__
#define __MAP_OF_PTR_H__

#include <map>

using std::map;

//=============================================================================
/// A map of exclusively owned pointers, based on std::map.  

/**
	The members of this class are pair<K,T*>, with K being the key type
	and T is the contained type.  The pointers should be exclusive, i.e., 
	whereas other copies of the same pointer may read or write
	the object, only this map has responsibility for de-allocating
	the objects' memory.  This is generally useful for 
	look-up tables of owned references.  
 */
template <class K, class T>
class map_of_ptr : public map<K,T*> {
private:
	typedef map<K,T*>				parent;
public:
	typedef	typename parent::iterator		iterator;
	typedef	typename parent::const_iterator		const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;
protected:
	int			own;		///< ownership flag
public:

/// The default constructor just creates an empty map.  
explicit map_of_ptr() : parent(), own(1) { }
// copy constructor copies pointers without transferring ownership
	map_of_ptr(const map_of_ptr<K,T>& l);

/// The destructor frees memory to non-NULL pointers in the map.  
virtual	~map_of_ptr();		// don't want to inline this

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

using	parent::begin;
using	parent::end;

// non-essential add-on methods
};

#endif	// __MAP_OF_PTR_H__

