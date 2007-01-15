/**
	\file "sim/chpsim/Event.h"
	Various classes of chpsim events.  
	$Id: Event.h,v 1.1.2.20 2007/01/15 04:04:20 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENT_H__
#define	__HAC_SIM_CHPSIM_EVENT_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include "util/string_fwd.h"
#include <iosfwd>
#include <valarray>
#include <vector>
#include "sim/chpsim/Dependence.h"
#include "Object/ref/reference_enum.h"

/**
	Define to 1 to embed a guard expresion in EventNode.
	Goal: 0
	Rationale: don't make a special case for condition_wait events, 
	treat them like any other blockable event.  
 */
#define	CHPSIM_EVENT_GUARDED		0

#if CHPSIM_EVENT_GUARDED
#include "util/memory/count_ptr.h"
#endif

namespace HAC {
namespace entity {
#if CHPSIM_EVENT_GUARDED
	class bool_expr;
	class state_manager;
	class footprint;
#endif
namespace CHP {
	class action;
}
}
namespace SIM {
namespace CHPSIM {
class DependenceSetCollector;
class nonmeta_context;
class graph_options;
using std::ostream;
using std::string;
using std::vector;
using std::valarray;
#if CHPSIM_EVENT_GUARDED
using util::memory::count_ptr;
using entity::bool_expr;
using entity::state_manager;
using entity::footprint;
#endif
using entity::CHP::action;
using entity::global_indexed_reference;


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
	typedef	valarray<event_index_type>	successor_list_type;
private:
#if CHPSIM_EVENT_GUARDED
	/// wake-up guard expression for THIS event, if applicable
	count_ptr<const bool_expr>	guard_expr;
#endif
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
	successor_list_type		successor_events;
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

	const size_t
	get_process_index(void) const { return process_index; }

#if CHPSIM_EVENT_GUARDED
	void
	set_guard_expr(const count_ptr<const bool_expr>&);
#endif

	void
	set_event_type(const unsigned short t) { event_type = t; }

	void
	set_predecessors(const event_index_type n) { predecessors = n; }

	void
	import_dependencies(const DependenceSetCollector& d) {
		deps.import(d);
	}

	const DependenceSet&
	get_deps(void) const { return deps; }

	void
	reset(void);

	void
	execute(const nonmeta_context&, vector<global_indexed_reference>&);

	bool
	recheck(const nonmeta_context&);


	ostream&
	dump_brief(ostream&) const;

	ostream&
	dump_pending(ostream&) const;

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_dot_node(ostream&, const event_index_type, 
		const graph_options&) const;

	ostream&
	dump_dot_edge(ostream&) const;

};	// end class EventNode

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_EVENT_H__

