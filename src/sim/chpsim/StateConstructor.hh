/**
	\file "sim/chpsim/StateConstructor.hh"
	The visitor that initializes and allocates CHPSIM state.  
	$Id: StateConstructor.hh,v 1.7 2010/04/07 00:13:08 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__
#define	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__

#include "Object/lang/CHP_visitor.hh"
#include "sim/chpsim/StateConstructorFlags.hh"
#include "sim/common.hh"

namespace HAC {
namespace SIM {
namespace CHPSIM {
class EventNode;		// from "sim/chpsim/Event.hh"
class State;
using entity::state_manager;
#if 0
using entity::cflat_context_visitor;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_macro;
using entity::PRS::footprint_expr_node;
#endif
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
	const state_type&			state;
private:
	event_type&				event;
public:
	/**
		index of the globally allocated process, for context.  
	 */
	node_index_type				current_process_index;
private:
	// non-copy-able
	explicit
	StateConstructor(const StateConstructor&);
public:
	explicit
	StateConstructor(const state_type&, event_type&);

	~StateConstructor();

	const entity::footprint&
	get_process_footprint(void) const;

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

};	// end class StateConstructor

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__

