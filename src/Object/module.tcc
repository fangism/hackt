/**
	\file "Object/art_object_module.tcc"
	Template method definitions for the module class.
	$Id: module.tcc,v 1.3 2005/09/04 21:14:40 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_MODULE_TCC__
#define	__OBJECT_ART_OBJECT_MODULE_TCC__

#include "Object/common/namespace.tcc"
#include "Object/module.h"

namespace ART {
namespace entity {
//=============================================================================/// class module method definitions

template <class L>
void
module::collect(L& l) const {
	// first collect namespaces into a flat list, then operate on it.
	// this really should be cacheable... but that would require
	// a coherent mechanism for invalidation... bah!
	namespace_collection_type nspcs;
	collect_namespaces(nspcs);
	namespace_collection_type::const_iterator i(nspcs.begin());
	const namespace_collection_type::const_iterator e(nspcs.end());
	// for_each(...) ?
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->collect(l);
	}
}

//=============================================================================
}	// end namepace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_MODULE_TCC__

