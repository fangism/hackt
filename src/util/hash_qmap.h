/**
	\file "hash_qmap.h"
	Queryable hash_map that doesn't add empty elements on lookup.  
	$Id: hash_qmap.h,v 1.6 2004/12/03 02:46:42 fang Exp $
 */

#ifndef	__HASH_QMAP_H__
#define	__HASH_QMAP_H__

#include "macros.h"
#include "hash_qmap_fwd.h"		// forward declarations only

namespace HASH_QMAP_NAMESPACE {
using namespace HASH_MAP_NAMESPACE;

//-----------------------------------------------------------------------------
/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
	\param K the key type.
	\param T the mapped data type.
	\param H the hashing function.
	\param E the equal_to comparator function.
	\param A the allocator.  
 */
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap {
private:
	typedef hash_map<K,T,H,E,A>			parent;
	parent						the_map;
public:
	/// key type is K
	typedef typename parent::key_type		key_type;
	/// data type is T
	typedef typename parent::data_type		data_type;
	/// mapped type is T
	typedef typename parent::mapped_type		mapped_type;
	/// value_type is pair<const K, T>
	typedef typename parent::value_type		value_type;

	typedef typename parent::hasher			hasher;
	typedef typename parent::key_equal		key_equal;
	typedef typename parent::allocator_type		allocator_type;

	/// this is not T&, depends on implementation type (hashtable)
	typedef typename parent::reference		reference;
	/// this is not const T&, depends on implementation type (hashtable)
	typedef typename parent::const_reference	const_reference;

	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
	typedef typename parent::size_type		size_type;
	typedef typename parent::difference_type	difference_type;
	typedef typename parent::pointer		pointer;
	typedef typename parent::const_pointer		const_pointer;
	typedef typename parent::allocator_type		allocator_type;

	// note: no reverse iterator

public:
	hasher
	hash_funct() const { return the_map.hash_funct(); }

	key_equal
	key_eq() const { return the_map.key_eq(); }

	allocator_type
	get_allocator() const { return the_map.get_allocator(); }

public:
	/** Default constructor. */
	hash_qmap();

	explicit
	hash_qmap(size_type __n);

	hash_qmap(size_type __n, const hasher& __hf);

	hash_qmap(size_type __n, const hasher& __hf, const key_equal& __eql,
		const allocator_type& __a = allocator_type());

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l);

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l, size_type __n);

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l, size_type __n,
		const hasher& __hf);

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l, size_type __n,
		const hasher& __hf, const key_equal& __eql,
		const allocator_type& __a = allocator_type());


	/** Default destructor. */
	~hash_qmap();

	size_type
	size(void) const { return the_map.size(); }

	size_type
	max_size(void) const { return the_map.max_size(); }

	bool
	empty(void) const { return the_map.empty(); }

	void
	swap(hash_qmap& h);


	template <class _K1, class _T1, class _HF, class _EqK, class _Al>
	friend bool operator == (const hash_qmap<_K1, _T1, _HF, _EqK, _Al>&,
		const hash_qmap<_K1, _T1, _HF, _EqK, _Al>&);

	iterator
	begin(void) { return the_map.begin(); }

	iterator
	end(void) { return the_map.end(); }

	const_iterator
	begin(void) const { return the_map.begin(); }

	const_iterator
	end(void) const { return the_map.end(); }

	pair<iterator,bool>
	insert(const value_type& __obj);

	pair<iterator,bool>
	insert_noresize(const value_type& __obj);

	iterator
	find(const key_type& __key);

	const_iterator
	find(const key_type& __key) const;

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

	// consider something that returns const_reference?

	/**
		Constant query associative lookup.  
		Since it's constant, it won't modify the map
		by making a blank entry at the given key, 
		which is what map<K,T> does.  
		IMPORTANT: just using the [] operator on a map
		will default to the non-const version.  
		To disambiguate, thus forcing use of this const-preserving
		lookup, one must explicitly static_cast the invoking object.
		e.g. static_cast<const map_type&>(map_name)[key]
		\param k the key used to lookup.  
		\return COPY of the object if found, 
			else a freshly constructed object.  
			Can't use a reference because might return
			reference to a temporary.  
	 */
	mapped_type
	operator [] (const key_type& k) const;

	size_type
	count(const key_type& __key) const;

	pair<iterator, iterator>
	equal_range(const key_type& __key);

	pair<const_iterator, const_iterator>
	equal_range(const key_type& __key) const;

	size_type
	erase(const key_type& __key);

	void
	erase(iterator __it);

	void
	erase(iterator __f, iterator __l);

	void
	clear();

	void
	resize(size_type __hint);

	size_type
	bucket_count() const;

	size_type
	max_bucket_count() const;

	size_type
	elems_in_bucket(size_type __n) const;

};	// end class hash_qmap

//-----------------------------------------------------------------------------
/**
	Specialization for hash_qmap that contains bare pointers.  
	Since pointers, by default, are not initialized, 
	this ensures that NULL is returned for pointers not found.  
	This class could specialize hash_map directly, 
	and need not only apply to hash_qmap.
	NOTE: that this make it impossible to distinguish between
	a pointer whose value is actually NULL and a pointer that doesn't 
	exist in the map.  
	The clean() function removes NULL pointers from the hash-map.  
 */
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap<K,T*,H,E,A> {
private:
	typedef hash_map<K,T*,H,E,A>			parent;
	parent						the_map;
public:
	/// key type is K
	typedef typename parent::key_type		key_type;
	/// data type is T
	typedef typename parent::data_type		data_type;
	/// mapped type is T
	typedef typename parent::mapped_type		mapped_type;
	/// value_type is pair<const K, T>
	typedef typename parent::value_type		value_type;

	typedef typename parent::hasher			hasher;
	typedef typename parent::key_equal		key_equal;
	typedef typename parent::allocator_type		allocator_type;

	typedef typename parent::reference		reference;
	typedef typename parent::const_reference	const_reference;
	typedef typename parent::iterator		iterator;
	typedef typename parent::const_iterator		const_iterator;
	typedef typename parent::size_type		size_type;
	typedef typename parent::difference_type	difference_type;
	typedef typename parent::pointer		pointer;
	typedef typename parent::const_pointer		const_pointer;
	typedef typename parent::allocator_type		allocator_type;

	// note: no reverse iterator

public:
	// use default constructor and destructors

public:
	hasher
	hash_funct() const { return the_map.hash_funct(); }

	key_equal
	key_eq() const { return the_map.key_eq(); }

	allocator_type
	get_allocator() const { return the_map.get_allocator(); }

public:
	/** Default constructor. */
	hash_qmap();

	explicit
	hash_qmap(size_type __n);

	hash_qmap(size_type __n, const hasher& __hf);

	hash_qmap(size_type __n, const hasher& __hf, const key_equal& __eql,
		const allocator_type& __a = allocator_type());

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l);

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l, size_type __n);

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l, size_type __n,
		const hasher& __hf);

	template <class _InputIterator>
	hash_qmap(_InputIterator __f, _InputIterator __l, size_type __n,
		const hasher& __hf, const key_equal& __eql,
		const allocator_type& __a = allocator_type());


	/** Default destructor. */
	~hash_qmap();

	size_type
	size(void) const { return the_map.size(); }

	size_type
	max_size(void) const { return the_map.max_size(); }

	bool
	empty(void) const { return the_map.empty(); }

	void
	swap(hash_qmap& h);


	template <class _K1, class _T1, class _HF, class _EqK, class _Al>
	friend bool operator== (const hash_qmap<_K1, _T1, _HF, _EqK, _Al>&,
		const hash_qmap<_K1, _T1, _HF, _EqK, _Al>&);

	iterator
	begin(void) { return the_map.begin(); }

	iterator
	end(void) { return the_map.end(); }

	const_iterator
	begin(void) const { return the_map.begin(); }

	const_iterator
	end(void) const { return the_map.end(); }

	pair<iterator,bool>
	insert(const value_type& __obj);

	pair<iterator,bool>
	insert_noresize(const value_type& __obj);

	iterator
	find(const key_type& __key);

	const_iterator
	find(const key_type& __key) const;

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

	// consider something that returns const_reference?

	/**
		Constant query associative lookup.  
		Since it's constant, it won't modify the map
		by making a blank entry at the given key, 
		which is what map<K,T> does.  
		IMPORTANT: just using the [] operator on a map
		will default to the non-const version.  
		To disambiguate, thus forcing use of this const-preserving
		lookup, one must explicitly static_cast the invoking object.
		e.g. static_cast<const map_type&>(map_name)[key]
		\param k the key used to lookup.  
		\return COPY of the object if found, 
			else a freshly constructed object.  
			Can't use a reference because might return
			reference to a temporary.  
	 */
	mapped_type
	operator [] (const key_type& k) const;

	size_type
	count(const key_type& __key) const;

	pair<iterator, iterator>
	equal_range(const key_type& __key);

	pair<const_iterator, const_iterator>
	equal_range(const key_type& __key) const;

	size_type
	erase(const key_type& __key);

	void
	erase(iterator __it);

	void
	erase(iterator __f, iterator __l);

	void
	clear();

	void
	resize(size_type __hint);

	size_type
	bucket_count() const;

	size_type
	max_bucket_count() const;

	size_type
	elems_in_bucket(size_type __n) const;


	/**
		*NEW* -- This is not in the default hash_map.
		For all entries whose value is the default, remove them.
	 */
	void
	clean(void);

};	// and class hash_qmap specialization

//-----------------------------------------------------------------------------

}	// end namespace HASH_QMAP_NAMESPACE

#endif	//	__HASH_QMAP_H__

