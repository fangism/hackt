/**
	\file "util/unique_list.h"
	List that tracks uniqueness.
	Order-preserving set.  

	$Id: unique_list.h,v 1.4.4.1 2005/10/31 04:45:55 fang Exp $
 */

#ifndef __UTIL_UNIQUE_LIST__
#define __UTIL_UNIQUE_LIST__

#include "util/macros.h"
#include "util/STL/list.h"
#include <set>
#include <algorithm>		// for find

namespace util {
USING_LIST
using std::find;
using std::set;
//=============================================================================
/**
        Uses a set to track uniqueness.  
        Consider using extensible vector for speed, instead of list.  
	Can also use slist for list for forward-only iteration.
	__gnu_cxx::hash_set provides constant time hashing.  
 */
template <class T, class List = list<T>, class Set = set<T> >
class unique_list {
public:
	typedef	T				value_type;
//	typedef	T&				reference;
	typedef	const T&			const_reference;
//	typedef	T*				pointer;
	typedef	const T*			const_pointer;
	typedef Set				set_type;
	typedef List				sequence_type;
	typedef typename sequence_type::const_iterator	const_iterator;
private:
	set_type				_set;
	sequence_type				_sequence;
public:
	/// default constructor
	unique_list() : _set(), _sequence() { }

	// default copy-constructor

	/// default destructor
	~unique_list() { }

	const_iterator
	begin(void) const {
		return _sequence.begin();
	}

	const_iterator
	end(void) const {
		return _sequence.end();
	}

	/**
		\return whether or not the list is empty.  
	 */
	bool
	empty(void) const {
		INVARIANT(_set.empty() == _sequence.empty());
		return _set.empty();
	}

	/**
		\return number of elements in list/set.
	 */
	size_t
	size(void) const {
		INVARIANT(_set.size() == _sequence.size());
		return _set.size();
	}

	/**
		Appends a value to end of list (and set) 
		if it's not already in list.  
	 */
	void
	push(const_reference n) {
		const typename set_type::const_iterator probe(_set.find(n));
		// if not found, add
		if (probe == _set.end()) {
			_set.insert(n);
			_sequence.push_back(n);
		}
		// else ignore
	}

	/**
		Alias for push().
	 */
	void
	insert(const_reference n) {
		push(n);
	}

	/**
		Removes a value from the set/list.
	 */
	void
	erase(const_reference n) {
		const typename set_type::iterator probe(_set.find(n));
		if (probe != _set.end()) {
			const typename sequence_type::iterator
				l_probe(find(_sequence.begin(),
					_sequence.end(), n));
			INVARIANT(l_probe != _sequence.end());
			_sequence.erase(l_probe);
			_set.erase(probe);
		}
		// else not already in set
	}

};      // end class unique_list

//=============================================================================
}	// end namespace util

#endif	// __UTIL_UNIQUE_LIST__

