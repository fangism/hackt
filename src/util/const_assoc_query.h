/**
	\file "util/const_assoc_query.h"
	Adapter around map-like classes to alter the semantics of 
	the associative lookup operator [].
	$Id: const_assoc_query.h,v 1.4 2005/09/05 05:04:51 fang Exp $
 */

#ifndef	__UTIL_CONST_ASSOC_QUERY_H__
#define	__UTIL_CONST_ASSOC_QUERY_H__

namespace util {
//=============================================================================
/**
	Adapter to alter the semantics of the associative lookup operator []
	to provide a lookup that is const, i.e. guaranteed NOT to modify
	the map by creating a default-constructed entry.  
	This is accomplished by publicly inheriting all methods from
	the base class, and overshadowing the definitions of the [] operator.  
	The only other requirement for the base class is that 
	the find() method is defined with both const and non-const semantics.  

	Definitions are spearate to give option to compile separately.  
 */
template <class M>
class const_assoc_query : public M {
public:
	typedef	M					parent_type;

	typedef typename parent_type::key_type          key_type;
	typedef typename parent_type::mapped_type       mapped_type;
	typedef typename parent_type::value_type        value_type;

	typedef typename parent_type::iterator          iterator;
	typedef typename parent_type::const_iterator    const_iterator;
#if 0
	typedef typename parent_type::allocator_type    allocator_type;

	typedef typename parent_type::reference         reference;
	typedef typename parent_type::const_reference   const_reference;

	typedef typename parent_type::size_type         size_type;
	typedef typename parent_type::difference_type   difference_type;
	typedef typename parent_type::pointer           pointer;
	typedef typename parent_type::const_pointer     const_pointer;
	typedef typename parent_type::allocator_type    allocator_type;
#endif

public:

	/**
		Wrapper to parent's lookup operator, may modify the set.  
	 */
	mapped_type&
	operator [] (const key_type& k);

	/**
		Must return by value, because cannot return temporary
		by reference, in the event where the mapped value is not
		found.  
		This call to find() should be const.  
		See "null_construct.h" for definition with pointer 
		specialization.  
	 */
	mapped_type
	operator [] (const key_type& k) const;

	// ALL other methods are inherited

};	// end class const_assoc_query

//=============================================================================
}	// end namespace util

#endif	// __UTIL_CONST_ASSOC_QUERY_H__

