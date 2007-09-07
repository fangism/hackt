/**
	\file "sim/chpsim/StateConstructor.h"
	The visitor that initializes and allocates CHPSIM state.  
	$Id: StateConstructor.h,v 1.4.8.3 2007/09/07 01:33:23 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__
#define	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__

#include "sim/chpsim/devel_switches.h"
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
// include "sim/chpsim/DependenceSetCollector.h"?
#else
#include <vector>
#include <set>		// or use util/memory/free_list interface
#endif
#include "Object/lang/CHP_visitor.h"
#include "sim/chpsim/StateConstructorFlags.h"
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
class EventNode;		// from "sim/chpsim/Event.h"
class State;
using entity::state_manager;
using entity::cflat_context_visitor;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_macro;
using entity::PRS::footprint_expr_node;
using entity::SPEC::footprint_directive;
using entity::CHP::chp_visitor;
using entity::CHP::action;
using entity::CHP::action_sequence;
using entity::CHP::concurrent_actions;
using entity::CHP::guarded_action;
using entity::CHP::deterministic_selection;
using entity::CHP::nondeterministic_selection;
using entity::CHP::metaloop_selection;
using entity::CHP::metaloop_statement;
using entity::CHP::assignment;
using entity::CHP::condition_wait;
using entity::CHP::channel_send;
using entity::CHP::channel_receive;
using entity::CHP::do_while_loop;
using entity::CHP::do_forever_loop;
using entity::CHP::function_call_stmt;

//=============================================================================
/**
	Visitor that initializes and allocates CHPSIM state.  
	TODO: re-factor code to not refer to non-CHP visitees.  
	This clearly lacks good organization.  :S
 */
class StateConstructor : public chp_visitor {
public:
	typedef	State				state_type;
	// typedef	std::default_vector<size_t>::type	return_indices_type;
	typedef	EventNode			event_type;
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
//	typedef	state_type::event_pool_type	event_pool_type;
	typedef	std::vector<event_type>		event_pool_type;
private:
	typedef	std::set<size_t>		free_list_type;
#endif
private:
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	const
#endif
	state_type&				state;
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	event_type&				event;
#endif
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	free_list_type				free_list;
#endif
public:
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	/**
		Return value slot to indicate last allocated event(s).  
		Should be non-zero.  
		There may be more than one in the case of concurrency.  
		These are needed to re-link predecessors to successors.  
	 */
	// return_indices_type			last_event_indices;
	event_index_type			last_event_index;
#endif
	/**
		index of the globally allocated process, for context.  
	 */
	node_index_type				current_process_index;
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	/**
		List of initially ready events.  
	 */
	std::vector<event_index_type>		initial_events;
#endif
private:
	// non-copy-able
	explicit
	StateConstructor(const StateConstructor&);
public:
	explicit
	StateConstructor(
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		const
#endif
		state_type&
#if CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
		, event_type&
#endif
		);

	~StateConstructor();

	const state_manager&
	get_state_manager(void) const;

	const entity::footprint&
	get_process_footprint(void) const;

#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
protected:
	event_type&
	get_event(const event_index_type ei);

	const event_type&
	get_event(const event_index_type ei) const;

	event_index_type
	allocate_event(const event_type&);

	// only needed when trying to be clever and recycle useless events
	void
	deallocate_event(const event_index_type);

	size_t
	event_pool_size(void) const;

	void
	connect_successor_events(event_type&) const;

public:
	void
	count_predecessors(const event_type&) const;

protected:
	event_index_type
	forward_successor(const event_index_type);

	void
	forward_successor(const event_index_type, const event_index_type, 
		const event_index_type);
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS

public:
	void
	visit(const action_sequence&);

	void
	visit(const concurrent_actions&);

	void
	visit(const guarded_action&);

	void
	visit(const deterministic_selection&);

	void
	visit(const nondeterministic_selection&);

	void
	visit(const metaloop_selection&);

	void
	visit(const metaloop_statement&);

	void
	visit(const assignment&);

	void
	visit(const condition_wait&);

	void
	visit(const channel_send&);

	void
	visit(const channel_receive&);

	void
	visit(const do_while_loop&);

	void
	visit(const do_forever_loop&);

	void
	visit(const function_call_stmt&);

protected:
	using chp_visitor::visit;

#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
	void
	reset(void);

	// overrides
	void
	visit(const state_manager&);
#endif

};	// end class StateConstructor

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__

