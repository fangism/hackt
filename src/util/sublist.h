// "sublist.h"
// list slices

#ifndef	__SUBLIST_H__
#define	__SUBLIST_H__

#include <list>

#ifndef	SUBLIST_NAMESPACE
#define	SUBLIST_NAMESPACE		sublist_ns
#endif

//=============================================================================
namespace SUBLIST_NAMESPACE {
using std::list;

//=============================================================================
/**
	Sub-list with only const semantics.  
	Behaves like a const list.  
	Want to be able to pass truncated versions.  
 */
template <class T>
class sublist {
public:
	typedef	typename list<T>::iterator			iterator;
	typedef	typename list<T>::const_iterator		const_iterator;
	typedef	typename list<T>::reverse_iterator		reverse_iterator;
	typedef	typename list<T>::const_reverse_iterator	const_reverse_iterator;
protected:
	const list<T>&		the_list;	// actual referenced list
		// do we actually use the_list?
	// set of semi-redundant iterators
	const_iterator		head;
	const_iterator		tail;
	const_reverse_iterator	rhead;
	const_reverse_iterator	rtail;

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
	/** Standard copy constructor */
	sublist(const sublist<T>& l) :
		the_list(l.the_list), head(l.head), tail(l.tail), 
		rhead(l.rhead), rtail(l.rtail) { }

	/**
		Implicit constructor to covert from a list to a list slice.  
	 */
	sublist(const list<T>& l) :
		the_list(l), head(l.begin()), tail(l.end()), 
		rhead(l.rbegin()), rtail(l.rend()) { }

	/** Standard destructor */
virtual	~sublist() { }

	bool empty() const { return (head == tail); }
//	bool empty() const { return (rhead == rtail); }	// equivalent

	size_t size(void) const { return distance(head, tail); }

	const_iterator begin(void) const { return head; }
	const_iterator end(void) const { return tail; }
	const_reverse_iterator rbegin(void) const { return rhead; }
	const_reverse_iterator rend(void) const { return rtail; }

	/** Removes the first element from the slice. */
	sublist<T>& behead(void) {
		if (head != tail) {
			head++; rtail--;
		}
		return *this;
	}

	/** alias for behead() */
	void pop_front(void) { behead(); }

	/** Removes the last element from the slice. */
	sublist<T>& betail(void) {
		if (rhead != rtail) {
			tail--; rhead++;
		}
		return *this;
	}

	/** alias for betail() */
	void pop_back(void) { betail(); }

// no push methods, slices may only shrink...

};	// end class sublist
//=============================================================================

}	// end namespace SUBLIST_NAMESPACE
//=============================================================================

#endif	//	__SUBLIST_H__

