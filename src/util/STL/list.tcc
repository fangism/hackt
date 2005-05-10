/**
	\file "util/STL/list.tcc"
	Class method definitions for std::list wrapper class.
	$Id: list.tcc,v 1.2 2005/05/10 04:51:32 fang Exp $
 */

#ifndef	__UTIL_STL_LIST_TCC__
#define	__UTIL_STL_LIST_TCC__

#include "util/STL/list.h"

#if !USE_STD_LIST

#if LIST_INHERITANCE
	#define	LIST_CONSTRUCT	list_type
	#define	LIST_PARENT	list_type::
#else
	#define	LIST_CONSTRUCT	the_list
	#define	LIST_PARENT	the_list.
#endif

namespace util {
namespace STL {
//=============================================================================
// class list method definitions

STL_LIST_TEMPLATE_SIGNATURE
list<T,Alloc>::list(const allocator_type& __a) :
		LIST_CONSTRUCT(__a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
list<T,Alloc>::list(size_type __n, const value_type& __value,
		const allocator_type& __a) :
		LIST_CONSTRUCT(__n, __value, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
list<T,Alloc>::list(size_type __n) :
		LIST_CONSTRUCT(__n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
list<T,Alloc>::list(const list_type& __x) :
		LIST_CONSTRUCT(__x) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
template<typename _InputIterator>
list<T,Alloc>::list(_InputIterator __first, _InputIterator __last,
		const allocator_type& __a) :
		LIST_CONSTRUCT(__first, __last, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
list<T,Alloc>::~list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
typename list<T,Alloc>::list_type&
list<T,Alloc>::operator = (const list_type& __x) {
#if LIST_INHERITANCE
	return list_type::operator = (__x);
#else
	the_list = __x;
	return *this;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::assign(size_type __n, const value_type& __val) {
	LIST_PARENT assign(__n, __val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
template<typename _InputIterator>
void
list<T,Alloc>::assign(_InputIterator __first, _InputIterator __last) {
	LIST_PARENT assign(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
typename list<T,Alloc>::allocator_type
list<T,Alloc>::get_allocator() const {
	return LIST_PARENT get_allocator();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::resize(size_type __new_size, const value_type& __x) {
	LIST_PARENT resize(__new_size, __x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::resize(size_type __new_size) {
	LIST_PARENT resize(__new_size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
typename list<T,Alloc>::iterator
list<T,Alloc>::insert(iterator __position, const value_type& __x) {
	return LIST_PARENT insert(__position, __x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::insert(iterator __pos, size_type __n, const value_type& __x) {
	return LIST_PARENT insert(__pos, __n, __x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
template<typename _InputIterator>
void
list<T,Alloc>::insert(iterator __pos, _InputIterator __first,
		_InputIterator __last) {
	LIST_PARENT insert(__pos, __first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
typename list<T,Alloc>::iterator
list<T,Alloc>::erase(iterator __position) {
	return LIST_PARENT erase(__position);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
typename list<T,Alloc>::iterator
list<T,Alloc>::erase(iterator __first, iterator __last) {
	return LIST_PARENT erase(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::clear(void) { LIST_PARENT clear(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::splice(iterator __position, list& __x) {
	LIST_PARENT splice(__position, __x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::splice(iterator __position, list& __x, iterator __i) {
	LIST_PARENT splice(__position, __x, __i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::splice(iterator __position, list& __x,
		iterator __first, iterator __last) {
	LIST_PARENT splice(__position, __x, __first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::remove(const T& __value) {
	LIST_PARENT remove(__value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
template<typename _Predicate>
void
list<T,Alloc>::remove_if(_Predicate __p) {
	LIST_PARENT remove_if(__p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::unique(void) {
	LIST_PARENT unique();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
template<typename _BinaryPredicate>
void
list<T,Alloc>::unique(_BinaryPredicate __p) {
	LIST_PARENT unique(__p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::merge(list& __x) { LIST_PARENT merge(__x.the_list); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
template<typename _StrictWeakOrdering>
void
list<T,Alloc>::merge(list& __x, _StrictWeakOrdering __o) {
	LIST_PARENT merge(__x, __o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::reverse(void) { LIST_PARENT reverse(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
void
list<T,Alloc>::sort(void) { LIST_PARENT sort(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_LIST_TEMPLATE_SIGNATURE
template<typename _StrictWeakOrdering>
void
list<T,Alloc>::sort(_StrictWeakOrdering __o) { LIST_PARENT sort(__o); }

//=============================================================================

}	// end namespace STL
}	// end namespace util

#undef	LIST_INHERITANCE
#undef	LIST_CONSTRUCT
#undef	LIST_PARENT

#endif	// USE_STD_LIST

#endif	// __UTIL_STL_LIST_TCC__

