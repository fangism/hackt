/**
	\file "Object/def/footprint.tcc"
	Exported template implementation of footprint base class. 
	$Id: footprint.tcc,v 1.2.88.3 2010/01/29 02:39:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_TCC__
#define	__HAC_OBJECT_DEF_FOOTPRINT_TCC__

#include <iostream>
#include "Object/def/footprint.h"
#include "Object/inst/instance_pool.h"	// for pool_private_map_entry_type
#include "Object/inst/state_instance.h"
#include "Object/common/dump_flags.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class footprint_base method definitions

#if 0
// may not be needed after the last branch merge
template <class Tag>
footprint_base<Tag>::footprint_base() :
		_pool(class_traits<Tag>::instance_pool_chunk_size >> 1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
footprint_base<Tag>::~footprint_base() { }
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	This lookup could be critical?
	Return a reference to a unique object in the hierarchy
	that represents the type information.  
	\param Tag is the meta-type tag.
	\param gi is the global index being referenced.  0-based.
 */
template <class Tag>
const state_instance<Tag>&
footprint::get_instance(const size_t gi) const {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
//	STACKTRACE_VERBOSE;
	const pool_type& p(get_instance_pool<Tag>());
	const size_t offset = p.local_entries();
	if (gi < offset) {
		return p[gi];
	} else {
		const pool_private_map_entry_type&
			e(p.locate_private_entry(gi -offset));
		// e.first is the local process index (1-indexed)
		const state_instance<process_tag>::pool_type&
			ppool(get_instance_pool<process_tag>());
		INVARIANT(e.first <= ppool.local_entries());
		const state_instance<process_tag>& sp(ppool[e.first -1]);
		// e.second is the offset to subtract
		return sp._frame._footprint->get_instance<Tag>(gi -e.second);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param Tag is the meta-type tag.
	\param gi is the global index being referenced.  0-based.
 */
template <class Tag>
ostream&
footprint::dump_canonical_name(ostream& o, const size_t gi) const {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
//	STACKTRACE_VERBOSE;
	const pool_type& p(get_instance_pool<Tag>());
	const size_t offset = p.local_entries();
	if (gi < offset) {
		p[gi].get_back_ref()->dump_hierarchical_name(o, 
			dump_flags::no_definition_owner);
	} else {
		const pool_private_map_entry_type&
			e(p.locate_private_entry(gi -offset));
		// e.first is the local process index (1-indexed)
		const state_instance<process_tag>::pool_type&
			ppool(get_instance_pool<process_tag>());
		const size_t m = ppool.local_entries();
//		STACKTRACE_INDENT_PRINT("<e.first=" << e.first << '/' << m << '>' << endl);
		INVARIANT(e.first <= m);
		const state_instance<process_tag>& sp(ppool[e.first -1]);
		sp.get_back_ref()->dump_hierarchical_name(o, 
			dump_flags::no_definition_owner) << '.';
		// TODO: pass in dump_flags to honor hierarchical separator
		// e.second is the offset to subtract
		sp._frame._footprint->dump_canonical_name<Tag>(o, gi -e.second);
	}
	return o;
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_TCC__

