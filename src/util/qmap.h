/**
	\file "qmap.h"
	Queryable map with non-modifying (const) lookup method.  
	Non-modifying lookup guarantees that no extraneous empty
	elements are added by lookup.  
	$Id: qmap.h,v 1.8.24.1 2005/02/06 03:20:52 fang Exp $
 */

#ifndef	__UTIL_QMAP_H__
#define	__UTIL_QMAP_H__

#include "macros.h"
#include "qmap_fwd.h"		// forward declarations only
#include "STL/map.h"
#include "const_assoc_query.h"

namespace QMAP_NAMESPACE {
USING_MAP
using std::pair;

#if 1
/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
 */
QMAP_TEMPLATE_SIGNATURE
class qmap : public const_assoc_query<map<K,T,C,A> > {
private:
	typedef	map<K,T,C,A>				parent_type;
public:
	// convenient types used in clean(), all other inherited naturally
        typedef typename parent_type::mapped_type	mapped_type;   
	typedef typename parent_type::iterator		iterator;
	typedef typename parent_type::const_iterator	const_iterator;


	void
	clean(void);

	// everything else is inherited
};	// end class qmap

#else
//-----------------------------------------------------------------------------
QMAP_TEMPLATE_SIGNATURE
class qmap {
private:
	typedef	map<K,T,C,A>				parent_type;
	parent_type					the_map;
public:
        typedef typename parent_type::key_type		key_type;
        typedef typename parent_type::mapped_type	mapped_type;   
        typedef typename parent_type::value_type	value_type;
        typedef typename parent_type::key_compare	key_compare;
        typedef typename parent_type::value_compare	value_compare;
	typedef typename parent_type::allocator_type	allocator_type;

	/// this is not T&, this depends on implementation
	typedef typename parent_type::reference		reference;
	/// this is not const T&, this depends on implementation
	typedef typename parent_type::const_reference	const_reference;

	typedef typename parent_type::iterator		iterator;
	typedef typename parent_type::const_iterator	const_iterator;
	typedef typename parent_type::reverse_iterator	reverse_iterator;
	typedef typename parent_type::const_reverse_iterator
							const_reverse_iterator; 
	typedef typename parent_type::size_type		size_type;
	typedef typename parent_type::difference_type	difference_type;
	typedef typename parent_type::pointer		pointer;
	typedef typename parent_type::const_pointer	const_pointer;
	typedef typename parent_type::allocator_type	allocator_type;

public:
	/** Default constructor. */
	qmap();

	explicit
	qmap(const key_compare& __comp,
		const allocator_type& __a = allocator_type());

	/** Default copy constructor */
	qmap(const qmap& __x);

	template <typename _InputIterator>
	qmap(_InputIterator __first, _InputIterator __last);

	template <typename _InputIterator>
	qmap(_InputIterator __first, _InputIterator __last,
		const key_compare& __comp,
		const allocator_type& __a = allocator_type());

	/** Default destructor. */
	~qmap();

	qmap&
	operator = (const qmap& __x);

	allocator_type
	get_allocator(void) const { return the_map.get_allocator(); }

	key_compare
	key_comp(void) const { return the_map.key_comp(); }

	value_compare
	value_comp(void) const { return the_map.value_comp(); }

	iterator
	begin(void) { return the_map.begin(); }

	iterator
	end(void) { return the_map.end(); }

	const_iterator
	begin(void) const { return the_map.begin(); }

	const_iterator
	end(void) const { return the_map.end(); }

	reverse_iterator
	rbegin(void) { return the_map.rbegin(); }

	reverse_iterator
	rend(void) { return the_map.rend(); }

	const_reverse_iterator
	rbegin(void) const { return the_map.rbegin(); }

	const_reverse_iterator
	rend(void) const { return the_map.rend(); }

	bool
	empty(void) const { return the_map.empty(); }

	size_type
	size(void) const { return the_map.size(); }

	size_type
	max_size(void) const { return the_map.max_size(); }

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
	mapped_type&
	operator [] (const key_type& k);

	/**
		Constant query associative lookup.  
		Since it's constant, it won't modify the map
		by making a blank entry at the given key, 
		which is what map<K,T> does.  
		\param k the key used to lookup.  
		\return read-only COPY of the object if found, 
			else a freshly constructed object, 
			such as a wrapper NULL pointer.  
	 */
	mapped_type
	operator [] (const key_type& k) const;

	/**
		For all entries whose value is the default, remove them.  
	 */
	void
	clean(void);

	pair<iterator,bool>
	insert(const value_type& __x);

	iterator
	insert(iterator position, const value_type& __x);

	template <typename _InputIterator>
	void
	insert(_InputIterator __first, _InputIterator __last);

	void
	erase(iterator __position);

	size_type
	erase(const key_type& __x);

	void
	erase(iterator __first, iterator __last);

	void
	swap(qmap& __x);

	void
	clear(void);

	iterator
	find(const key_type& __x);

	const_iterator
	find(const key_type& __x) const;

	size_type
	count(const key_type& __x) const;

	iterator
	lower_bound(const key_type& __x);

	const_iterator
	lower_bound(const key_type& __x) const;

	iterator
	upper_bound(const key_type& __x);

	const_iterator
	upper_bound(const key_type& __x) const;

	pair<iterator,iterator>
	equal_range(const key_type& __x);

	pair<const_iterator,const_iterator>
	equal_range(const key_type& __x) const;

	template <typename _K1, typename _T1, typename _C1, typename _A1>
	friend
	bool
	operator == (const qmap<_K1,_T1,_C1,_A1>&,
		const qmap<_K1,_T1,_C1,_A1>&);

	template <typename _K1, typename _T1, typename _C1, typename _A1>
	friend
	bool
	operator < (const qmap<_K1,_T1,_C1,_A1>&,
		const qmap<_K1,_T1,_C1,_A1>&);


};	// end class qmap

//-----------------------------------------------------------------------------
/**
	Specialization for qmap that contains bare pointers.  
	Since pointers, by default, are not initialized, 
	this ensures that NULL is returned for pointers not found.  
	This class could specialize map directly, 
	and need not only apply to qmap.
	NOTE: that this make it impossible to distinguish between
	a pointer whose value is actually NULL and a pointer that doesn't 
	exist in the map.  
	The clean() function removes NULL pointers from the hash-map.  
 */
QMAP_TEMPLATE_SIGNATURE
class qmap<K,T*,C,A> : public map<K,T*,C,A> {
private:
	typedef map<K,T*,C,A>				parent_type;
	parent_type					the_map;
public:
        typedef typename parent_type::key_type		key_type;
        typedef typename parent_type::mapped_type	mapped_type;   
        typedef typename parent_type::value_type	value_type;
        typedef typename parent_type::value_compare	value_compare;
        typedef typename parent_type::key_compare	key_compare;
	typedef typename parent_type::allocator_type	allocator_type;

	typedef typename parent_type::reference		reference;
	typedef typename parent_type::const_reference	const_reference;
	typedef typename parent_type::iterator		iterator;
	typedef typename parent_type::const_iterator	const_iterator;
	typedef typename parent_type::reverse_iterator	reverse_iterator;
	typedef typename parent_type::const_reverse_iterator
							const_reverse_iterator; 
	typedef typename parent_type::size_type		size_type;
	typedef typename parent_type::difference_type	difference_type;
	typedef typename parent_type::pointer		pointer;
	typedef typename parent_type::const_pointer	const_pointer;
	typedef typename parent_type::allocator_type	allocator_type;

public:
	/** Default constructor. */
	qmap();

	explicit
	qmap(const key_compare& __comp,
		const allocator_type& __a = allocator_type());

	/** Default copy constructor */
	qmap(const qmap& __x);

	template <typename _InputIterator>
	qmap(_InputIterator __first, _InputIterator __last);

	template <typename _InputIterator>
	qmap(_InputIterator __first, _InputIterator __last,
		const key_compare& __comp,
		const allocator_type& __a = allocator_type());

	/** Default destructor. */
	~qmap();

	qmap&
	operator = (const qmap& __x);

	allocator_type
	get_allocator(void) const { return the_map.get_allocator(); }

	key_compare
	key_comp(void) const { return the_map.key_comp(); }

	value_compare
	value_comp(void) const { return the_map.value_comp(); }

	iterator
	begin(void) { return the_map.begin(); }

	iterator
	end(void) { return the_map.end(); }

	const_iterator
	begin(void) const { return the_map.begin(); }

	const_iterator
	end(void) const { return the_map.end(); }

	reverse_iterator
	rbegin(void) { return the_map.rbegin(); }

	reverse_iterator
	rend(void) { return the_map.rend(); }

	const_reverse_iterator
	rbegin(void) const { return the_map.rbegin(); }

	const_reverse_iterator
	rend(void) const { return the_map.rend(); }

	bool
	empty(void) const { return the_map.empty(); }

	size_type
	size(void) const { return the_map.size(); }

	size_type
	max_size(void) const { return the_map.max_size(); }

	/**
		Just wrapper to the parent's lookup operation, 
		which modifies the map even when querying for a
		non-existent entry.  
		Explicitly written here so the const version below
		doesn't overshadow the parent's non-const version.  
		\param k the key used to lookup.  
		\return modifiable reference to the pointer if found, 
			else a freshly constructed pointer (may be garbage!).  
	 */
	mapped_type&
	operator [] (const key_type& k);

	/**
		Constant query associative lookup.  
		Since it's constant, it won't modify the map
		by making a blank entry at the given key, 
		which is what map<K,T> does.  
		\param k the key used to lookup.  
		\return read-only COPY of the object if found, 
			else a freshly constructed object, 
			such as a wrapper NULL pointer.  
	 */
	mapped_type
	operator [] (const key_type& k) const;

	/**
		For all entries whose value is the default (NULL), 
		remove them.  
	 */
	void clean(void);

	pair<iterator,bool>
	insert(const value_type& __x);

	iterator
	insert(iterator position, const value_type& __x);

	template <typename _InputIterator>
	void
	insert(_InputIterator __first, _InputIterator __last);

	void
	erase(iterator __position);

	size_type
	erase(const key_type& __x);

	void
	erase(iterator __first, iterator __last);

	void
	swap(qmap& __x);

	void
	clear(void);

	iterator
	find(const key_type& __x);

	const_iterator
	find(const key_type& __x) const;

	size_type
	count(const key_type& __x) const;

	iterator
	lower_bound(const key_type& __x);

	const_iterator
	lower_bound(const key_type& __x) const;

	iterator
	upper_bound(const key_type& __x);

	const_iterator
	upper_bound(const key_type& __x) const;

	pair<iterator,iterator>
	equal_range(const key_type& __x);

	pair<const_iterator,const_iterator>
	equal_range(const key_type& __x) const;

	template <typename _K1, typename _T1, typename _C1, typename _A1>
	friend
	bool
	operator == (const qmap<_K1,_T1*,_C1,_A1>&,
		const qmap<_K1,_T1*,_C1,_A1>&);

	template <typename _K1, typename _T1, typename _C1, typename _A1>
	friend
	bool
	operator < (const qmap<_K1,_T1*,_C1,_A1>&,
		const qmap<_K1,_T1*,_C1,_A1>&);

};      // and class qmap specialization
#endif

//-----------------------------------------------------------------------------
}	// end namespace QMAP_NAMESPACE

#endif	// __UTIL_QMAP_H__

