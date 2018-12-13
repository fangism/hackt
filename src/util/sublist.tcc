/**
	\file "util/sublist.tcc"
	Template method definitions for sublist.
	$Id: sublist.tcc,v 1.4 2005/06/21 21:26:39 fang Exp $
 */

#ifndef	__UTIL_SUBLIST_TCC__
#define	__UTIL_SUBLIST_TCC__

#include "util/sublist.hh"

#ifndef	EXTERN_TEMPLATE_UTIL_SUBLIST

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

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_SUBLIST
#endif	// __UTIL_SUBLIST_TCC__

