/**
	\file "ring_node.h"
	Declaration for ring_node struct.
	$Id: ring_node.h,v 1.1.2.3 2005/02/09 04:14:19 fang Exp $
 */

#ifndef	__UTIL_RING_NODE_H__
#define	__UTIL_RING_NODE_H__

// whether or not to include subset of definitions separately, but still inline
#ifndef	FORCE_INLINE_RING_NODE
#define	FORCE_INLINE_RING_NODE		0
#endif

// #include <iostream>
#include <iterator>
#include "macros.h"
#include "ring_node_fwd.h"

namespace util {
//=============================================================================
// forward declarations

class ring_node_base;

class ring_node_iterator_base;

template <class, class, class>
class ring_node_iterator;

//=============================================================================
/**
	Safely down-castable base class for templated ring_nodes.  
 */
class ring_node_base {
friend class ring_node_iterator_base;
protected:
	ring_node_base*				next;

private:
	/**
		All ring_node point to themselves upon construction.  
		No empty construction.  
	 */
	ring_node_base();

protected:
	explicit
	ring_node_base(ring_node_base* r) : next(r) { NEVER_NULL(next); }

#if FORCE_INLINE_RING_NODE
	/**
		Potentially expensive destructor.
		Must maintain circular reference invariant!
	 */
	~ring_node_base() {
		if (next != this) {
			ring_node_base* walk = next;
			while (walk->next != this)
				walk = walk->next;
			// found the node that points to this, update it
			walk->next = next;
		}
		// else this is the last node, just drops itself
	}
#else
	~ring_node_base();
#endif

	/**
		Fusing two ring_nodes into one, extremely efficient.  
		This is unsafe, because if two nodes are already
		in the same cycle, then swapping pointer will result
		in two disjoint cycles!  Kinda cool actually.  
	 */
	void
	unsafe_merge(ring_node_base& r) {
		std::swap(next, r.next);
	}

public:
	/**
		Checks for intersection before merging.  
		If they already intersect, then this does nothing.  
	 */
	void
	safe_merge(ring_node_base& r) {
		if (!contains(r))
			unsafe_merge(r);
	}

	/**
		Run-time check for intersection.  
		Stops as soon as one element is found reachable from the other, 
		or as soon as one list is done searching.
		\pre A containing B <=> B containing A.
		\post A containing B <=> B containing A.
	 */
#if FORCE_INLINE_RING_NODE
	bool
	contains(const ring_node_base& r) const {
		const ring_node_base* walk1 = this;
		const ring_node_base* walk2 = &r;
		do {
			if (walk1 == &r || walk2 == this)
				return true;
			else {
				walk1 = walk1->next;
				walk2 = walk2->next;
			}
		} while (walk1 != this && walk2 != &r);
		return false;
	}
#else
	bool
	contains(const ring_node_base& r) const;
#endif

	bool
	points_to(ring_node_base* b) const {
		return (next == b);
	}

};	// end struct ring_node_base

//-----------------------------------------------------------------------------
/**
	Base iterator for generic ring nodes.  
 */
class ring_node_iterator_base {
template <class, class, class> friend class ring_node_iterator;
public:
	typedef	size_t				size_type;
	typedef	ptrdiff_t			difference_type;
	typedef	std::forward_iterator_tag	iterator_category;
protected:
	/**
		Points to the current referenced node.
	 */
	ring_node_base*				current_node;
	/**
		Since iterator points to a circular structure with no
		one-past-the-end element, we need a second pointer to
		distinguish between between begin and end when iterators
		point to the same reference element.  
		See implementation of comparison operators.  
	 */
	const ring_node_base*			last_node;

protected:
	// protecting constructors to prevent abuse...

	ring_node_iterator_base() : current_node(NULL), last_node(NULL) { }

	/**
		This constructor is intended for use with begin().  
		Note that by construction, no ring_nodes are empty!
	 */
	explicit
	ring_node_iterator_base(ring_node_base* p) :
		current_node(p), last_node(NULL) {
	}

	/**
		This constructor is intended for use with end().
		\param p initial node pointer.  
		\param l should be equal to p->next, unless it is NULL.
	 */
	ring_node_iterator_base(ring_node_base* p, const ring_node_base* l) :
		current_node(p), last_node(l) {
		NEVER_NULL(l);
		INVARIANT(l->points_to(p));
	}

public:
	void
	advance(void) {
		last_node = current_node;
		current_node = current_node->next;
		NEVER_NULL(last_node);
		NEVER_NULL(current_node);
	}

	/**
		Most of the time, this should be as fast as 
		the time for one comparison because the first
		comparison will usually be short-circuit false.  
	 */
	bool
	operator == (const ring_node_iterator_base& i) const {
		return (current_node == i.current_node) &&
			(last_node == i.last_node);
	}

	/**
		Most of the time, this should be as fast as 
		the time for one comparison because the first
		comparison will usually be short-circuit true.  
	 */
	bool
	operator != (const ring_node_iterator_base& i) const {
		return (current_node != i.current_node) ||
			(last_node != i.last_node);
	}

	bool
	operator == (const ring_node_base* r) const {
		return current_node == r;
	}

	bool
	operator != (const ring_node_base* r) const {
		return current_node != r;
	}

#if 0
	std::ostream&
	dump(std::ostream& o) const {
		return o << "ring_node_iter: " << current_node <<
			", " << last_node;
	}
#endif

};	// end class ring_node_iterator_base

//-----------------------------------------------------------------------------
/**
	A ring_node::iterator.
 */
template <class T, class R, class P>
class ring_node_iterator : public ring_node_iterator_base {
public:
	typedef	T				value_type;
	typedef	R				reference;
	typedef	P				pointer;

	typedef	ring_node_iterator<T, R, P>	this_type;
	typedef	ring_node_iterator<T, T&, T*>	iterator;
	typedef	ring_node_iterator<T, const T&, const T*>
						const_iterator;
	typedef	ring_node<T>			node_type;
	typedef	ring_node_base			node_base_type;

	explicit
	ring_node_iterator(node_base_type* n) :
		ring_node_iterator_base(n) { }

	/**
		\pre n must point to l.
	 */
	ring_node_iterator(ring_node_base* n, const ring_node_base* l) :
		ring_node_iterator_base(n, l) { }

	// using default copy constructor
	// iterators are allowed to point to the same nodes.  

	/**
		Allows both iterator and const_iterator to be 
		copy-constructed from iterator.  
	 */
	// explicit
	ring_node_iterator(const iterator& i) :
		ring_node_iterator_base(i.current_node, i.last_node) { }

	reference
	operator * () const {
		return *static_cast<node_type&>(*current_node);
	}

	pointer
	operator -> () const { return &(operator * ()); }

	/**
		Prefix increment / advance.
	 */
	this_type&
	operator ++ () {
		this->advance();
		return *this;
	}

	/**
		Postfix increment / advance.
	 */
	this_type
	operator ++ (int) {
		const this_type temp(*this);
		this->advance();
		return temp;
	}

};	// end struct ring_node_iterator

//-----------------------------------------------------------------------------
/**
	A ring-node is an implementation of an amoeba-like circularly
	linked list.  
	A ring_node by itself forms a ring with itself.  
	Any two ring_node merged together will connect into a bigger ring.  

	Nodes of this type have several invariant properties:
	1) Nodes are self assembled into rings.  
		Any node object is a member of EXACTLY ONE ring.  
		A new node is always connected to itself in a self-ring.  
	2) As rings grow and nodes die, this property is maintained INVARIANT.  
	3) If A is reachable from B <=> B is reachable from A.  
	4) While shape and structure are maintained, nodes never manage
		each other's memory -- strictly mind-your-own-memory.  
 */
template <class T>
class ring_node : public ring_node_base {
template <class, class, class> friend class ring_node_iterator;
private:
	typedef	ring_node_base			base_type;
	typedef	ring_node<T>			this_type;
public:
	typedef	T				value_type;
	typedef	T&				reference;
	typedef	const T&			const_reference;
	typedef	T*				pointer;
	typedef	const T*			const_pointer;
	typedef	ring_node_iterator<T, T&, T*>	iterator;
	typedef	ring_node_iterator<T, const T&, const T*>
						const_iterator;
	// no reverse iterators, forward only
	typedef	size_t				size_type;
	typedef	ptrdiff_t			difference_type;
	// any allocator type
protected:
	value_type				value;
public:
	/**
		All ring_node point to themselves upon construction.  
	 */
	ring_node() : ring_node_base(this), value() { }

	explicit
	ring_node(const value_type& v) : ring_node_base(this), value(v) { }

	/**
		NOTE the unusual copy construct semantics!
		Copy constructor does not copy the ring!
		Only copies the value contained.  
		This maintains the uniqueness property:
		No ring_node shall be pointed to be more than one other
		ring_node.  
	 */
	ring_node(const ring_node& r) : ring_node_base(this), value(r.value) { }

	/**
		Uses base destructor, which walks the list.  
		Will not be virtual, never deleting through base-type 
		pointers.  
	 */
	~ring_node() { }

	// no assignment

	/**
		Since node and list are one and the same, 
		we can dereference the node just like an iterator.  
	 */
	reference
	operator * () { return value; }

	const_reference
	operator * () const { return value; }

	pointer
	operator -> () { return &value; }

	const_pointer
	operator -> () const { return &value; }

	iterator
	begin(void) { return iterator(next); }

	const_iterator
	begin(void) const {
		return const_iterator(next);
	}

	iterator
	end(void) { return iterator(this->next, this); }

	/**
		Nodes and lists are actually one and the same
		tightly coupled.  
	 */
	const_iterator
	end(void) const {
		return const_iterator(this->next, this);
	}

	/**
		ring_nodes are never empty!
	 */
	bool
	empty(void) const { return false; }

	/**
		Linear-time size evaluation.  Always >= 1.
	 */
	size_type
	size(void) const { return std::distance(begin(), end()); }

	size_type
	max_size(void) const { return size_type(-1); }

	using base_type::contains;

	// front() ?
	// back() ?

	void
	push(const value_type& v);

	// don't allow use of iterators to insert...
	// only provide value interface.  

#if 0
	void
	insert(iterator pos, const value_type& v);
#endif

	/**
		Make sure that n is NOT already a member of this ring.  
		Invariant: this does not contain n if and only if n 
			does not contain this if and only if neither n 
			nor this contain any of each other's elements -- 
			they are disjoint.  
		This should be fairly type independent.
	 */
	void
	merge(this_type& n) {
		base_type::safe_merge(n);
	}

#if 0
	iterator
	erase(iterator pos);
#endif

	// void swap(...); ?

	// void clear(void); ?

	// void splice(...); ?

	// void remove(const value_type& v); ?

	// void sort(...); ?

};	// end class ring_node

//=============================================================================
}	// end namespace util

#endif	// __UTIL_RING_NODE_H__

