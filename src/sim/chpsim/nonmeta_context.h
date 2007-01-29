/**
	\file "sim/chpsim/nonmeta_context.h"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.h,v 1.2.2.1 2007/01/29 04:44:13 fang Exp $
 */
#ifndef	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__
#define	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

#include "Object/nonmeta_context.h"
#include "Object/nonmeta_variable.h"	// for event_subscribers_type
#include "util/STL/vector_fwd.h"
#include "sim/common.h"
#include "util/member_saver.h"
#include "sim/chpsim/devel_switches.h"
#if CHPSIM_TRACING
#include "util/memory/excl_ptr.h"
#endif

namespace HAC {
namespace SIM {
namespace CHPSIM {
class EventNode;
class State;
using entity::footprint;
using entity::footprint_frame;
using entity::state_manager;
using entity::nonmeta_state_manager;
using entity::nonmeta_context_base;
using entity::event_subscribers_type;
#if CHPSIM_TRACING
using util::memory::never_ptr;
class TraceManager;
#endif

//=============================================================================
/**
	Context information for lookup up run-time values from state.  
	This is now tied to CHPSIM data structures.  
 */
class nonmeta_context : public nonmeta_context_base {
	typedef	nonmeta_context			this_type;
friend class EventNode;
	// these types must correspond to those used in CHPSIM::State!
	// but I'm too lazy to include its header here...
	typedef	EventNode			event_type;
	typedef	std::default_vector<event_index_type>::type
							enqueue_queue_type;
	typedef	std::default_vector<event_type>::type	event_pool_type;
private:
	/**
		Global process index, for looking up footprint frames.  
		Zero-value means top-level.
	 */
	event_type*				event;
	/**
		may need that enqueue_list again...
		deterministic selections may need to enqueue 
		successor events directly.  
	 */
	enqueue_queue_type&			enqueue_list;
public:
	/**
		Set of events to re-evaluate, so see if they 
		can move from pending (blocked) to execute.  
	 */
	event_subscribers_type&			rechecks;
	/**
		Global pool of events.  
	 */
	event_pool_type&			event_pool;
#if CHPSIM_TRACING
	never_ptr<TraceManager>			trace_manager;
#endif

	typedef	util::member_saver<this_type, event_type*, &this_type::event>
						event_setter_base;
	struct event_setter : public util::member_saver<this_type, 
			event_type*, &this_type::event> {
		event_setter(const this_type& t, event_type* e) :
			event_setter_base(const_cast<this_type&>(t), e) { }
	};

public:
	nonmeta_context(const state_manager&, const footprint&, 
		event_type&, State&);

	nonmeta_context(const state_manager&, const footprint&, 
		State&);

	~nonmeta_context();

	void
	set_event(event_type&);

	event_type&
	get_event(void) const { return *event; }

	void
	subscribe_this_event(void) const;

private:
	// for EventNode only
	void
	enqueue(const event_index_type) const;

};	// end class nonmeta_context

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

