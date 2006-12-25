/**
	\file "sim/chpsim/StateConstructor.h"
	The visitor that initializes and allocates CHPSIM state.  
	$Id: StateConstructor.h,v 1.1.2.5 2006/12/25 03:28:03 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__
#define	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__

#include <vector>
#include "Object/lang/cflat_context_visitor.h"
#include "sim/chpsim/StateConstructorFlags.h"
#include "sim/chpsim/State.h"
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
class EventNode;		// from "sim/chpsim/Event.h"
using entity::state_manager;
using entity::cflat_context_visitor;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_macro;
using entity::PRS::footprint_expr_node;
using entity::SPEC::footprint_directive;

//=============================================================================
/**
	Visitor that initializes and allocates CHPSIM state.  
	TODO: re-factor code to not refer to non-CHP visitees.  
	This clearly lacks good organization.  :S
 */
class StateConstructor : public cflat_context_visitor {
public:
	typedef	State				state_type;
	// typedef	std::default_vector<size_t>::type	return_indices_type;
	typedef	EventNode			event_type;
public:
	state_type&				state;
	/**
		Return value slot to indicate last allocated event(s).  
		Should be non-zero.  
		There may be more than one in the case of concurrency.  
		These are needed to re-link predecessors to successors.  
	 */
	// return_indices_type			last_event_indices;
	event_index_type			last_event_index;
	/**
		index of the globally allocated process, for context.  
	 */
	node_index_type				current_process_index;
	/**
		List of initially ready events.  
	 */
	std::vector<event_index_type>		initial_events;
private:
	// non-copy-able
	explicit
	StateConstructor(const StateConstructor&);
public:
	explicit
	StateConstructor(state_type&);

	~StateConstructor();

	const state_manager&
	get_state_manager(void) const;

	const entity::footprint&
	get_process_footprint(void) const;

	void
	connect_successor_events(event_type&) const;

	void
	count_predecessors(const event_type&) const;

protected:
	using cflat_context_visitor::visit;

	void
	reset(void);

	// overrides
	void
	visit(const state_manager&);

	// overrides
	void
	visit(const entity::PRS::footprint&);	// no-op

	void
	visit(const footprint_rule&);	// no-op

	void
	visit(const footprint_expr_node&);	// no-op

	void
	visit(const footprint_macro&);	// no-op

	void
	visit(const entity::SPEC::footprint_directive&);	// no-op


};	// end class StateConstructor

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__

