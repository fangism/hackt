/**
	\file "sim/chpsim/nonmeta_context.h"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.h,v 1.1.4.6 2007/01/15 21:53:44 fang Exp $
 */
#ifndef	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__
#define	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

#include "Object/nonmeta_context.h"
#include "Object/nonmeta_variable.h"	// for event_subscribers_type
#include "util/STL/vector_fwd.h"

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


//=============================================================================
/**
	Context information for lookup up run-time values from state.  
	This is now tied to CHPSIM data structures.  
 */
class nonmeta_context : public nonmeta_context_base {
friend class EventNode;
	// these types must correspond to those used in CHPSIM::State!
	// but I'm too lazy to include its header here...
	typedef	EventNode				event_type;
	typedef	std::default_vector<size_t>::type	enqueue_queue_type;
	typedef	std::default_vector<event_type>::type	event_pool_type;
private:
	/**
		Global process index, for looking up footprint frames.  
		Zero-value means top-level.
	 */
	event_type*				event;
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
public:
	nonmeta_context(const state_manager&, const footprint&, 
		event_type&, State&);

	nonmeta_context(const state_manager&, const footprint&, 
		State&);

	~nonmeta_context();

	void
	set_event(event_type&);

	const event_type&
	get_event(void) const { return *event; }

	event_type&
	get_event(void) { return *event; }

};	// end class nonmeta_context

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

