/**
	\file "Object/state_manager.tcc"
	$Id: state_manager.tcc,v 1.1 2006/11/02 22:01:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_STATE_MANAGER_TCC__
#define	__HAC_OBJECT_STATE_MANAGER_TCC__

#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/entry_collection.h"
#include "util/stacktrace.h"
#include "util/memory/index_pool.tcc"

namespace HAC {
namespace entity {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
size_t
state_manager::allocate(void) {
	return global_entry_pool<Tag>::allocate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
size_t
state_manager::allocate(const typename global_entry_pool<Tag>::entry_type& t) {
	return global_entry_pool<Tag>::allocate(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, just written to assist instantiation.  
 */
template <class Tag>
void
state_manager::__allocate_test(void) {
	const typename global_entry_pool<Tag>::entry_type	_e;
	allocate<Tag>();
	allocate<Tag>(_e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now, quick and recursive implementation, 
	could do this in a worklist fashion if performance really mattered.  
 */
template <class Tag>
void
state_manager::collect_subentries(entry_collection& e, const size_t i) const {
	const global_entry_pool<Tag>& p(get_pool<Tag>());
	// already bounds checked?
	INVARIANT(i);
	const global_entry<Tag>& g(p[i]);
	index_set_type& v(e.template get_index_set<Tag>());
	// insert returns .second=true if new element was inserted
	// otherwise, we've already visited this substructure.  
	if (v.insert(i).second) {
		g.collect_subentries(e, *this);
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_STATE_MANAGER_TCC__
