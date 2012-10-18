/**
	\file "Object/module.tcc"
	Previously named "Object/art_object_module.tcc"
	Template method definitions for the module class.
	$Id: module.tcc,v 1.10 2010/04/07 00:12:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_MODULE_TCC__
#define	__HAC_OBJECT_MODULE_TCC__

#include "Object/common/namespace.hh"
#include "Object/common/namespace.tcc"
#include "Object/module.hh"

namespace HAC {
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_MODULE_TCC__

