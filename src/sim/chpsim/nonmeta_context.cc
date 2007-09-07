/**
	\file "sim/chpsim/nonmeta_context.cc"
	$Id: nonmeta_context.cc,v 1.5.12.3 2007/09/07 01:33:24 fang Exp $
 */

#include <vector>
// #include <iterator>
#include "sim/chpsim/nonmeta_context.h"
#include "sim/chpsim/Event.h"
#include "sim/chpsim/State.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/traits/proc_traits.h"
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
#include "Object/lang/CHP_event.h"
#else
#include "util/STL/valarray_iterator.h"
#endif
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using entity::process_tag;
//=============================================================================
// class nonmeta_context method definitions

#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
/**
	If process_index is 0 duplicate the top-level footprint as the 
	local footprint, else use the global entry's local footprint pointer.  
 */
nonmeta_context::nonmeta_context(const state_manager& s, 
		const footprint& f, 
		event_type& e, 
		State& r) :
		nonmeta_context_base(s, f, 
			(e.get_process_index() ?
				&s.get_pool<process_tag>()
				[e.get_process_index()]._frame
				: NULL),
			r.instances),
		event(&e), 
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		global_event_offset(0), 	// any invalid value
		process_index(0),
#endif
		first_checks(), 
		updates(r.__updated_list),
		event_pool(r.event_pool), 
		trace_manager(r.get_trace_manager_if_tracing())
		{
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor without event type argument.  
	A delegating constructor would be nice...
 */
nonmeta_context::nonmeta_context(const state_manager& s, 
		const footprint& f, 
		State& r) :
		nonmeta_context_base(s, f, NULL, r.instances),
		event(NULL), 
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		global_event_offset(0), 	// any invalid value
		process_index(0),
#endif
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
nonmeta_context::set_event(event_type& e
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		, const size_t pid
		, const event_index_type offset
#endif
		) {
	event = &e;
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	global_event_offset = offset;
	process_index = pid;
#else
	const size_t pid = e.get_process_index();
#endif
	fpf = (pid ? &sm->get_pool<process_tag>()[pid]._frame : NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Subscribe this event to its dependencies.
	Event index is deduced using iterator distance and event pool.  
	Only called by CHP::nondeterministic_selection::execute()!
 */
void
nonmeta_context::subscribe_this_event(void) const {
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
#if 0
/**
	Use offset to lookup pid.
 */
event_index_type
nonmeta_context::get_proces_index(void) const {
	return state.get_process_id(global_event_offset);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_context::insert_first_checks(const event_index_type ei) {
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	first_checks.insert(ei +global_event_offset);
#else
	first_checks.insert(ei);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enqueue all successors to first-check queue.  
	Only call this for non-selection events.
 */
void
nonmeta_context::first_check_all_successors(void) {
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	const event_type::local_event_type::successor_list_type&
		l(event->get_local_event().successor_events);
	copy(l.begin(), l.end(), util::set_inserter(*this));
#else
	const event_type::successor_list_type&
		l(event->successor_events);
	copy(std::begin(l), std::end(l), util::set_inserter(*this));
#endif
	// use set_inserter to automatically transform with offset
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

