/**
	\file "multikey_set.tcc"
	Method definitions for multidimensional set, based on
	multikey_assoc wrapper interface. 
	$Id: multikey_set.tcc,v 1.1.4.1.2.3 2005/02/14 21:35:36 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_SET_TCC__
#define	__UTIL_MULTIKEY_SET_TCC__

#include <iostream>
#include "multikey_set.h"
#include "multikey.tcc"
#include "multikey_assoc.tcc"
#include "null_construct.h"

namespace util {
#include "using_ostream.h"
//=============================================================================
// class multikey_set method definition

MULTIKEY_SET_TEMPLATE_SIGNATURE
multikey_set<D,T,S>::multikey_set() : set_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MULTIKEY_SET_TEMPLATE_SIGNATURE
multikey_set<D,T,S>::~multikey_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_SET_TEMPLATE_SIGNATURE
void
multikey_set<D,T,S>::clean(void) {
	const impl_value_type def(null_construct<impl_value_type>());
		// default value
	iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; ) {
		/**
			We leverage the specialization of 
			_Select1st and _Select2nd in "maplikeset.h"
			to select the key and value when
			the implementation type is not a pair.  
		 */
#if 0
		if (i->second == def)
#else
		if (std::_Select2nd<value_type>()(*i) == def)
#endif
		{
			iterator j = i;
			j++;
			set_type::erase(i);
			i = j;
		} else {
			i++;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_SET_TEMPLATE_SIGNATURE
ostream&
multikey_set<D,T,S>::dump(ostream& o) const {
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++)
#if 0
		o << i->first << " = " << i->second << endl;
#else
		o << std::_Select1st<value_type>()(*i) << " = " <<
			std::_Select2nd<value_type>()(*i) << endl;
#endif
	return o;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_SET_TCC__

