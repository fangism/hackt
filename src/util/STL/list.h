/**
	\file "list.h"
	Class declaration for std::list wrapper.
	$Id: list.h,v 1.1 2004/12/05 05:07:57 fang Exp $
 */

#ifndef	__UTIL_STL_LIST_H__
#define	__UTIL_STL_LIST_H__

#include "STL/list_fwd.h"

#if !USE_STD_LIST
#include <list>

#define	STL_LIST_TEMPLATE_SIGNATURE	template <class T, class Alloc>

/**
	Experimenting around with code generated, comparing
	containership vs. inheritance.  
 */
#define LIST_INHERITANCE		1

/**
	Manual inline level!
	Not used yet...
 */
#define LIST_INLINE_LEVEL		0


#if LIST_INHERITANCE
	#define	LIST_PARENT		list_type::
#else
	#define	LIST_PARENT		the_list.
#endif

namespace util {
namespace STL {

/**
	Wrapper around std::list, for the sake of splitting declarations
	from definitions, to control template instantiation.  
	If inheritance is used, then this class introduces no new members
	so there is no problem with non-virtual base destructor.  
	\param T the value type contained.
	\param Alloc the allocator type, default to std::allocator<T>.
 */
STL_LIST_TEMPLATE_SIGNATURE
class list
#if LIST_INHERITANCE
: private std::list<T,Alloc>
#endif
{
private:
	typedef	std::list<T,Alloc>			list_type;
#if !LIST_INHERITANCE
	list_type					the_list;
#endif
public:
	typedef	typename list_type::value_type		value_type;
	typedef	typename list_type::pointer		pointer;
	typedef	typename list_type::const_pointer	const_pointer;
	typedef	typename list_type::reference		reference;
	typedef	typename list_type::const_reference	const_reference;
	typedef	typename list_type::iterator		iterator;
	typedef	typename list_type::const_iterator	const_iterator;
	typedef	typename list_type::reverse_iterator	reverse_iterator;
	typedef	typename list_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	typename list_type::size_type		size_type;
	typedef	typename list_type::difference_type	difference_type;
	typedef	typename list_type::allocator_type	allocator_type;

public:
	explicit
	list(const allocator_type& __a = allocator_type());

	list(size_type __n, const value_type& __value,
		const allocator_type& __a = allocator_type());

	explicit
	list(size_type __n);

	/// pseudo copy-constructor, accepts base type
	list(const list_type& __x);

	template<typename _InputIterator>
	list(_InputIterator __first, _InputIterator __last,
		const allocator_type& __a = allocator_type());

	~list();

	/// note: this accepts the base type of list
	list_type&
	operator=(const list_type& __x);

#if 0
	/// implicit interoperability with std::list
	operator list_type& () { return the_list; }

	/// implicit interoperability with std::list
	operator const list_type& () const { return the_list; }
#endif

	void
	assign(size_type __n, const value_type& __val);

	template<typename _InputIterator>
	void
	assign(_InputIterator __first, _InputIterator __last);

	allocator_type
	get_allocator() const;

	iterator
	begin(void) { return LIST_PARENT begin(); }

	const_iterator
	begin(void) const { return LIST_PARENT begin(); }

	iterator
	end(void) { return LIST_PARENT end(); }

	const_iterator
	end(void) const { return LIST_PARENT end(); }

	reverse_iterator
	rbegin(void) { return LIST_PARENT rbegin(); }

	const_reverse_iterator
	rbegin(void) const { return LIST_PARENT rbegin(); }

	reverse_iterator
	rend(void) { return LIST_PARENT rend(); }

	const_reverse_iterator
	rend(void) const { return LIST_PARENT rend(); }

	bool
	empty(void) const { return LIST_PARENT empty(); }

	size_type
	size(void) const { return LIST_PARENT size(); }

	size_type
	max_size(void) const { return LIST_PARENT max_size(); }

	reference
	front(void) { return LIST_PARENT front(); }

	const_reference
	front(void) const { return LIST_PARENT front(); }

	reference
	back(void) { return LIST_PARENT back(); }

	const_reference
	back(void) const { return LIST_PARENT back(); }

	void
	push_front(const value_type& v) { LIST_PARENT push_front(v); }

	void
	pop_front(void) { LIST_PARENT pop_front(); }

	void
	push_back(const value_type& v) { LIST_PARENT push_back(v); }

	void
	pop_back(void) { LIST_PARENT pop_back(); }

	// note: using base list type
	void
	swap(list_type& __x) {
#if LIST_INHERITANCE
		LIST_PARENT swap(__x);
#endif
	}

#if !LIST_INHERITANCE
	void
	swap(list& __x) {
		LIST_PARENT swap(__x.the_list);
	}
#endif

	void
	resize(size_type __new_size, const value_type& __x);

	void
	resize(size_type __new_size);

	iterator
	insert(iterator __position, const value_type& __x);

	void
	insert(iterator __pos, size_type __n, const value_type& __x);

	template<typename _InputIterator>
	void
	insert(iterator __pos, _InputIterator __first, _InputIterator __last);

	iterator
	erase(iterator __position);

	iterator
	erase(iterator __first, iterator __last);

	void
	clear(void);

	void
	splice(iterator __position, list& __x);

	void
	splice(iterator __position, list&, iterator __i);

	void
	splice(iterator __position, list&, iterator __first, iterator __last);

	void
	remove(const T& __value);

	template<typename _Predicate>
	void
	remove_if(_Predicate);

	void
	unique(void);

	template<typename _BinaryPredicate>
	void
	unique(_BinaryPredicate);

	void
	merge(list& __x);

	template<typename _StrictWeakOrdering>
	void
	merge(list&, _StrictWeakOrdering);

	void
	reverse(void);

	void
	sort(void);

	template<typename _StrictWeakOrdering>
	void
	sort(_StrictWeakOrdering);

};	// end class list

// comparison operators

}
}

// not intended for external use
#undef LIST_PARENT

#endif	// USE_STD_LIST

#endif	// __UTIL_STL_LIST_H__

