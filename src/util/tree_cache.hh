/**
	\file "util/tree_cache.hh"
	Structure for a hierarchical cache.
	$Id: tree_cache.hh,v 1.2 2010/04/02 22:19:23 fang Exp $
 */

#ifndef	__UTIL_TREE_CACHE_H__
#define	__UTIL_TREE_CACHE_H__

#include <iosfwd>
#include <set>
#include "util/size_t.h"

namespace util {
using std::set;
using std::ostream;
using std::pair;

#define	TREE_CACHE_TEMPLATE_SIGNATURE		template <class K, class T>
#define	TREE_CACHE_CLASS			tree_cache<K,T>

/**
	Cache structure where the weight of the children contributes to
	the weight of the parent.  
	Weights are used to determine eviction policy.
	\param T the value type to cache.
 */
template <class K, class T>
class tree_cache {
public:
	typedef	T				value_type;
	typedef	tree_cache<K, T>		this_type;
	typedef	K				key_type;
	typedef	set<this_type>			children_cache_type;
	typedef	typename children_cache_type::iterator
						child_iterator;
	typedef	typename children_cache_type::const_iterator
						const_child_iterator;
	typedef	size_t				weight_type;
private:
	this_type*				parent;
	/**
		A node needs to be aware of its own key in relation
		to its parent (if it has a parent).
	 */
	const key_type				_key;
	// mutable because these fields don't affect the sort-key
	mutable children_cache_type		children_cache;
	mutable weight_type			local_weight;
	mutable weight_type			children_weight;
public:
	mutable value_type			value;

public:
	explicit
	tree_cache(const key_type&);

	tree_cache(const key_type&, const value_type&);

	~tree_cache();

	const key_type&
	key(void) const { return this->_key; }

	size_t
	size(void) const;

	weight_type
	weight(void) const {
		return this->local_weight +this->children_weight;
	}

	pair<child_iterator, bool>
	insert_find(const key_type&);

	weight_type
	halve(const weight_type shave = 0) const;

	bool
	operator < (const this_type& r) const {
		return this->_key < r._key;
	}

	ostream&
	dump(ostream&, void (*)(ostream&, const value_type&)) const;

public:
	// not really meant for public use
	tree_cache(const this_type&);

private:
	void
	count_hit(void);

	void
	connect_parent(this_type&);
	// no need for disconnect_parent

	// cannot copy-assign this type
	this_type&
	operator = (const this_type&);

};	// end struct tree_cache

}	// end namespace util

#endif	// __UTIL_TREE_CACHE_H__

