/**
	\file "sim/prsim/State.h"
	The state of the prsim simulator.  
	$Id: State.h,v 1.1.2.2 2005/12/16 02:43:20 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_STATE_H__
#define	__HAC_SIM_PRSIM_STATE_H__

#include "sim/prsim/Event.h"
#include "sim/prsim/Node.h"
#include "sim/prsim/Expr.h"
#include "util/list_vector.h"

namespace HAC {
namespace entity {
	class module;
}

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
	typedef	vector<Expr>			expr_pool_type;
private:
	/**
		A fast, realloc-free vector-like structure
		to built-up expressions.  
	 */
	typedef	list_vector<Expr>		temp_expr_pool_type;
	/**
		The structure for top-down expression topology.  
	 */
	typedef	list_vector<ExprGraphNode>	expr_graph_node_pool_type;
private:
	node_pool_type				node_pool;
	expr_pool_type				expr_pool;
	expr_graph_node_pool_type		expr_graph_node_pool;
	EventPool				event_pool;
	EventQueue				event_queue;
	// current time, etc...
	// watched nodes
	// vectors
	// channels
	// mode of operation
public:
	State();

	State(const entity::module&);

	State(const State&);

	~State();

	/// initializes the simulator state, all nodes and exprs X
	void
	initialize(void);

	/// wipes the simulation state (like destructor)
	void
	reset(void);

	// save

	// restore

};	// end class state

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_STATE_H__

