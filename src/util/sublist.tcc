/**
	\file "sublist.tcc"
	Template method definitions for sublist.
	$Id: sublist.tcc,v 1.1.60.1 2005/04/09 23:10:11 fang Exp $
 */

#ifndef	__UTIL_SUBLIST_TCC__
#define	__UTIL_SUBLIST_TCC__

// #include "list.tcc"
// not needed because we only need the list iterator

#include "sublist.h"

namespace util {
//=============================================================================
// class sublist method definitions

SUBLIST_TEMPLATE_SIGNATURE
sublist<T>::sublist(const sublist& l) :
//		the_list(l.the_list),
		head(l.head), tail(l.tail)
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
		, rhead(l.rhead), rtail(l.rtail)
#endif
{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SUBLIST_TEMPLATE_SIGNATURE
sublist<T>::sublist(const list_type& l) :
//		the_list(l),
		head(l.begin()), tail(l.end())
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
		, rhead(l.rbegin()), rtail(l.rend())
#endif
{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SUBLIST_TEMPLATE_SIGNATURE
sublist<T>::~sublist() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// inlined
SUBLIST_TEMPLATE_SIGNATURE
sublist<T>&
sublist<T>::behead(void) {
	if (head != tail) {
		head++;
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
		rtail--;
#endif
	}
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SUBLIST_TEMPLATE_SIGNATURE
sublist<T>&
sublist<T>::betail(void) {
	if (head != tail) {
		tail--;
#if SUBLIST_INCLUDE_REVERSE_ITERATORS
		rhead++;
#endif
	}
	return *this;
}
#endif

//=============================================================================
}	// end namespace util

#endif	// __UTIL_SUBLIST_TCC__

