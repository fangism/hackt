/**
	\file "util/qmap.tcc"
	Template class method definitions for queryable map.  
	$Id: qmap.tcc,v 1.6 2006/01/22 06:53:36 fang Exp $
 */

#ifndef	__UTIL_QMAP_TCC__
#define	__UTIL_QMAP_TCC__

#include "util/qmap.h"

// predefine to suppress template definition
#ifndef	EXTERN_TEMPLATE_UTIL_QMAP

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_QMAP
// don't bother...
#endif

#include "util/const_assoc_query.tcc"

namespace util {
/**
	Purges map of entries that are just default values, 
	useful for removing null pointers for maps of pointers
	(where semantically appropriate).  
	Requirement: T must be comparable for equality.  
 */
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T,C,A>::clean(void) {
	const mapped_type def = null_construct<T>();  // default value
	iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; ) {
		if (i->second == def) {
			iterator j = i;
			j++;
			this->erase(i);
			i = j;
		} else {
			i++;
		}
	}
}

}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_QMAP

#endif	// __UTIL_QMAP_TCC__

