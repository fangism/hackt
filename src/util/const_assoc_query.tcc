/**
	\file "util/const_assoc_query.tcc"
	Definition for map-like class adapter with const-semantics lookup.
	$Id: const_assoc_query.tcc,v 1.5 2005/09/05 05:04:51 fang Exp $
 */

#ifndef	__UTIL_CONST_ASSOC_QUERY_TCC__
#define	__UTIL_CONST_ASSOC_QUERY_TCC__

#include "util/const_assoc_query.h"
#include "util/assoc_traits.h"		// contains STL specializations
#include "util/null_construct.h"

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
	const const_iterator i = find(k);
	return (i != this->end()) ?
		value_selector()(*i) :
		null_construct<mapped_type>();
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_CONST_ASSOC_QUERY_TCC__

