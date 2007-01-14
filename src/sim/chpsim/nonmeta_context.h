/**
	\file "sim/chpsim/nonmeta_context.h"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.h,v 1.1.4.4 2007/01/14 23:36:31 fang Exp $
 */
#ifndef	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__
#define	__HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

#include "Object/nonmeta_context.h"
#include "Object/nonmeta_variable.h"	// for event_subscribers_type
#include "util/STL/vector_fwd.h"

/**
	Define to 1 to allow context and callers to enqueue events
	directly into the staging event queue.  
	Goal: 0
	Rationale: should not allow this, force every event to recheck.  
 */
#define	CHPSIM_DIRECT_ENQUEUE		0

namespace HAC {
namespace SIM {
namespace CHPSIM {
class EventNode;
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
	typedef	EventNode				event_type;
	typedef	std::default_vector<size_t>::type	enqueue_queue_type;
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
#if CHPSIM_DIRECT_ENQUEUE
	/**
		This lists successor events to enqueue for execution.  
	 */
	enqueue_queue_type&			queue;
#endif
public:
	nonmeta_context(const state_manager&, const footprint&, 
		nonmeta_state_manager&, event_type&, 
		event_subscribers_type&
#if CHPSIM_DIRECT_ENQUEUE
		, enqueue_queue_type&
#endif
		);

	nonmeta_context(const state_manager&, const footprint&, 
		nonmeta_state_manager&, 
		event_subscribers_type&
#if CHPSIM_DIRECT_ENQUEUE
		, enqueue_queue_type&
#endif
		);

	~nonmeta_context();

	void
	set_event(event_type&);

	const event_type&
	get_event(void) const { return *event; }

	event_type&
	get_event(void) { return *event; }

#if CHPSIM_DIRECT_ENQUEUE
	void
	enqueue(const size_t);
#endif

	void
	schedule_recheck(const size_t i) const { rechecks.insert(i); }

};	// end class nonmeta_context

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_NONMETA_CONTEXT_H__

