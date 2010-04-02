/**
	\file "Object/state_manager.tcc"
	$Id: state_manager.tcc,v 1.3 2010/04/02 22:18:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_STATE_MANAGER_TCC__
#define	__HAC_OBJECT_STATE_MANAGER_TCC__

#include <iostream>
#include "Object/state_manager.h"
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/global_entry.h"
#include "Object/entry_collection.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/lang/cflat_visitor.h"
#include "util/stacktrace.h"
#include "util/memory/index_pool.tcc"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 	Could just move this definition to cflat_visitor, no on else uses it.
 */
template <class Tag>
void
global_entry_pool<Tag>::accept(PRS::cflat_visitor& v) const {
	STACKTRACE_VERBOSE;
	size_t j = 1;
	// NOTE: skip index 0, which is reserved for top-level process!
try {
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; ++i, ++j) {
		i->accept(v);
	}
} catch (...) {
	cerr << "FATAL: error during processing of " <<
		class_traits<Tag>::tag_name << " id " << j
		<< "." << endl;
#if 0
	cerr << "\tinstance: ";
	proc_entry_pool[pid].dump_canonical_name(cerr, topfp, sm) << endl;
#endif
	// topfp footprint is not available here, pass pid in exception
	throw PRS::cflat_visitor::instance_exception<Tag>(j);
}
}

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
 	Could just move this definition to cflat_visitor, no on else uses it.
 */
template <class Tag>
void
state_manager::__accept(PRS::cflat_visitor& v) const {
	global_entry_pool<Tag>::accept(v);
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
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

#endif	// __HAC_OBJECT_STATE_MANAGER_TCC__
