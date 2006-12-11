/**
	\file "sim/chpsim/Event.h"
	Various classes of chpsim events.  
	$Id: Event.h,v 1.1.2.4 2006/12/11 00:40:19 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENT_H__
#define	__HAC_SIM_CHPSIM_EVENT_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include <iosfwd>
#include <valarray>
#include "util/memory/count_ptr.h"
#include "sim/chpsim/Dependence.h"

namespace HAC {
namespace entity {
	class bool_expr;
namespace CHP {
	class action;
}
}
namespace SIM {
namespace CHPSIM {
using std::ostream;
using std::valarray;
using entity::bool_expr;
using entity::CHP::action;
using util::memory::count_ptr;

//=============================================================================
/**
	Plan is to have different event pools.  
 */
enum {
	EVENT_NULL = 0,		///< can be used to mean 'skip' or 'no-op'
	EVENT_ASSIGN = 1,
	EVENT_SEND = 2,
	EVENT_RECEIVE = 3,
	EVENT_CONCURRENT_FORK = 4,	///< divergence of concurrent events
	EVENT_CONCURRENT_JOIN = EVENT_NULL,	///< convergence event (no-op)
	EVENT_SELECTION_BEGIN = 5,	///< the start of any selection
	EVENT_SELECTION_END = EVENT_NULL	///< end of any selection (no-op)
};

//-----------------------------------------------------------------------------
#if 0
/**
	Event representing change of variable value.  
	For now, we assign bools and ints, don't deal with structures yet.
	The lvalue reference is the result of run-time resolution, 
	in the case of non-meta indexed variables.  
	The rvalue is a resolved/updated value to assign.  
	Could also template this by type... bool, int.
	Aggregate assignments? not yet.
 */
struct assign_event {
	// lvalue -- some resolved reference to lvalue (bool, int)
	size_t				lvalue_type;
	// would be best to resolve ASAP
	size_t				lvalue_index;
	// the value to assign (currently, only integer)
	size_t				rvalue;
};	// end struct assign_event

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct send_event {
	size_t				lvalue_channel_index;
	// rvalue must be aggregate structure
};	// end struct send_event

//=============================================================================
/**
	The master event record for the event queue.  
	Q: TimeType? integer of floating point?
 */
struct event_placeholder {
	size_t				event_type;
	size_t				event_index;
};	// end struct event_placeholder
#endif

//=============================================================================
/**
	This represents a node in the event graph, which can be 
	thought of as a marked graph.  
	Events are generalized to have guards, though in many cases, 
	they will be NULL.  
	We 'allocate' graph nodes with back-references to their
	corresponding events in the CHP footprint.  
	The 'edges' between graph nodes form predecessor-successor relations.
	This will be used as a base class for other events.  
	TODO: align to natural boundary.
 */
class EventNode {
public:
	typedef	size_t			event_index_type;
private:
	/// wake-up guard expression for THIS event, if applicable
	count_ptr<const bool_expr>	guard_expr;
	/**
		the (atomic) event to occur corresponding to this node
		Would be nice if some of theses actions were 
		resolved to static references... (don't optimize now)
		Can be lightweight pointer instead of reference count?
	 */
	// count_ptr<const action>		action_ptr;
	const action*			action_ptr;
public:
	/**
		events that follow this event.  
		The interpretation of these events (concurrent, 
		deterministic, nondeterministic) is depending on the type.  
	 */
	valarray<event_index_type>	successor_events;
private:
#if 1
	/**
		enumeration for this event, 
		semi-redundant with the action pointer.  
	 */
	unsigned short			event_type;
#endif
#if 1
	/**
		General purpose flags (space-filler).  
	 */
	unsigned short			flags;
#endif
	/**
		footprint, footprint frame / global_entry<process>
		or just 0-based index to process entries.  (0 => top)
	 */
	size_t				process_index;
	/**
		The number of concurrent event predecessors.
		Wait for this number of events to precede before executing, 
		like a barrier count.  
		Sequential actions (in chain) only have one predecessor.  
		Selection statements re-join with only one predecessor.  
		Do we keep predecessor edge information anywhere else?
		Should be const, incidentally...
	 */
	unsigned short			predecessors;
	/**
		barrier count: from number of predecessors (join operation)
		Event fires when countdown reaches zero, post-decrement.  
		This is dynamic stateful information that needs 
		to be checkpointed.  
	 */
	unsigned short			countdown;
	/**
		Set of variables and channels whose update may affect the
		blocking state of this event.  
		Consider using an excl_ptr to this if 
		a significant number of events don't need this.  
	 */
	DependenceSet			deps;
public:
	EventNode();

	EventNode(const action*, const unsigned short, const size_t pid);

	~EventNode();

	void
	set_guard_expr(const count_ptr<const bool_expr>&);

	void
	set_predecessors(const event_index_type n) { predecessors = n; }

	ostream&
	dump_struct(ostream&) const;

};	// end class EventNode

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_EVENT_H__

