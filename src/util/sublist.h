/**
	\file "sublist.h"
	Wrapper class interface for list slices, that behave like lists.  
	$Id: sublist.h,v 1.5 2004/12/05 05:07:25 fang Exp $
 */

#ifndef	__SUBLIST_H__
#define	__SUBLIST_H__

#include "STL/list.h"

#define SUBLIST_TEMPLATE_SIGNATURE		template <class T>
#define	SUBLIST_INCLUDE_REVERSE_ITERATORS	0

//=============================================================================
namespace util {
//=============================================================================
/**
	Sub-list with only const semantics.  
	Behaves like a const list.  
	Really only needs to be an iterator pair, a range.  
	Want to be able to pass truncated versions.  
 */
SUBLIST_TEMPLATE_SIGNATURE
class sublist {
public:
	typedef	LIST_NAMESPACE::list<T>			list_type;
	typedef	typename list_type::iterator		iterator;
	typedef	typename list_type::const_iterator	const_iterator;
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
	typedef	typename list_type::reverse_iterator	reverse_iterator;
	typedef	typename list_type::const_reverse_iterator
							const_reverse_iterator;
#endif
protected:
#if 0
	const list_type&	the_list;	// actual referenced list
		// do we actually use the_list?
#endif
	// set of semi-redundant iterators
	const_iterator		head;
	const_iterator		tail;
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
	const_reverse_iterator	rhead;
	const_reverse_iterator	rtail;
#endif

#if 0
static const list<T>		bogus_empty_list;
public:
	// default empty constructor?
	sublist() : the_list(bogus_empty_list),
		head(bogus_empty_list.begin()),
		tail(bogus_empty_list.end()), 
		rhead(bogus_empty_list.rbegin()),
		rtail(bogus_empty_list.rend()) { }
#endif

public:
	// no default constructor because the_list is a reference

	/** Standard copy constructor */
	sublist(const sublist<T>& l);

	/**
		Implicit constructor to covert from a list to a list slice.  
	 */
	sublist(const list_type& l);

	/** Standard destructor */
	~sublist();

	bool
	empty() const { return (head == tail); }

	size_t
	size(void) const { return distance(head, tail); }

	const_iterator
	begin(void) const { return head; }

	const_iterator
	end(void) const { return tail; }

#if SUBLIST_INCLUDE_REVERSE_ITERATORS
	const_reverse_iterator
	rbegin(void) const { return rhead; }

	const_reverse_iterator
	rend(void) const { return rtail; }
#endif

	/** Removes the first element from the slice. */
	sublist<T>&
	behead(void) {
		if (head != tail) {
			head++;
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
			rtail--;
#endif
		}
		return *this;
	}

	/** alias for behead() */
	void
	pop_front(void) { behead(); }

	/** Removes the last element from the slice. */
	sublist<T>&
	betail(void) {
		if (head != tail) {
			tail--;
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
			rhead++;
#endif
		}
		return *this;
	}

	/** alias for betail() */
	void
	pop_back(void) { betail(); }

// no push methods, slices may only shrink...

};	// end class sublist
//=============================================================================

}	// end namespace util
//=============================================================================

#endif	//	__SUBLIST_H__

