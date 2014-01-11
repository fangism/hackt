/**
	\file "util/unique_list.hh"
	List that tracks uniqueness.
	Order-preserving set.  

	$Id: unique_list.hh,v 1.8 2006/11/15 00:09:08 fang Exp $
 */

#ifndef __UTIL_UNIQUE_LIST_HH__
#define __UTIL_UNIQUE_LIST_HH__

#include "util/macros.h"
#include "util/STL/list_fwd.hh"
#include "util/STL/map_fwd.hh"
#include <list>
#include <map>
#include <algorithm>		// for find

namespace util {
using std::find;

#define	UNIQUE_LIST_TEMPLATE_SIGNATURE					\
template <class T, class List, class Set>
#define	UNIQUE_LIST_CLASS			unique_list<T, List, Set>

template <class T, class List, class Set>
class unique_list;

template <class T, class List = typename std::default_list<T>::type,
	class Set = typename std::default_map<T, typename List::iterator>::type >
class unique_list;

//=============================================================================
/**
        Uses a map of list-iterators to track uniqueness.  
        Consider using extensible vector for speed, instead of list.  
	Can also use slist for list for forward-only iteration.
	Alternate implementation: use a set and a list of set-iterators.
 */
UNIQUE_LIST_TEMPLATE_SIGNATURE
class unique_list {
public:
	typedef	T				value_type;
	typedef	T&				reference;
	typedef	const T&			const_reference;
//	typedef	T*				pointer;
	typedef	const T*			const_pointer;
	typedef Set				set_type;
	typedef List				sequence_type;
	typedef typename sequence_type::const_iterator	const_iterator;
private:
	typedef typename sequence_type::iterator	iterator;
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

	const_reference
	back(void) const {
		return _sequence.back();
	}

	reference
	back(void) {
		return _sequence.back();
	}

	const_reference
	front(void) const {
		return _sequence.front();
	}

	reference
	front(void) {
		return _sequence.front();
	}

	const_iterator
	find(const_reference n) const {
		const typename set_type::const_iterator f(_set.find(n));
		if (f != _set.end()) {
			return f->second;
		} else {
			return this->end();
		}
	}

	bool
	contains(const_reference n) const {
		return this->find(n) != this->end();
	}

protected:
	iterator
	find(const_reference n) {
		const typename set_type::iterator f(_set.find(n));
		if (f != _set.end()) {
			return f->second;
		} else {
			return this->end();
		}
	}

public:
	/**
		Appends a value to end of list (and set) 
		if it's not already in list.  
		\return true if last addition was inserted
	 */
	std::pair<typename set_type::iterator, bool>
	push_back(const_reference n) {
		typename set_type::value_type p(n, iterator());
		const std::pair<typename set_type::iterator, bool>
			r(_set.insert(p));
		if (r.second) {
			// if successfully inserted into set
			_sequence.push_back(n);
			// typename sequence_type::iterator e(--_sequence.end());
			r.first->second = --_sequence.end();
		}
		return r;
	}

	/**
		\return true if last addition was inserted
	 */
	std::pair<typename set_type::iterator, bool>
	push_front(const_reference n) {
		typename set_type::value_type p(n, iterator());
		const std::pair<typename set_type::iterator, bool>
			r(_set.insert(p));
		if (r.second) {
			// if successfully inserted into set
			_sequence.push_front(n);
			r.first->second = _sequence.begin();
		}
		return r;
	}

	/// defaults to back operation
	std::pair<typename set_type::iterator, bool>
	push(const_reference n) {
		return push_back(n);
	}

	/**
		Removes back entry.  
	 */
	value_type
	pop_back(void) {
		const typename set_type::iterator
			probe(_set.find(_sequence.back()));
		INVARIANT(probe != _set.end());
		const value_type ret = *probe->second;
		_set.erase(probe);
		_sequence.pop_back();
		return ret;
	}

	value_type
	pop_front(void) {
		const typename set_type::iterator
			probe(_set.find(_sequence.front()));
		INVARIANT(probe != _set.end());
		const value_type ret = *probe->second;
		_set.erase(probe);
		_sequence.pop_front();
		return ret;
	}

	std::pair<typename set_type::iterator, bool>
	move_back(const_reference n) {
		erase(n);
		return push_back(n);
	}

	std::pair<typename set_type::iterator, bool>
	move_front(const_reference n) {
		erase(n);
		return push_front(n);
	}

	/// defaults to pop_back operation
	value_type
	pop(void) {
		return this->pop_back();
	}

	/**
		Alias for push().
		push happens to be back operation.  
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
			_sequence.erase(probe->second);
			_set.erase(probe);
		}
		// else not already in set
	}

	void
	clear(void) {
		_set.clear();
		_sequence.clear();
	}

};      // end class unique_list

//=============================================================================
}	// end namespace util

#endif	// __UTIL_UNIQUE_LIST_HH__

