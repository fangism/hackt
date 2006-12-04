/**
	\file "sim/chpsim/StateConstructor.h"
	The visitor that initializes and allocates CHPSIM state.  
	$Id: StateConstructor.h,v 1.1.2.1 2006/12/04 09:56:01 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__
#define	__HAC_SIM_CHPSIM_STATECONSTRUCTOR_H__

#include "Object/lang/cflat_context_visitor.h"
#include "sim/chpsim/StateConstructorFlags.h"
#include "sim/chpsim/State.h"
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
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

public:
	state_type&				state;
protected:

public:
	explicit
	StateConstructor(state_type& s) : state(s) { }

	~StateConstructor();
protected:
	using cflat_context_visitor::visit;

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

