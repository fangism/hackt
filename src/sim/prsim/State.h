/**
	\file "sim/prsim/State.h"
	The state of the prsim simulator.  
	$Id: State.h,v 1.1.2.1 2005/12/15 04:46:06 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_STATE_H__
#define	__HAC_SIM_PRSIM_STATE_H__

#include "sim/prsim/Event.h"
#include "sim/prsim/Node.h"
#include "sim/prsim/Expr.h"
#include "util/list_vector.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using util::list_vector;

//=============================================================================
/**
	The prsim simulation state.  
	This state should be saveable and restorable.  
	This will even be duplicable for scenario testing!
 */
class State {
public:
	typedef	vector<Node>			node_pool_type;
	typedef	list_vector<Expr>		expr_pool_type;
private:
	node_pool_type				node_pool;
	expr_pool_type				expr_pool;
	EventPool				event_pool;
	EventQueue				event_queue;
public:
	State();

	State(const State&);

	~State();

	// save

	// restore

};	// end class state

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_STATE_H__

