/**
	\file "sim/prsim/ExprAlloc.h"
	$Id: ExprAlloc.h,v 1.9.2.1 2008/07/09 04:34:45 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOC_H__
#define	__HAC_SIM_PRSIM_EXPRALLOC_H__

#include <queue>
#include "Object/lang/cflat_context_visitor.h"
#include "sim/prsim/ExprAllocFlags.h"
#include "sim/prsim/State-prsim.h"		// for nested typedefs
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
class unique_process_subgraph;
#if PRSIM_INDIRECT_EXPRESSION_MAP
class unique_process_subgraph;
#endif
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
	typedef	state_type::node_type			node_type;
	typedef	state_type::node_pool_type		node_pool_type;
	typedef	state_type::expr_struct_type		expr_struct_type;
	typedef	state_type::expr_state_type		expr_state_type;
#if PRSIM_INDIRECT_EXPRESSION_MAP
	typedef	unique_process_subgraph			unique_type;
#else
	typedef	State					unique_type;
#endif
	typedef	unique_type::expr_pool_type		expr_pool_type;
	typedef	unique_type::graph_node_type		graph_node_type;
	typedef	unique_type::expr_graph_node_pool_type	graph_node_pool_type;
	typedef	unique_type::rule_map_type		rule_map_type;
	typedef	unique_type::rule_type			rule_type;
protected:
	typedef	std::queue<expr_index_type>	free_list_type;
public:
	state_type&				state;
	node_pool_type&				st_node_pool;
#if 0
	expr_pool_type&				st_expr_pool;
	graph_node_pool_type&			st_graph_node_pool;
	rule_map_type&				st_rule_map;
#else
	unique_process_subgraph*		g;
#endif
protected:
	/// the expression index last returned
	expr_index_type				ret_ex_index;
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
	explicit
	ExprAlloc(state_type&);

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
	// these public functions are really only intended for
	// macro/directive/attribute visitor classes...

	void
	link_node_to_root_expr(const node_index_type, 
		const expr_index_type, const bool dir
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

