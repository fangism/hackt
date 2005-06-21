/**
	\file "util/multikey_set.tcc"
	Method definitions for multidimensional set, based on
	multikey_assoc wrapper interface. 
	$Id: multikey_set.tcc,v 1.3.12.1 2005/06/21 01:08:25 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_SET_TCC__
#define	__UTIL_MULTIKEY_SET_TCC__

#include "util/multikey_set.h"

#ifndef	EXTERN_TEMPLATE_UTIL_MULTIKEY_SET

#include <iostream>
#include "util/null_construct.h"

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MULTIKEY_SET
#define	EXTERN_TEMPLATE_UTIL_MULTIKEY
#define	EXTERN_TEMPLATE_UTIL_MULTIKEY_ASSOC
#endif

#include "util/multikey.tcc"
#include "util/multikey_assoc.tcc"

namespace util {
#include "util/using_ostream.h"
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
		if (std::_Select2nd<value_type>()(*i) == def) {
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
		o << std::_Select1st<value_type>()(*i) << " = " <<
			std::_Select2nd<value_type>()(*i) << endl;
	return o;
}

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_MULTIKEY_SET
#endif	// __UTIL_MULTIKEY_SET_TCC__

