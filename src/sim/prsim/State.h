/**
	\file "sim/prsim/State.h"
	The state of the prsim simulator.  
	$Id: State.h,v 1.2.26.2 2006/03/24 00:01:51 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_STATE_H__
#define	__HAC_SIM_PRSIM_STATE_H__

#include <iosfwd>
#include "sim/prsim/Event.h"
#include "sim/prsim/Node.h"
#include "sim/prsim/Expr.h"
#include "Object/lang/PRS_enum.h"	// for expression parenthesization
#include "util/string_fwd.h"
#include "util/list_vector.h"
// #include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
	class module;
}

namespace SIM {
namespace PRSIM {
class ExprAlloc;
using std::string;
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
	// these typedefs will make it convenient to template this
	// class in the future...
	/// can switch between integer and real-valued time
	typedef	real_time			time_type;
	typedef	NodeState			node_type;
	typedef	Expr				expr_type;
	typedef	ExprGraphNode			graph_node_type;
	typedef	Event				event_type;
	typedef	EventPool			event_pool_type;
	typedef	EventPlaceholder<time_type>	event_placeholder_type;
	typedef	EventQueue<event_placeholder_type>	event_queue_type;

	typedef	vector<node_type>		node_pool_type;
	typedef	vector<Expr>			expr_pool_type;
private:
	/**
		A fast, realloc-free vector-like structure
		to built-up expressions.  
		Will have log(N) time access due to internal tree structure.
	 */
	typedef	list_vector<expr_type>		temp_expr_pool_type;
	/**
		The structure for top-down expression topology.  
		Will have log(N) time access due to internal tree structure.
	 */
	typedef	list_vector<graph_node_type>	expr_graph_node_pool_type;

	enum {
		/// index of the first valid node
		FIRST_VALID_NODE = SIM::INVALID_NODE_INDEX +1,
		/// index of the first valid expr/expr_graph_node
		FIRST_VALID_EXPR = SIM::INVALID_EXPR_INDEX +1,
		/// index of the first valid event
		FIRST_VALID_EVENT = SIM::INVALID_EVENT_INDEX +1
	};
	/**
		Return codes for set_node_time.  
	 */
	enum {
		ENQUEUE_ACCEPT = 0,
		ENQUEUE_WARNING = 1,
		ENQUEUE_REJECT = 2,
		ENQUEUE_FATAL = 3
	};
private:
//	count_ptr<const module>			mod;
	const module&				mod;
	node_pool_type				node_pool;
	expr_pool_type				expr_pool;
	expr_graph_node_pool_type		expr_graph_node_pool;
	event_pool_type				event_pool;
	event_queue_type			event_queue;
	// current time, etc...
	time_type				current_time;
	// watched nodes
	// vectors
	// channels
	// mode of operation
public:
#if 0
	explicit
	State(const count_ptr<const module>&);
#else
	explicit
	State(const module&);
#endif
private:
	// inaccessible undefined copy-constructor ... for now
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

	const node_type&
	get_node(const node_index_type) const;

	node_type&
	get_node(const node_index_type);

	string
	get_node_canonical_name(const node_index_type) const;

	int
	set_node_time(const node_index_type, const char val, 
		const time_type t);

	int
	set_node(const node_index_type n, const char val) {
		return set_node_time(n, val, this->current_time);
	}

private:
	event_index_type
	allocate_event(void);

	void
	deallocate_event(const event_index_type);

	event_type&
	get_event(const event_index_type);

	void
	enqueue_event(const event_placeholder_type&);

public:
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

	ostream&
	dump_event_queue(ostream&) const;

	ostream&
	dump_node_fanout(ostream&, const node_index_type) const;

	ostream&
	dump_node_fanin(ostream&, const node_index_type) const;

	ostream&
	dump_subexpr(ostream&, const expr_index_type, 
		const char p = entity::PRS::PRS_LITERAL_TYPE_ENUM) const;

private:
	void
	head_sentinel(void);

};	// end class State

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_STATE_H__

