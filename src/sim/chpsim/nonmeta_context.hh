/**
	\file "sim/chpsim/nonmeta_context.hh"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.hh,v 1.8 2010/04/07 00:13:08 fang Exp $
 */
#ifndef	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__
#define	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

#include <vector>			// only need fwd decl
#include "Object/nonmeta_context.hh"
#include "Object/nonmeta_variable.hh"	// for event_subscribers_type
#include "Object/ref/reference_set.hh"
#include "sim/common.hh"
#include "util/member_saver.hh"
#include "util/memory/excl_ptr.hh"

namespace HAC {
namespace SIM {
class state_base;
namespace CHPSIM {
class EventNode;
class State;
using entity::footprint;
using entity::footprint_frame;
using entity::state_manager;
using entity::global_offset;
using entity::global_entry_context;
using entity::nonmeta_state_manager;
using entity::nonmeta_context_base;
using entity::event_subscribers_type;
using util::memory::never_ptr;
class TraceManager;

//=============================================================================
/**
	Context information for lookup up run-time values from state.  
	This is now tied to CHPSIM data structures.  
 */
class nonmeta_context : public nonmeta_context_base {
	typedef	nonmeta_context			this_type;
// TODO: is local_event enough, do we need global_event?
	// these types must correspond to those used in CHPSIM::State!
	// but I'm too lazy to include its header here...
	typedef	EventNode			event_type;
	typedef	std::vector<event_index_type>	enqueue_queue_type;
	typedef	std::vector<event_type>		event_pool_type;
	/// for set-insert interface to first_checks
	typedef	event_subscribers_type::const_iterator	const_iterator;
public:
	typedef	event_subscribers_type::value_type	value_type;
	typedef	event_subscribers_type::const_reference	const_reference;
private:
	/**
		Reference to the event in question.
	 */
	event_type*				event;
	/**
		Offset to add to local-event indices to 
		translate to global event indices.  
		Should be set by set_event.  
	 */
	event_index_type			global_event_offset;
	/**
		Current process index for this event.
		Mostly used for diagnostics.
	 */
	size_t					process_index;
	/**
		Successor of a just-executed event to check 
		for the first time, after their respective delays.  
		NOTE: this is a local structure now, not a reference!
	 */
	event_subscribers_type			first_checks;
public:
	/**
		List of references modified by the visiting event.
	 */
	entity::global_references_set&		updates;
	/**
		Global pool of events.  
	 */
	event_pool_type&			event_pool;
	/**
		Reference to the State's trace_manager.  
	 */
	never_ptr<TraceManager>			trace_manager;

	typedef	util::member_saver<this_type, event_type*, &this_type::event>
						event_setter_base;
	/**
		To be used by only EventExecutor please!
	 */
	struct event_setter : public util::member_saver<this_type, 
			event_type*, &this_type::event> {
		event_setter(const this_type& t, event_type* e) :
			event_setter_base(const_cast<this_type&>(t), e) { }
	};
public:
	nonmeta_context(
		// const global_entry_context& available from State&
		// may need to sub-class global_offset to map
		// offsets of events and other things!
		State&);

	~nonmeta_context();

	void
	set_event(const state_base&,	// for frame-cache
		event_type&, const size_t, const event_index_type);

	event_type&
	get_event(void) const { return *event; }

	/**
		The distance from the start of the pool is the index.
	 */
	size_t
	get_event_index(void) const;
	
	event_index_type
	get_process_index(void) const { return process_index; }

	void
	subscribe_this_event(void) const;

	void
	insert_first_checks(const event_index_type);

	void
	insert(const event_index_type ei) {
		insert_first_checks(ei);
	}

	const_iterator
	first_checks_begin(void) const { return first_checks.begin(); }

	const_iterator
	first_checks_end(void) const { return first_checks.end(); }

	void
	first_check_all_successors(void);

};	// end class nonmeta_context

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

