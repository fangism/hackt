// "sublist.h"
// list slices

#ifndef	__SUBLIST_H__
#define	__SUBLIST_H__

#include <list>
using std::list;

// namespace std?

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
public:
	sublist(const sublist<T>& l) :
		the_list(l.the_list), head(l.head), tail(l.tail), 
		rhead(l.rhead), rtail(l.rtail) { }
	sublist(const list<T>& l) :
		the_list(l), head(l.begin()), tail(l.end()), 
		rhead(l.rbegin()), rtail(l.rend()) { }
virtual	~sublist() { }

	bool empty() const { return (head == tail); }
//	bool empty() const { return (rhead == rtail); }	// equivalent

	const_iterator begin(void) const { return head; }
	const_iterator end(void) const { return tail; }
	const_reverse_iterator rbegin(void) const { return rhead; }
	const_reverse_iterator rend(void) const { return rtail; }

	sublist<T>& behead(void) {
		if (head != tail) {
			head++; rtail--;
		}
		return *this;
	}

	sublist<T>& betail(void) {
		if (rhead != rtail) {
			tail--; rhead++;
		}
		return *this;
	}

};

#endif	//	__SUBLIST_H__

