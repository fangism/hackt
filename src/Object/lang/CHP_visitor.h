/**
	\file "Object/lang/CHP_visitor.h"
	The visitor that initializes and allocates CHPSIM state.  
	$Id: CHP_visitor.h,v 1.7 2010/04/02 22:18:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_VISITOR_H__
#define	__HAC_OBJECT_LANG_CHP_VISITOR_H__

#include "Object/lang/cflat_context_visitor.h"

namespace HAC {
namespace entity {
namespace CHP {
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
using entity::state_manager;
using entity::cflat_context_visitor;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_macro;
using entity::PRS::footprint_expr_node;
using entity::SPEC::footprint_directive;
#endif

// forward declarations
class action;
class action_sequence;
class concurrent_actions;
class guarded_action;
class deterministic_selection;
class nondeterministic_selection;
class metaloop_selection;
class metaloop_statement;
class assignment;
class condition_wait;
class channel_send;
class channel_receive;
class do_while_loop;
class do_forever_loop;
class function_call_stmt;

//=============================================================================
/**
	Visitor that initializes and allocates CHPSIM state.  
	TODO: re-factor code to not refer to non-CHP visitees.  
	This clearly lacks good organization.  :S
 */
class chp_visitor
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	: public cflat_context_visitor
#endif
	{
public:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	chp_visitor() : cflat_context_visitor() { }
#endif

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	explicit
	chp_visitor(
		const state_manager& _sm,
		const entity::footprint& _topfp
		) :
		cflat_context_visitor(_sm, _topfp) { }
#endif

virtual	~chp_visitor() { }

#if 0
// no catch-all necessary
virtual	void
	visit(const action&) = 0;
#endif

virtual	void
	visit(const action_sequence&) = 0;

virtual	void
	visit(const concurrent_actions&) = 0;

virtual	void
	visit(const guarded_action&) = 0;

virtual	void
	visit(const deterministic_selection&) = 0;

virtual	void
	visit(const nondeterministic_selection&) = 0;

virtual	void
	visit(const metaloop_selection&) = 0;

virtual	void
	visit(const metaloop_statement&) = 0;

virtual	void
	visit(const assignment&) = 0;

virtual	void
	visit(const condition_wait&) = 0;

virtual	void
	visit(const channel_send&) = 0;

virtual	void
	visit(const channel_receive&) = 0;

virtual	void
	visit(const do_while_loop&) = 0;

virtual	void
	visit(const do_forever_loop&) = 0;

virtual	void
	visit(const function_call_stmt&) = 0;

protected:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	using cflat_context_visitor::visit;
#endif

	// overrides
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
virtual	void
	visit(const state_manager&);

virtual	void
	visit(const GLOBAL_ENTRY<process_tag>&);

	// overrides
virtual	void
	visit(const entity::PRS::footprint&);	// no-op

virtual	void
	visit(const footprint_rule&);	// no-op

virtual	void
	visit(const footprint_expr_node&);	// no-op

virtual	void
	visit(const footprint_macro&);	// no-op

virtual	void
	visit(const entity::SPEC::footprint_directive&);	// no-op
#endif

};	// end class StateConstructor

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_VISITOR_H__

