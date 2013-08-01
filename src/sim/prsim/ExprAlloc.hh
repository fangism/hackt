/**
	\file "sim/prsim/ExprAlloc.hh"
	$Id: ExprAlloc.hh,v 1.19 2010/09/29 00:13:42 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOC_H__
#define	__HAC_SIM_PRSIM_EXPRALLOC_H__

#include <queue>
#include <map>
#include "Object/global_entry_context.hh"
#include "Object/lang/cflat_visitor.hh"
#include "Object/lang/cflat_context_visitor.hh"
#include "sim/prsim/ExprAllocFlags.hh"
#include "sim/prsim/State-prsim.hh"		// for nested typedefs
#include "sim/common.hh"

#if PRSIM_PRECHARGE_INVARIANTS
#include "util/memory/excl_ptr.hh"
#endif

namespace HAC {
#if PRSIM_PRECHARGE_INVARIANTS
namespace NET {
class netlist_generator;
struct transistor_base;
}
#endif
namespace SIM {
namespace PRSIM {
class State;
struct unique_process_subgraph;
using std::map;
using entity::state_instance;
using entity::process_tag;
using entity::state_manager;
using entity::PRS::footprint_rule;
using entity::PRS::footprint_expr_node;
using entity::PRS::footprint_macro;
using entity::SPEC::footprint_directive;
using entity::bool_tag;
using entity::PRS::cflat_visitor;
using entity::footprint_frame;
using entity::global_offset;
using entity::global_entry_context;
using entity::cflat_context_visitor;
using entity::global_process_context;
#if PRSIM_PRECHARGE_INVARIANTS
class current_path_graph;
struct netgraph_node;
struct transistor_edge;
using util::memory::excl_ptr;
using NET::netlist_generator;
using NET::transistor_base;
#endif

//=============================================================================
/**
	Visits all PRS expressions and allocates them for use with 
	the prsim simulator.  
 */
class ExprAlloc :
		public cflat_visitor, 
		public cflat_context_visitor
		{
public:
	typedef	State					state_type;
	typedef	state_type::expr_struct_type		expr_struct_type;
	typedef	state_type::expr_state_type		expr_state_type;
	typedef	expr_struct_type			expr_type;
	typedef	unique_process_subgraph			unique_type;
	typedef	map<const entity::footprint*, process_index_type>
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
#if PRSIM_MODEL_POWER_SUPPLIES
	/// the current power supply associated with rule/internal node
	node_index_type				power_supply;
	/**
		Auxiliary flag for determining when power supply 
		expression is needed.
	 */
	bool					at_source;
#endif
	/**
		Set of optimization flags.  
	 */
	ExprAllocFlags				flags;
	/**
		List of expression indices to recycle before allocating
		new ones.  FIFO.  Only used when optimizations are enabled.  
	 */
	free_list_type				expr_free_list;
#if PRSIM_PRECHARGE_INVARIANTS
	excl_ptr<netlist_generator>		netlists;
#endif
	/**
		Define to true when the visitor is doing a 
		once-per-type unique_process_subgraph pass.
		Define to false when the visitor is doing a
		once-per-process-instance pass for global information
		construction.  
		This is kind of kludgy, but needed because we have a mix
		of information that is locally scoped and globally scoped.
	 */
	bool					unique_pass;
public:

	ExprAlloc(state_type&, 
		const global_process_context&,
		const ExprAllocFlags&);
	~ExprAlloc();

	// default empty destructor

	expr_index_type
	last_expr_index(void) const { return ret_ex_index; }

	rule_type&
	get_temp_rule(void) const { NEVER_NULL(temp_rule); return *temp_rule; }

	const ExprAllocFlags&
	get_flags(void) const { return flags; }

	State&
	get_state(void) { return state; }

	const State&
	get_state(void) const { return state; }

	void
	operator () (void);

	bool
	in_unique_pass(void) const {
		return unique_pass;
	}

protected:
	using cflat_visitor::visit;

	using global_entry_context::visit;

	void
	visit(const footprint&);

	void
	visit(const state_instance<process_tag>&);

	void
	visit(const state_instance<bool_tag>&);

	void
	visit(const entity::PRS::footprint&);

	void
	visit(const footprint_rule&);

	void
	visit(const footprint_expr_node&);

	void
	visit_and_expr(const footprint_expr_node&);

	void
	visit_or_expr(const footprint_expr_node&);

	void
	visit(const footprint_macro&);

	// what do we do?
	void
	visit(const footprint_directive&);

	void
	visit_rules_and_directives(const footprint&);

	size_t
	auto_create_unique_process_graph(const footprint&);

public:
// wrapper implementations:
// select depending on whether target is local scope or global scope

	/**
		\param ni 1-based local index
		\return 0-based local index
	 */
	node_index_type
	lookup_local_bool_id(const node_index_type ni) const {
		return ni -1;
	}

	// for now, exclusive rings (both force and check) use this
	// eventually, they may be pushed into local subgraphs...
	node_index_type
	lookup_global_bool_id(const node_index_type ni) const {
		return global_entry_context::lookup_global_id<bool_tag>(ni);
	}

	// these public functions are really only intended for
	// macro/directive/attribute visitor classes...
protected:
	void
	link_invariant_expr(const expr_index_type, const size_t);

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

	void
	append_child_expr(const expr_index_type p, const expr_index_type c);

	void
	prepend_child_expr(const expr_index_type p, const expr_index_type c);

private:
	expr_index_type
	allocate_new_supply_expr(void);

	expr_index_type
	and_expression_with_literal(const expr_index_type, 
		const expr_index_type);

	void
	fold_literal(const expr_index_type);

	void
	denormalize_negation(const expr_index_type);

	void
	compact_expr_pools(void);

	void
	update_expr_maps(const unique_process_subgraph&, const size_t, 
		const footprint_frame_map_type&, const size_t);

#if PRSIM_PRECHARGE_INVARIANTS
	template <bool (current_path_graph::*)(const size_t) const,
		vector<transistor_edge> netgraph_node::*,
		bool (transistor_base::*)(void) const>
	expr_index_type
	__visit_current_path_graph_generic(const current_path_graph&,
		const size_t, const size_t);

	expr_index_type
	__visit_current_path_graph_node_precharge_power(
		const current_path_graph&, const size_t, const size_t);
	expr_index_type
	__visit_current_path_graph_node_precharge_ground(
		const current_path_graph&, const size_t, const size_t);
	expr_index_type
	__visit_current_path_graph_node_logic_power(
		const current_path_graph&, const size_t, const size_t);
	expr_index_type
	__visit_current_path_graph_node_logic_ground(
		const current_path_graph&, const size_t, const size_t);
	expr_index_type
	__visit_current_path_graph_node_logic_output_up(
		const current_path_graph&, const size_t, const size_t);
	expr_index_type
	__visit_current_path_graph_node_logic_output_down(
		const current_path_graph&, const size_t, const size_t);
#endif

#if PRSIM_SETUP_HOLD
public:
	// really, only want to provide this interface to spec directives
	// node argument is 1-based local index
	void
	add_global_setup_constraint(const node_index_type) const;

	void
	add_global_hold_constraint(const node_index_type) const;
#endif

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

