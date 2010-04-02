/**
	\file "sim/chpsim/nonmeta_context.cc"
	$Id: nonmeta_context.cc,v 1.7 2010/04/02 22:19:14 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <vector>
#include "sim/chpsim/nonmeta_context.h"
#include "sim/chpsim/Event.h"
#include "sim/chpsim/State.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/def/footprint.h"
#else
#include "Object/state_manager.h"
#endif
#include "Object/global_entry.h"
#include "Object/traits/proc_traits.h"
#include "Object/lang/CHP_event.h"
#include "util/iterator_more.h"
#include "common/TODO.h"
#include "util/stacktrace.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using entity::process_tag;
//=============================================================================
// class nonmeta_context method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor without event type argument.  
	A delegating constructor would be nice...
 */
nonmeta_context::nonmeta_context(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& s, 
		const footprint& f, 
#endif
		State& r) :
		nonmeta_context_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			s, f, 
#else
			*r.top_context.fpf,
			*r.top_context.parent_offset, 
#endif
			r.instances),
		event(NULL), 
		global_event_offset(0), 	// any invalid value
		process_index(0),
		first_checks(), 
		updates(r.__updated_list),
		event_pool(r.event_pool), 
		trace_manager(r.get_trace_manager_if_tracing())
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_context::~nonmeta_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_context::set_event(
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_base& sb,
#endif
		event_type& e,
		const size_t pid, const event_index_type offset) {
	STACKTRACE_VERBOSE;
	event = &e;
	global_event_offset = offset;
	process_index = pid;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
//	fpf = &sb.get_footprint_frame(pid);
	NEVER_NULL(fpf);
#else
	fpf = (pid ? &sm->get_pool<process_tag>()[pid]._frame : NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Subscribe this event to its dependencies.
	Event index is deduced using iterator distance and event pool.  
	Only called by CHP::nondeterministic_selection::execute()!
 */
void
nonmeta_context::subscribe_this_event(void) const {
	STACKTRACE_VERBOSE;
	const event_index_type d = std::distance(&event_pool[0], event);
	INVARIANT(d < event_pool.size());
	event->subscribe_deps(*this, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
nonmeta_context::get_event_index(void) const {
	INVARIANT(event);
	return std::distance(&event_pool[0], event);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Add offset to translate from local to global event index!
 */
void
nonmeta_context::insert_first_checks(const event_index_type ei) {
	STACKTRACE_VERBOSE;
	first_checks.insert(ei +global_event_offset);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enqueue all successors to first-check queue.  
	Only call this for non-selection events.
 */
void
nonmeta_context::first_check_all_successors(void) {
	STACKTRACE_VERBOSE;
	const event_type::local_event_type::successor_list_type&
		l(event->get_local_event().successor_events);
	copy(l.begin(), l.end(), util::set_inserter(*this));
	// use set_inserter to automatically transform with offset
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

