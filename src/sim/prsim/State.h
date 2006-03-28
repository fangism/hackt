/**
	\file "sim/prsim/State.h"
	The state of the prsim simulator.  
	$Id: State.h,v 1.2.26.6 2006/03/28 03:48:05 fang Exp $
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
		(modeled after old prsim's struct Prs)
	This structure shall contain no pointers!
	This state should be trivially saveable and restorable.  
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
	typedef	ExprState			expr_type;
	typedef	ExprGraphNode			graph_node_type;
	typedef	Event				event_type;
	typedef	EventPool			event_pool_type;
	typedef	EventPlaceholder<time_type>	event_placeholder_type;
	typedef	EventQueue<event_placeholder_type>	event_queue_type;

	typedef	vector<node_type>		node_pool_type;
	typedef	vector<expr_type>		expr_pool_type;
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

	/**
		Simulation flags, bit fields.  
	 */
	enum {
		FLAGS_DEFAULT = 0x0,
		FLAG_NO_WEAK_INTERFERENCE = 0x1,
		FLAG_STOP_SIMULATION = 0x2,
		FLAG_ESTIMATE_ENERGY = 0x4,
		FLAG_RANDOM_TIMING = 0x8
	};
	typedef	unsigned int			flags_type;

	/**
		Instead of using circular linked lists with pointers, 
		we use a map of (cyclic referenced) indices to represent
		the exclusive rings. 
		Requires half as much memory as equivalent ring of pointers.  
		Another possiblity: fold these next-indices into the 
			Node structure.  
		Alternative: use map for sparser exclusive rings.  
	 */
#if 0
	typedef	map<node_index_type, node_index_type>
#else
	typedef	vector<node_index_type>
#endif
						excl_ring_map_type;
	typedef	vector<event_placeholder_type>	excl_queue_type;
	typedef	vector<event_index_type>	pending_queue_type;
private:
//	count_ptr<const module>			mod;
	const module&				mod;
	node_pool_type				node_pool;
	expr_pool_type				expr_pool;
	expr_graph_node_pool_type		expr_graph_node_pool;
	event_pool_type				event_pool;
	event_queue_type			event_queue;
	// exclusive rings
	excl_ring_map_type			exhi;
	excl_ring_map_type			exlo;
	// exclusive logic queues
	excl_queue_type				exclhi_queue;
	excl_queue_type				excllo_queue;
	// pending queue
	pending_queue_type			pending_queue;
	// current time, etc...
	time_type				current_time;
	// watched nodes
	// vectors
	// channels
	// mode of operation
	flags_type				flags;
public:
	enum {
		EVENT_VACUOUS = 0x1,
		EVENT_UNSTABLE = 0x2,
		EVENT_INTERFERENCE = 0x4,
		EVENT_WEAK = 0x8
	};
	static const	unsigned char		upguard[3][3];
	static const	unsigned char		dnguard[3][3];
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

	void
	update_time(const time_type t) {
		current_time = t;
	}

	void
	advance_time(const time_type t) {
		current_time += t;
	}

	const time_type&
	time(void) const { return current_time; }

	bool
	pending_events(void) const { return !event_queue.empty(); }

	int
	set_node_time(const node_index_type, const char val, 
		const time_type t);

	int
	set_node_after(const node_index_type n, const char val, 
		const time_type t) {
		return set_node_time(n, val, this->current_time +t);
	}

	int
	set_node(const node_index_type n, const char val) {
		return set_node_time(n, val, this->current_time);
	}

	node_index_type
	step(void);

	node_index_type
	cycle(void);

	bool
	stopped(void) const { return flags & FLAG_STOP_SIMULATION; }

	void
	resume(void) { flags &= ~FLAG_STOP_SIMULATION; }

private:
	event_index_type
	__allocate_event(node_type&, const node_index_type, const char);

#if 0
	event_index_type
	allocate_event(const node_index_type, const char);
#endif

	void
	__deallocate_event(node_type&, const event_index_type);

#if 0
	void
	deallocate_event(const event_index_type);
#endif

	const event_type&
	get_event(const event_index_type) const;

	event_type&
	get_event(const event_index_type);

	void
	enqueue_event(const time_type, const event_index_type);

	void
	enqueue_exclhi(const time_type, const event_index_type);

	void
	enforce_exclhi(const node_index_type);

	void
	flush_exclhi_queue(void);

	void
	enqueue_excllo(const time_type, const event_index_type);

	void
	enforce_excllo(const node_index_type);

	void
	flush_excllo_queue(void);

	void
	enqueue_pending(const event_index_type);

	void
	flush_pending_queue(void);

	event_placeholder_type
	dequeue_event(void);

	time_type
	get_delay_up(const event_type&) const;

	time_type
	get_delay_dn(const event_type&) const;

	void
	propagate_evaluation(const node_index_type, expr_index_type, 
		char prev, char next);

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

