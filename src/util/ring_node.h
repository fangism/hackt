/**
	\file "util/ring_node.h"
	Declaration for ring_node struct.
	$Id: ring_node.h,v 1.3.14.1 2005/07/15 03:49:28 fang Exp $
 */

#ifndef	__UTIL_RING_NODE_H__
#define	__UTIL_RING_NODE_H__

// whether or not to include subset of definitions separately, but still inline
#ifndef	FORCE_INLINE_RING_NODE
#define	FORCE_INLINE_RING_NODE		0
#endif

#include <iterator>
#include "util/macros.h"
#include "util/ring_node_fwd.h"

namespace util {
//=============================================================================
// forward declarations
// and summary of type defined here

class ring_node_base;

template <class>
class ring_node_iterator_base;

template <class, class, class>
class ring_node_iterator;

template <class>
class ring_node_iterator_default;

template <class>
class ring_node_derived;

template <class, class, class>
class ring_node_derived_iterator;

template <class>
class ring_node_derived_iterator_default;

#define	RING_NODE_DERIVED_TEMPLATE_SIGNATURE				\
template <class T>


//=============================================================================
/**
	Safely down-castable base class for templated ring_nodes.  
 */
class ring_node_base {
friend class ring_node_iterator_base<ring_node_base>;
public:
	typedef	ring_node_base*			next_type;
	typedef	const ring_node_base*		const_next_type;
protected:
	ring_node_base*				next;

protected:
	/**
		All ring_node point to themselves upon construction.  
		No empty construction.  
	 */
#if FORCE_INLINE_RING_NODE
	ring_node_base() : next(this) { }
#else
	ring_node_base();
#endif

protected:
	explicit
#if FORCE_INLINE_RING_NODE
	ring_node_base(ring_node_base* r) : next(r) { NEVER_NULL(next); }
#else
	ring_node_base(ring_node_base* r);
#endif

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
		next = NULL;
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
#if FORCE_INLINE_RING_NODE
	void
	unsafe_merge(ring_node_base& r) {
		std::swap(next, r.next);
	}
#else
	void
	unsafe_merge(ring_node_base& r);
#endif

public:
#if 0
	ring_node_base*&
	get_next(void);
#endif

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
	Base iterator for ring_node.  
	\param N is the underlying type pointed to by the iterator.  
		For ring_node, it will be ring_node_base.  
		For ring_node_derived, it will be different.  
 */
template <class N>
class ring_node_iterator_base {
template <class, class, class> friend class ring_node_iterator;
public:
	typedef	N				node_type;
	typedef	N*				node_ptr_type;
	typedef	const N*			const_node_ptr_type;
	typedef	size_t				size_type;
	typedef	ptrdiff_t			difference_type;
	typedef	std::forward_iterator_tag	iterator_category;
protected:
	/**
		Points to the current referenced node.
	 */
	node_ptr_type				current_node;
	/**
		Since iterator points to a circular structure with no
		one-past-the-end element, we need a second pointer to
		distinguish between between begin and end when iterators
		point to the same reference element.  
		See implementation of comparison operators.  
	 */
	const_node_ptr_type			last_node;

protected:
	// protecting constructors to prevent abuse...

	ring_node_iterator_base() : current_node(NULL), last_node(NULL) { }

	/**
		This constructor is intended for use with begin().  
		Note that by construction, no ring_nodes are empty!
	 */
	explicit
	ring_node_iterator_base(node_ptr_type p) :
		current_node(p), last_node(NULL) {
	}

	/**
		This constructor is intended for use with end().
		\param p initial node pointer.  
		\param l->next should be equal to p, unless it is NULL.
	 */
	ring_node_iterator_base(node_ptr_type p, const_node_ptr_type l) :
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
	operator == (const node_ptr_type* r) const {
		return current_node == r;
	}

	bool
	operator != (const node_ptr_type* r) const {
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
	Handy way of declaring standard iterator types for ring nodes.  
 */
template <class T>
struct ring_node_iterator_default {
	typedef	ring_node_iterator<T, T&, T*>			type;
	typedef	ring_node_iterator<T, const T&, const T*>	const_type;
};	// end struct ring_node_iterator_default

//-----------------------------------------------------------------------------
/**
	A ring_node::iterator.
 */
template <class T, class R, class P>
class ring_node_iterator : public ring_node_iterator_base<ring_node_base> {
protected:
	typedef	ring_node_iterator_base<ring_node_base>	parent_type;
public:
	typedef	T				value_type;
	typedef	R				reference;
	typedef	P				pointer;

	typedef	ring_node_iterator<T, R, P>	this_type;
#if 0
	typedef	ring_node_iterator<T, T&, T*>	iterator;
	typedef	ring_node_iterator<T, const T&, const T*>
						const_iterator;
#else
	typedef	typename ring_node_iterator_default<T>::type
						iterator;
	typedef	typename ring_node_iterator_default<T>::const_type
						const_iterator;
#endif
	typedef	ring_node<T>			node_type;
	typedef	ring_node_base			node_base_type;

public:
	explicit
	ring_node_iterator(node_base_type* n) :
		parent_type(n) { }

	/**
		\pre n must point to l.
	 */
	ring_node_iterator(node_base_type* n, const node_base_type* l) :
		parent_type(n, l) { }

	// using default copy constructor
	// iterators are allowed to point to the same nodes.  

	/**
		Allows both iterator and const_iterator to be 
		copy-constructed from iterator.  
	 */
	// explicit
	ring_node_iterator(const iterator& i) :
		parent_type(i.current_node, i.last_node) { }

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
#if 0
	typedef	ring_node_iterator<T, T&, T*>	iterator;
	typedef	ring_node_iterator<T, const T&, const T*>
						const_iterator;
#else
	typedef	typename ring_node_iterator_default<T>::type
						iterator;
	typedef	typename ring_node_iterator_default<T>::const_type
						const_iterator;
#endif
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
	ring_node() : ring_node_base(), value() { }

	explicit
	ring_node(const value_type& v) : ring_node_base(), value(v) { }

	/**
		NOTE the unusual copy construct semantics!
		Copy constructor does not copy the ring!
		Only copies the value contained.  
		This maintains the uniqueness property:
		No ring_node shall be pointed to be more than one other
		ring_node.  
	 */
	ring_node(const ring_node& r) : ring_node_base(), value(r.value) { }

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

	void
	unsafe_merge(this_type& n) {
		base_type::unsafe_merge(n);
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
/**
	Template generalization for ring_node implementation policies.  
	Requires that type T has next_type defined, which is the
	type of its next member.  
	Partially specializing this will allow other classes
	to use the ring_node design pattern without
	deriving from ring_node or ring_node_base.  
 */
template <class T>
struct ring_node_traits {
	typedef	T					ring_node_type;
	typedef typename ring_node_type::next_type	next_type;

	static
	next_type*&
	get_next(ring_node_type& n) { return n.next; }

	static
	next_type*
	get_next(const ring_node_type& n) { return n.next; }
};	// end struct ring_node_traits

//=============================================================================
// classes below are implementations of ring_nodes *derived* from the base type
//=============================================================================
/**
	Handy shortcut for making standard ring_node_derived_iterator types.  
 */
template <class T>
struct ring_node_derived_iterator_default {
	typedef	ring_node_derived_iterator<T, T&, T*>	type;
	typedef	ring_node_derived_iterator<T, const T&, const T*>
							const_type;
};	// end class ring_node_derived_iterator_default

//-----------------------------------------------------------------------------
/**
	Iterator for use with ring_node_derived.  
 */
template <class T, class R, class P>
class ring_node_derived_iterator :
		public ring_node_iterator_base<ring_node_derived<T> > {
public:
	typedef	T					value_type;
	typedef	R					reference;
	typedef	P					pointer;

	typedef	ring_node_derived_iterator<T, R, P>	this_type;
#if 0
	typedef	ring_node_derived_iterator<T, T&, T*>	iterator;
	typedef	ring_node_derived_iterator<T, const T&, const T*>
							const_iterator;
#else
	typedef	typename ring_node_derived_iterator_default<T>::type
							iterator;
	typedef	typename ring_node_derived_iterator_default<T>::const_type
							const_iterator;
#endif
	typedef	ring_node_derived<T>			node_type;
protected:
	typedef	ring_node_iterator_base<node_type>	parent_type;
public:
	explicit
	ring_node_derived_iterator(node_type* n) :
		parent_type(n) { }

	/**
		\pre n must point to l.
	 */
	ring_node_derived_iterator(node_type* n, const node_type* l) :
		parent_type(n, l) { }

	// using default copy constructor
	// iterators are allowed to point to the same nodes.  

	/**
		Allows both iterator and const_iterator to be 
		copy-constructed from iterator.  
	 */
	// explicit
	ring_node_derived_iterator(const iterator& i) :
		parent_type(i.current_node, i.last_node) { }

	reference
	operator * () const {
		return static_cast<node_type&>(*this->current_node);
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

};	// end class ring_node_derived_iterator

//=============================================================================
// class ring_node_derived

/**
	The purpose of this class is to extend an existing class
	with ring_node capabilities, rather than contain a value
	of the base type.  

	Sometimes having next point to a ring_node_base is 
	too restrictive.  This class template extends the ring node
	concept to allow next to point to any type that meets the
	ring_node criteria.  
	Unfortunately next is always initialized to point to itself, 
	so one cannot create "colored" ring elements with this template.  

	In contrast with the other ring_node template class, 
	this does not have any dereference operators, because this
	is already a sub-type of T.  

	\param N the type pointed to by the next pointer.  
		Defaults to a ring_node_derived of this type.  
 */
RING_NODE_DERIVED_TEMPLATE_SIGNATURE
class ring_node_derived : public T {
template <class> friend class ring_node_iterator_base;
// template <class, class, class> friend class ring_node_derived_iterator;
	typedef	T					base_type;
	typedef	ring_node_derived<T>			this_type;
protected:
	typedef	T					parent_type;
public:
	typedef	this_type*				next_type;
	typedef	const this_type*			const_next_type;
	typedef	T					value_type;
	typedef	T&					reference;
	typedef	const T&				const_reference;
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
#if 0
	typedef	ring_node_derived_iterator<T, T&, T*>	iterator;
	typedef	ring_node_derived_iterator<T, const T&, const T*>
							const_iterator;
#else
	typedef typename ring_node_derived_iterator_default<T>::type
							iterator;
	typedef typename ring_node_derived_iterator_default<T>::const_type
							const_iterator;
#endif
	// no reverse iterators, forward only
	typedef	size_t					size_type;
	typedef	ptrdiff_t				difference_type;
	// any allocator type
protected:
	next_type					next;
public:
	/**
		Requires that next_type is a base type of this, 
		i.e. ring_node_derived : T.
		Requires that T has a default constructor.  
		Does this make it unusable with T == POD?  Yes.  
	 */
	ring_node_derived() : T(), next(this) { }

	explicit
	ring_node_derived(const value_type& v) : T(v), next(this) { }
	
	/**
		NOTE the unusual copy construct semantics!
		Copy constructor does not copy the ring!
		Only copies the value contained.  
		This maintains the uniqueness property:
		No ring_node shall be pointed to be more than one other
		ring_node.  
	 */
	ring_node_derived(const this_type& r) :
		T(static_cast<const T&>(r)), next(this) { }

	/**
		Note: non-virtual destructor.  
		Thus, it is NOT safe to let pointers to this type
		manage memory.  

		We end up making this virtual to make this type polymorphic, 
		want to be able to dynamically cast this_type to 
		derived types.  
	 */
virtual	~ring_node_derived();

public:

	const_next_type
	get_next(void) const { return next; }

	// no assignment
#if 0
	// no dereference operators required!
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
#endif

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

	bool
	points_to(this_type* b) const {
		return (next == b);
	}

	bool
	contains(const this_type& ) const;

	// front() ?
	// back() ?

#if 0
	void
	push(const value_type& v);
#endif

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
		if (!contains(n))
			std::swap(next, n.next);
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

};	// end class ring_node_derived

//=============================================================================
}	// end namespace util

#endif	// __UTIL_RING_NODE_H__

