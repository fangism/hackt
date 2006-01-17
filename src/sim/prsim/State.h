/**
	\file "sim/prsim/State.h"
	The state of the prsim simulator.  
	$Id: State.h,v 1.1.2.7 2006/01/17 20:55:27 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_STATE_H__
#define	__HAC_SIM_PRSIM_STATE_H__

#include <iosfwd>
#include "sim/prsim/Event.h"
#include "sim/prsim/Node.h"
#include "sim/prsim/Expr.h"
#include "util/list_vector.h"
// #include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
	class module;
}

namespace SIM {
namespace PRSIM {
class ExprAlloc;
using entity::module;
using util::list_vector;
using std::ostream;
// using util::memory::count_ptr;
//=============================================================================
/**
	The prsim simulation state.  
	This state should be saveable and restorable.  
	This will even be duplicable for scenario testing!
	TODO: make a CompactState from this State that uses plain
		vector instead of list_vectors for constant-time access.  
		For now, only the expr_graph_node_pool is log(N) access, 
		but it's not accessed during simulation, so HA!
 */
class State {
	// too lazy to write public mutator methods for the moment.  
	friend class ExprAlloc;
public:
	typedef	vector<Node>			node_pool_type;
	typedef	vector<Expr>			expr_pool_type;
private:
	/**
		A fast, realloc-free vector-like structure
		to built-up expressions.  
		Will have log(N) time access due to internal tree structure.
	 */
	typedef	list_vector<Expr>		temp_expr_pool_type;
	/**
		The structure for top-down expression topology.  
		Will have log(N) time access due to internal tree structure.
	 */
	typedef	list_vector<ExprGraphNode>	expr_graph_node_pool_type;

	enum {
		/// index of the first valid node
		FIRST_VALID_NODE = 1,
		/// index of the first valid expr/expr_graph_node
		FIRST_VALID_EXPR = 1
	};
private:
//	count_ptr<const module>			mod;
	const module&				mod;
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
#if 0
	State();
#endif

#if 0
	explicit
	State(const count_ptr<const module>&);
#else
	explicit
	State(const module&);
#endif
private:
	State(const State&);

public:
	~State();

	const module&
	get_module(void) const { return mod; }

	/// initializes the simulator state, all nodes and exprs X
	void
	initialize(void);

	/// wipes the simulation state (like destructor)
	void
	reset(void);

	void
	check_node(const node_index_type) const;

	void
	check_expr(const expr_index_type) const;

	/// run-time check of invariants in Node/Expr structures.  
	void
	check_structure(void) const;

	/// optimizes structure
	void
	optimize_structure(void) const;

	// save

	// restore

	// TODO: dump structure and state information, human-readable

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_state(ostream&) const;

	/// prints output in DOT form for visualization (options?)
	ostream&
	dump_struct_dot(ostream&) const;

private:
	void
	head_sentinel(void);

};	// end class State

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_STATE_H__

