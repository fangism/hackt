/**
	\file "sim/chpsim/Event.h"
	Various classes of chpsim events.  
	$Id: Event.h,v 1.1.2.10 2006/12/19 23:44:11 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENT_H__
#define	__HAC_SIM_CHPSIM_EVENT_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include "util/string_fwd.h"
#include <iosfwd>
#include <valarray>
#include <vector>
#include "util/memory/count_ptr.h"
#include "sim/chpsim/Dependence.h"
#include "sim/chpsim/type_enum.h"
// #include "util/STL/vector_fwd.h"
#include "sim/chpsim/InstancePools.h"

namespace HAC {
namespace entity {
	class bool_expr;
	class state_manager;
namespace CHP {
	class action;
}
}
namespace SIM {
namespace CHPSIM {
class DependenceSetCollector;
using std::ostream;
using std::string;
using std::vector;
using std::valarray;
using entity::bool_expr;
using entity::state_manager;
using entity::CHP::action;
using util::memory::count_ptr;

//=============================================================================
/**
	Plan is to have different event pools... not anymore! is unified.
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
	typedef	EventNode		this_type;
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

	// need to override because valarray doesn't implement 
	// operator = using default container behavior
	this_type&
	operator = (const this_type&);

	void
	set_guard_expr(const count_ptr<const bool_expr>&);

	void
	set_event_type(const unsigned short t) { event_type = t; }

	void
	set_predecessors(const event_index_type n) { predecessors = n; }

	void
	import_dependencies(const DependenceSetCollector& d) {
		deps.import(d);
	}

	// return: references to changed state?
	void
	execute(const state_manager&, InstancePools&, 
		vector<instance_reference>&);

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_dot_node(ostream&) const;

	ostream&
	dump_dot_edge(ostream&) const;

};	// end class EventNode

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_EVENT_H__

