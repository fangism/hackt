/**
	\file "sim/prsim/ExprAlloc.h"
	$Id: ExprAlloc.h,v 1.14 2009/02/07 03:32:57 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOC_H__
#define	__HAC_SIM_PRSIM_EXPRALLOC_H__

#include <queue>
#include <map>
#include "Object/lang/cflat_context_visitor.h"
#include "sim/prsim/ExprAllocFlags.h"
#include "sim/prsim/State-prsim.h"		// for nested typedefs
#include "sim/common.h"

/**
	Define to 1 to try to use simple allocation traversal
	without peeling out top-level process separately.  
 */
#define	PRSIM_SIMPLE_ALLOC			0

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
class unique_process_subgraph;
using std::map;
using entity::global_entry;
using entity::process_tag;
using entity::state_manager;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_expr_node;
using entity::PRS::footprint_macro;
using entity::SPEC::footprint_directive;
using entity::cflat_context_visitor;

//=============================================================================
/**
	Visits all PRS expressions and allocates them for use with 
	the prsim simulator.  
 */
class ExprAlloc : public cflat_context_visitor {
public:
	typedef	State					state_type;
	typedef	cflat_context_visitor			parent_type;
	typedef	state_type::expr_struct_type		expr_struct_type;
	typedef	state_type::expr_state_type		expr_state_type;
	typedef	expr_struct_type			expr_type;
	typedef	unique_process_subgraph			unique_type;
#if PRSIM_SIMPLE_ALLOC
	typedef	map<const entity::footprint*, process_index_type>
#else
	typedef	map<const entity::PRS::footprint*, process_index_type>
#endif
						process_footprint_map_type;
	typedef	unique_type::expr_pool_type		expr_pool_type;
	typedef	unique_type::graph_node_type		graph_node_type;
	typedef	unique_type::expr_graph_node_pool_type	graph_node_pool_type;
	typedef	unique_type::rule_map_type		rule_map_type;
	typedef	unique_type::rule_type			rule_type;
protected:
	typedef	std::queue<expr_index_type>	free_list_type;
public:
	state_type&				state;
	/**
		Structure and state is allocated one process at a time.
	 */
	unique_process_subgraph*		g;
protected:
	/**
		Currently running unique process index.
	 */
	size_t					current_process_index;
	/**
		Running total of the number of global expressions.
	 */
	size_t					total_exprs;
	/**
		Translates unique prs_footprint to unique process index.  
	 */
	process_footprint_map_type		process_footprint_map;
	/// the expression index last returned
	expr_index_type				ret_ex_index;
public:
	/**
		auxiliary value which is cleared by the 'iskeeper'
		rule attribute, and filters out keeper rules entirely
		from prsim.  
		When true, skip rule.  
		Anytime this value is read as true, then immediately
		reset it back to false, so the next rule is not
		affected.  
	 */
	bool					suppress_keeper_rule;
protected:
	/**
		Auxiliary temporary placeholder rule, 
		for applying attributes.  
	 */
	rule_type*				temp_rule;
	/**
		Set of optimization flags.  
	 */
	ExprAllocFlags				flags;
	/**
		List of expression indices to recycle before allocating
		new ones.  FIFO.  Only used when optimizations are enabled.  
	 */
	free_list_type				expr_free_list;
public:

	ExprAlloc(state_type&, const ExprAllocFlags&);

	// default empty destructor

	expr_index_type
	last_expr_index(void) const { return ret_ex_index; }

	rule_type&
	get_temp_rule(void) const { NEVER_NULL(temp_rule); return *temp_rule; }

	State&
	get_state(void) { return state; }

	const State&
	get_state(void) const { return state; }

protected:
	using cflat_visitor::visit;

	void
	visit(const state_manager&);

#if PRSIM_SIMPLE_ALLOC
	void
	visit(const global_entry<process_tag>&);
#endif

	void
	visit(const entity::PRS::footprint&);

	void
	visit(const footprint_rule&);

	void
	visit(const footprint_expr_node&);

	void
	visit(const footprint_macro&);

	// what do we do?
	void
	visit(const footprint_directive&);

public:
// wrapper implementations:
// select depending on whether target is local scope or global scope

	node_index_type
	lookup_local_bool_id(const node_index_type ni) const {
	// DELIBERATE OVERRIDE: DO NOT TRANSLATE TO GLOBAL NODE INDICES!
	// if node index argument comes from PRS_footprint, then it was
	// 1-indexed, and needs to be converted to 0-indexed.
	if (current_process_index) {
		INVARIANT(ni);
		return ni -1;
	} else {	// top-level, keep 1-indexed, 0 is reserved
		return ni;
	}
	}

	// for now, exclusive rings (both force and check) use this
	// eventually, they may be pushed into local subgraphs...
	node_index_type
	lookup_global_bool_id(const node_index_type ni) const {
		// works without catching pid=0 (top-level)
		// b/c we populated its frame map with identity indices
#if PRSIM_SIMPLE_ALLOC
	if (current_process_index) {
		return __lookup_global_bool_id(ni);
	} else {
		return __lookup_global_bool_id(ni) +1;
	}
#else
		return __lookup_global_bool_id(ni);
#endif
	}

	// these public functions are really only intended for
	// macro/directive/attribute visitor classes...
protected:
#if PRSIM_INVARIANT_RULES
	void
	link_invariant_expr(const expr_index_type);
#endif

public:
	void
	link_node_to_root_expr(const node_index_type, 
		const expr_index_type, const bool dir, const rule_type&
#if PRSIM_WEAK_RULES
		, const rule_strength
#endif
		);

	expr_index_type
	allocate_new_literal_expr(const node_index_type);

	expr_index_type
	allocate_new_not_expr(const expr_index_type);

	expr_index_type
	allocate_new_Nary_expr(const char, const size_t);

	void
	link_child_expr(const expr_index_type p, const expr_index_type c, 
		const size_t o);

private:
	void
	fold_literal(const expr_index_type);

	void
	denormalize_negation(const expr_index_type);

	void
	compact_expr_pools(void);

private:
	/// private, undefined copy-ctor.
	explicit
	ExprAlloc(const ExprAlloc&);

};	// end class ExprAlloc

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPRALLOC_H__

