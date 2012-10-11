/**
	\file "Object/lang/CHP_event.h"
	Various classes of chpsim events.  
	$Id: CHP_event.h,v 1.4 2008/11/23 17:53:51 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_EVENT_H__
#define	__HAC_OBJECT_LANG_CHP_EVENT_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include "util/string_fwd.h"
#include "util/utypes.h"
#include <iosfwd>
#include <vector>
#include "util/persistent_fwd.h"
#include "util/macros.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
struct graph_options;
}	// end namespace CHPSIM
}	// end namespace SIM
namespace entity {
struct expr_dump_context;
class nonmeta_state_manager;
namespace CHP {
class action;

using std::ostream;
using std::istream;
using std::string;
using std::vector;
using util::persistent_object_manager;
using entity::expr_dump_context;

//=============================================================================
/**
	Event type numeration codes.
	Can also introduce 'special' types of events like I/O.  
 */
enum {
	EVENT_NULL = 0,		///< can be used to mean 'skip' or 'no-op'
	EVENT_ASSIGN = 1,
	EVENT_SEND = 2,
	EVENT_RECEIVE = 3,
	EVENT_PEEK = 4,		///< read values from channel w/o acknowledge
	EVENT_CONCURRENT_FORK = 5,	///< divergence of concurrent events
	EVENT_CONCURRENT_JOIN = EVENT_NULL,	///< convergence event (no-op)
	/**
		the start of any selection: 
		deterministic, non-deterministic, and do-while loops
	 */
	EVENT_SELECTION_BEGIN = 6,
	EVENT_SELECTION_END = EVENT_NULL,	///< end of any selection (no-op)
	EVENT_CONDITION_WAIT = 7,	///< predicate wait
	EVENT_FUNCTION_CALL = 8		///< external function call
};

//=============================================================================
/**
	This contains information about the locally allocated CHP event.
	No dependency information here.
	Contains no stateful information.  

	This represents a node in the event graph, which can be 
	thought of as a marked graph.  
	We 'allocate' graph nodes with back-references to their
	corresponding events in the CHP footprint.  
	The 'edges' between graph nodes form predecessor-successor relations.
	This will be used as a base class for other events.  
 */
class local_event {
	typedef	local_event		this_type;
public:
	/**
		Hard-coded time type for now.
	 */
//	typedef	SIM::real_time		time_type;
	typedef	size_t			event_index_type;
	/**
		Can afford to use vector here, 
		this struct is not replicated per instance.
	 */
	typedef	vector<event_index_type>	successor_list_type;
//	static const time_type		default_delay;
	static const char		node_prefix[];
private:
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
		*Local* events that follow this event.  
		The interpretation of these events (concurrent, 
		deterministic, nondeterministic) is depending on the type.  
	 */
	successor_list_type		successor_events;
private:
	/**
		enumeration for this event, 
		semi-redundant with the action pointer.  
	 */
	ushort				event_type;
#if 0
	/**
		General purpose flags (space-filler).  
	 */
	unsigned short			flags;
#endif
	/**
		The number of concurrent event predecessors.
		Wait for this number of events to precede before executing, 
		like a barrier count.  
		Sequential actions (in chain) only have one predecessor.  
		Selection statements re-join with only one predecessor.  
		Do we keep predecessor edge information anywhere else?
		Should be const, incidentally...
	 */
	ushort				predecessors;
public:
	local_event();

	local_event(const action*, const unsigned short);

	~local_event();

#if 0
	void
	orphan(void);
#endif

	/**
		Entry events must have one predecessor.
	 */
	void
	mark_as_entry(void) {
		if (!predecessors)
			predecessors = 1;
	}

	bool
	is_null(void) const {
		return (event_type == EVENT_NULL);
	}

	/**
		\return true if this event type is considered trivial, 
			such as concurrent forks and joins, end-select, 
			but NOT condition waits.  
			This determines whether or not meta-events may
			be combined in event graph optimization.  
	 */
	bool
	is_trivial(void) const {
		return (event_type == EVENT_NULL) ||
			(event_type == EVENT_CONCURRENT_FORK);
	}

	/**
		Condition-waits are classified as having "trivial-delay", 
		even though they take up an action event node.
		They shouldn't incur additional delay unless set otherwise.
	 */
	bool
	has_trivial_delay(void) const {
		return is_trivial() || (event_type == EVENT_CONDITION_WAIT);
	}

	/**
		NOTE: need not be trivial, just copiable and 
		successor-substitutable.  
		NOTE: No selection has only one successor.  
	 */
	bool
	is_movable(void) const {
		return (successor_events.size() == 1);
	}

	/**
		this 'leaks' out the pointer, it is not meant to be misused, 
		only short-lived temporary references.  
	 */
	const action*
	get_chp_action(void) const { return action_ptr; }

#if 0
	void
	set_event_type(const unsigned short t) { event_type = t; }
#endif

	unsigned short
	get_event_type(void) const { return event_type; }

	void
	set_predecessors(const event_index_type n) { predecessors = ushort(n); }

	unsigned short
	get_predecessors(void) const { return predecessors; }

	ostream&
	dump_brief(ostream&, const expr_dump_context&) const;

	ostream&
	dump_source(ostream&, const expr_dump_context&) const;

	ostream&
	dump_pending(ostream&) const;

	ostream&
	dump_type(ostream&) const;

	ostream&
	dump_struct(ostream&, const expr_dump_context&, 
		const size_t = 0, 
		const event_index_type = 0) const;

// need graph_options
	ostream&
	dump_dot_node(ostream&, const event_index_type, 
		const SIM::CHPSIM::graph_options&,
		const expr_dump_context&, 
		const char* = NULL, 	// extra label text
		const event_index_type = 0	// offset to add to successors
		) const;

	ostream&
	dump_successor_edges_default(ostream&, const event_index_type ,
		const event_index_type = 0	// offset to add to successors
		) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class local_event

//=============================================================================
}	// end namespace HAC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_EVENT_H__

