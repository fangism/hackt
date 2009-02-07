/**
	\file "sim/prsim/process_state.h"
	The state of the prsim simulator.  
	$Id: process_state.h,v 1.1 2009/02/07 03:00:41 fang Exp $

	This file was renamed from:
	Id: State.h,v 1.17 2007/01/21 06:01:02 fang Exp
	to avoid object name collision in shared libraries.  
 */

#ifndef	__HAC_SIM_PRSIM_PROCESS_STATE_H__
#define	__HAC_SIM_PRSIM_PROCESS_STATE_H__

#include "sim/prsim/process_graph.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;

//=============================================================================
/**
	This base structure contains simulation-invariant structural data only.
 */
class process_sim_state_base {
protected:
	/**
		While state is being allocated, the index field is valid, 
		after the array is finished, the ptr replaces the index.
	 */
	union {
		process_index_type		index;
		const unique_process_subgraph*	ptr;
	} type_ref;
	/**
		global offset of first expression belonging to this process
		must be non-zero.
		In the global array of processes, these offset values
		must increase monotonically (sorted).
	 */
	expr_index_type				global_expr_offset;

public:
	struct offset_comparator;

	/// only call this after ptr has been set by finish_process...
	const unique_process_subgraph&
	type(void) const {
		NEVER_NULL(type_ref.ptr);
		return *type_ref.ptr;
	}

	const process_index_type&
	get_index(void) const { return type_ref.index; }

	const expr_index_type&
	get_offset(void) const { return global_expr_offset; }

	void
	set_ptr(const unique_process_subgraph& g) {
		type_ref.ptr = &g;
	}

	/**
		\param gei is a global expression index.
		\return local expression index.
	 */
	expr_index_type
	local_expr_index(const expr_index_type gei) const {
		return gei -global_expr_offset;
	}

	/**
		\param lei is a local expression index.
		\return global expression index.
	 */
	expr_index_type
	global_expr_index(const expr_index_type lei) const {
		return lei +global_expr_offset;
	}

	const unique_process_subgraph::rule_type*
	lookup_rule(const expr_index_type gei) const {
		return type().lookup_rule(local_expr_index(gei));
	}

};	// end struct process_sim_state_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	state information per process instance.
	This is memory-intensive, and thus should be kept small.  
	Node state information is kept outside of these structures.  
 */
struct process_sim_state : public process_sim_state_base {
	typedef	ExprState			expr_state_type;
	typedef	RuleState<rule_time_type>	rule_state_type;
	typedef	unique_process_subgraph::rule_type
						rule_type;
	typedef	unique_process_subgraph::node_set_type
						node_set_type;
	typedef	unique_process_subgraph::expr_struct_type
						expr_struct_type;
	typedef	unique_process_subgraph::graph_node_type
						graph_node_type;

	struct memory_accumulator;
//	struct expr_offset_comparator;

	/// array of expression states
	valarray<expr_state_type>		expr_states;
	/// array of rule states
	valarray<rule_state_type>		rule_states;


	void
	allocate_from_type(const unique_process_subgraph&, 
		const process_index_type, const expr_index_type);

	void
	clear(void);

	void
	initialize(void);

	void
	__get_local_X_fanins(const expr_index_type, 
		const State&, node_set_type&) const;

	void
	__local_expr_why_not(ostream&, const expr_index_type, 
		const State&, const size_t, 
		const bool, const bool, 
		node_set_type&, node_set_type&) const;

	void
	__local_expr_why_X(ostream&, const expr_index_type, 
		const State&, const size_t, 
		const bool, node_set_type&, node_set_type&) const;

	void
	__recurse_expr_why_not(ostream&, const expr_index_type, 
		const pull_enum, 
		const State&, const size_t, 
		const bool, const bool, 
		node_set_type&, node_set_type&) const;

	void
	__recurse_expr_why_X(ostream&, const expr_index_type, 
		const State&, const size_t, 
		const bool, node_set_type&, node_set_type&) const;

#if PRSIM_INVARIANT_RULES
	struct invariant_checker;
	struct invariant_dumper;

	bool
	check_invariants(ostream&, const State&) const;

	ostream&
	dump_invariants(ostream&, const State&, const bool) const;
#endif

	ostream&
	dump_subexpr(ostream&, const expr_index_type, 
		const State&, 
		const bool v, const uchar p = expr_struct_type::EXPR_ROOT,
		const bool cp = false) const;

	struct dumper_base;
	struct rules_dumper;

	ostream&
	dump_rules(ostream&, const State&, const bool) const;

	ostream&
	dump_rule(ostream&, const rule_index_type, 
		const State&, const bool, const bool) const;

	ostream&
	dump_node_fanin(ostream&, const node_index_type, 
		const State&, const bool) const;

};	// end struct process_sim_state

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_PROCESS_STATE_H__

