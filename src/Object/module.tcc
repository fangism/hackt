/**
	\file "Object/module.tcc"
	Previously named "Object/art_object_module.tcc"
	Template method definitions for the module class.
	$Id: module.tcc,v 1.9 2010/04/02 22:17:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_MODULE_TCC__
#define	__HAC_OBJECT_MODULE_TCC__

#include "Object/common/namespace.h"
#include "Object/common/namespace.tcc"
#include "Object/module.h"
#include "Object/inst/alias_matcher.h"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	\param a the list of aliases to accumulate matches.
	\param i the globally allocated (canonical) index to match.  
 */
template <class Tag>
void
module::match_aliases(util::string_list& a, const size_t i) const {
	INVARIANT(this->is_allocated());
	typedef	alias_matcher<Tag>		matcher_type;
	const footprint& _fp(get_footprint());
	matcher_type m(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		this->global_state, 
#endif
		_fp, NULL, a, i);
	_fp.accept(m);
}
#endif

//=============================================================================
}	// end namepace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_MODULE_TCC__

