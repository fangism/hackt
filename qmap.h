// "qmap.h"

#ifndef	__QMAP_H__
#define	__QMAP_H__

#include <map>

#ifndef	QMAP_NAMESPACE
#define	QMAP_NAMESPACE		fang
#endif

namespace QMAP_NAMESPACE {
using std::map;

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
	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
	typedef typename parent::reverse_iterator	reverse_iterator;
	typedef typename parent::const_reverse_iterator	const_reverse_iterator;
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
			else a freshly constructed object.  
	 */
	const T operator [] (const K& k) const {
		const_iterator i = find(k);	// uses find() const;
		return (i != this->end()) ? i->second : T();
		// if T is a pointer class, should be equivalent to NULL
		// or whatever the default constructor is
	}

};	// end class qmap

}	// end namespace

#endif	//	__QMAP_H__

