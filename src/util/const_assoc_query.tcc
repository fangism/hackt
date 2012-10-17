/**
	\file "util/const_assoc_query.tcc"
	Definition for map-like class adapter with const-semantics lookup.
	$Id: const_assoc_query.tcc,v 1.7 2006/05/06 04:18:57 fang Exp $
 */

#ifndef	__UTIL_CONST_ASSOC_QUERY_TCC__
#define	__UTIL_CONST_ASSOC_QUERY_TCC__

#include "util/STL/functional.hh"	// in case _Select is missing
#include "util/const_assoc_query.hh"
#include "util/assoc_traits.hh"		// contains STL specializations
#include "util/null_construct.hh"

namespace util {
//=============================================================================
// class const_assoc_query method definitions

template <class M>
typename const_assoc_query<M>::mapped_type&
const_assoc_query<M>::operator [] (const key_type& k) {
	return parent_type::operator[](k);
}

template <class M>
typename const_assoc_query<M>::mapped_type
const_assoc_query<M>::operator [] (const key_type& k) const {
	typedef typename assoc_traits<M>::value_selector	value_selector;
	const const_iterator i(parent_type::find(k));
	return (i != this->end()) ?
		value_selector()(*i) :
		null_construct<mapped_type>();
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_CONST_ASSOC_QUERY_TCC__

