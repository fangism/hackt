/**
	\file "multikey_set.tcc"
	Method definitions for multidimensional set, based on
	multikey_assoc wrapper interface. 
	$Id: multikey_set.tcc,v 1.1.2.1 2005/02/08 06:41:24 fang Exp $
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
	const mapped_type def(null_construct<mapped_type>());  // default value
	iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; ) {
		if (i->get_value() == def) {
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
		o << i->self_key() << " = " << i->get_value() << endl;
	return o;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_SET_TCC__

