/**
	\file "sim/prsim/ExprAlloc.cc"
	Visitor implementation for allocating simulator state structures.  
	$Id: ExprAlloc.cc,v 1.48 2011/02/03 02:23:23 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <algorithm>
#include <limits>
// #include <iterator>
#include "sim/prsim/ExprAlloc.h"
#include "sim/prsim/Expr.h"
#include "sim/prsim/State-prsim.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/PRS_attribute_common.h"
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/PRS_macro_common.h"
#include "Object/lang/PRS_macro_registry.tcc"
#include "Object/lang/SPEC_common.h"
#include "Object/lang/SPEC_registry.tcc"
#include "Object/lang/SPEC_footprint.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/state_instance.h"
#if 0
#include "Object/lang/cflat_printer.h"		// for diagnostics
#include "main/cflat_options.h"			// for diagnostics
#endif
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/instance_traits.h"
#include "Object/traits/classification_tags.h"
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#if PRSIM_PRECHARGE_INVARIANTS
#include "net/netlist_options.h"
#include "net/netlist_generator.h"
#include "sim/prsim/current-path-graph.h"
#endif
#include "util/offset_array.h"
#include "util/stacktrace.h"
#include "util/qmap.tcc"
#include "util/value_saver.h"
#include "util/IO_utils.h"
#include "util/memory/free_list.h"
#include "common/TODO.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::replace;
using entity::bool_tag;
using entity::process_tag;
using entity::pint_const;
using entity::instance_alias_info;
using entity::meta_type_tag_enum;
using util::good_bool;
using util::memory::free_list_acquire;
using util::memory::free_list_release;
using util::value_saver;
#include "util/using_ostream.h"

// shortcut for accessing the rules structure of a process graph
#define	REF_RULE_MAP(g,i)		g->rule_pool[g->rule_map[i]]

//=============================================================================
// ExprAllocFlags method definitions

/**
	Don't bother saving the optimization level, as it should not
	affect functional behavior of the circuit under test.
	This way, we can directly diff checkpoint binaries.
	Checkpoints are valid across different optimization versions.
 */
bool
ExprAllocFlags::assert_equal(const ExprAllocFlags& f) const {
	bool eq = true;
	if (flags != f.flags) {
		// fortunately, optimization only affects rules and expressions
		// which are NOT encoded in the checkpoint, and are only
		// reconstructed from node values, which are unaffected
		// by these optimizations.
		cerr << "Warning: prsim optimization flags not equal." << endl;
		cerr << "got: 0x" << std::hex << size_t(flags) << " vs.: 0x" <<
			size_t(f.flags) << endl;
	}
	if (fast_weak_keepers != f.fast_weak_keepers) {
		// fast_weak_keepers however affect rules
		// this difference *could* still be ok
		eq = false;
		cerr << "Error: prsim fast-weak-keepers option does not match."
			<< endl;
	}
	if (auto_precharge_invariants != f.auto_precharge_invariants) {
		cerr << "Warning: auto-precharge-invariants option mismatch."
			<< endl;
		cerr << "got: " << size_t(auto_precharge_invariants) <<
			" vs.: " << size_t(f.auto_precharge_invariants) << endl;
	}
	return eq;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAllocFlags::write_object(ostream& o) const {
//	util::write_value(o, flags);
	util::write_value(o, fast_weak_keepers);
	util::write_value(o, auto_precharge_invariants);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAllocFlags::load_object(istream& i) {
//	util::read_value(i, flags);
	util::read_value(i, fast_weak_keepers);
	util::read_value(i, auto_precharge_invariants);
}

//=============================================================================
// currently only supports node attributes
typedef	entity::PRS::attribute_visitor_entry<ExprAlloc>
					ExprAlloc_attribute_definition_entry;

typedef	util::qmap<string, ExprAlloc_attribute_definition_entry>
					ExprAlloc_attribute_registry_type;

static
const ExprAlloc_attribute_registry_type
ExprAlloc_attribute_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_ExprAlloc_attribute_class(void) {
	typedef	ExprAlloc_attribute_registry_type	registry_type;
	typedef	registry_type::iterator			iterator;
	typedef	registry_type::mapped_type		mapped_type;
	ExprAlloc_attribute_registry_type&
		__init_registry(const_cast<ExprAlloc_attribute_registry_type&>(
				ExprAlloc_attribute_registry));
	const string k(T::name);
	mapped_type& m(__init_registry[k]);
	if (m) {
		cerr << "Error: PRS attribute by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = ExprAlloc_attribute_definition_entry(k, &T::main, &T::check_vals);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED_CTOR__((__init_registry[k]));
	INVARIANT(n);
	return ExprAlloc_attribute_registry.size();
}

//=============================================================================
typedef	entity::PRS::macro_visitor_entry<ExprAlloc>
					ExprAlloc_macro_definition_entry;

typedef	util::qmap<string, ExprAlloc_macro_definition_entry>
					ExprAlloc_macro_registry_type;

static
const ExprAlloc_macro_registry_type
ExprAlloc_macro_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_ExprAlloc_macro_class(void) {
	typedef	ExprAlloc_macro_registry_type		registry_type;
	typedef	registry_type::iterator			iterator;
	typedef	registry_type::mapped_type		mapped_type;
	ExprAlloc_macro_registry_type&
		__init_registry(const_cast<ExprAlloc_macro_registry_type&>(
				ExprAlloc_macro_registry));
	const string k(T::name);
	mapped_type& m(__init_registry[k]);
	if (m) {
		cerr << "Error: PRS macro by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = ExprAlloc_macro_definition_entry(k, &T::main,
		&T::check_num_params, &T::check_num_nodes, 
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED_CTOR__((__init_registry[k]));
	INVARIANT(n);
	return ExprAlloc_macro_registry.size();
}

//=============================================================================
typedef	entity::SPEC::spec_visitor_entry<ExprAlloc>
					ExprAlloc_spec_definition_entry;

typedef	util::qmap<string, ExprAlloc_spec_definition_entry>
					ExprAlloc_spec_registry_type;

static
const ExprAlloc_spec_registry_type
ExprAlloc_spec_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_ExprAlloc_spec_class(void) {
	typedef	ExprAlloc_spec_registry_type		registry_type;
	typedef	registry_type::iterator			iterator;
	typedef	registry_type::mapped_type		mapped_type;
	ExprAlloc_spec_registry_type&
		__init_registry(const_cast<ExprAlloc_spec_registry_type&>(
				ExprAlloc_spec_registry));
	const string k(T::name);
	mapped_type& m(__init_registry[k]);
	if (m) {
		cerr << "Error: SPEC directive by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	new (&m) ExprAlloc_spec_definition_entry(k, T::type, &T::main,
		&T::check_num_params, &T::check_num_nodes, 
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED_CTOR__((__init_registry[k]));
	INVARIANT(n);
	return ExprAlloc_spec_registry.size();
}

//=============================================================================
// class ExprAlloc method definitions

/**
	NOTE: 0 is an invalid index to the state's expr_pool.  
 */
ExprAlloc::ExprAlloc(state_type& _s, 
		const global_process_context& gpc,
		const ExprAllocFlags& f) :
		cflat_visitor(),
		cflat_context_visitor(gpc), 
		state(_s),
		g(NULL), 
		current_process_index(0), 	// to represent top-level
		total_exprs(FIRST_VALID_GLOBAL_EXPR), 	// non-zero!
		process_footprint_map(), 	// empty
		ret_ex_index(INVALID_EXPR_INDEX), 
		suppress_keeper_rule(false), 
		temp_rule(NULL),
		flags(f), expr_free_list()
#if PRSIM_PRECHARGE_INVARIANTS
		, netlists(NULL)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ExprAlloc::~ExprAlloc() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Local-only visit of rules and spec directives.
	\pre current_process_index has been set
 */
void
ExprAlloc::visit_rules_and_directives(const footprint& f) {
	STACKTRACE_VERBOSE;
	const entity::PRS::footprint& pfp(f.get_prs_footprint());
	pfp.accept(*this);
	// TODO: implement spec directives hierarchically
#if 0
	f.get_spec_footprint().accept(*this);
#endif

#if 0
	typedef footprint::invariant_pool_type::const_iterator
						const_iterator;
	// const expr_type_setter tmp(*this, PRS_LITERAL_TYPE_ENUM);
	const footprint::invariant_pool_type&
		ip(pfp.get_invariant_pool());
	const PRS_footprint_expr_pool_type& ep(pfp.get_expr_pool());
	const_iterator i(ip.begin()), e(ip.end());
	for ( ; i!=e; ++i) {
		// construct invariant expression
	//	ep[*i].accept(*this);
		visit(ep[*i]);
		link_invariant_expr(ret_ex_index);
	}
	}
#else
	// covered by visit(const PRS::footprint&), below
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to treat the top-footprint as global process 0.
 */
void
ExprAlloc::operator () (void) {
	STACKTRACE_VERBOSE;
	INVARIANT(!current_process_index);
	STACKTRACE_INDENT_PRINT("top-level process ..." << endl);
	STACKTRACE_INDENT_PRINT("current process index = "
		<< current_process_index << endl);
#if 0
	visit_rules_and_directives(*topfp);
	// already in auto_create_unique_process_subgraph
#endif
#if PRSIM_PRECHARGE_INVARIANTS
// hacknet requires your rules to be CMOS-implementable!
if (flags.auto_precharge_invariants) {
	NET::netlist_options netopt;	// mostly default
	netopt.print = false;
	netlists = excl_ptr<netlist_generator>(
		new netlist_generator(state.get_module(), cout, netopt));
	(*netlists)();		// run hierarchcail netlist generation first!
}
#endif

	// process top type
	const size_t ti = auto_create_unique_process_graph(*topfp);
	const unique_process_subgraph&
		ptemplate(state.unique_process_pool[ti]);
	process_sim_state& ps(state.process_state_array[current_process_index]);
	ps.allocate_from_type(ptemplate, ti, total_exprs);
	footprint_frame topff;
	topff.construct_top_global_context(*topfp, *parent_offset);
	const value_saver<const footprint_frame*> _ffs_(fpf, &topff);
#if ENABLE_STACKTRACE
	fpf->dump_frame(STACKTRACE_INDENT_PRINT("top-frame:")) << endl;
#endif
	// spec directives?
	const entity::footprint_frame_map_type&
		bmap(topff.get_frame_map<bool_tag>());
	const node_index_type node_pool_size =
		fpf->_footprint->get_instance_pool<bool_tag>().local_entries();
	update_expr_maps(ptemplate, node_pool_size, bmap, ps.get_offset());
	// problem: spec directives are still global, not per-process
try {
	const entity::SPEC::footprint& sfp(topfp->get_spec_footprint());
	sfp.accept(*this);
} catch (...) {
	report_instantiation_error(cerr);
	throw;
}

	topfp->accept(AS_A(global_entry_context&, *this));
	state.finish_process_type_map();	// finalize indices to pointers
#if ENABLE_STACKTRACE
	state.dump_struct(cerr << "Final global struct:" << endl) << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAlloc::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
//	visit_rules_and_directives(*topfp);
	// visit every unique bool once, every unique process once
	visit_local<bool_tag>(f, at_top());
	visit_local<process_tag>(f, at_top());
	visit_recursive(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only visit once per type!
 */
void
ExprAlloc::visit(const entity::PRS::footprint& pfp) {
	STACKTRACE_VERBOSE;
#if 0
	state.unique_process_pool.push_back(unique_process_subgraph());
	unique_process_subgraph& u(state.unique_process_pool.back());
	const util::value_saver<unique_process_subgraph*> tmp(g, &u);
	// resize the faninout_struct array as if it were local nodes
	// kludgy way of inferring the correct footprint
	u.local_faninout_map.resize(node_pool_size);
#endif
	cflat_visitor::visit(pfp);	// visit rules/macros/exprs
	{
	// handle invariants separately
	using entity::PRS::PRS_footprint_expr_pool_type;
	using entity::PRS::footprint;
	typedef footprint::invariant_pool_type::const_iterator
						const_iterator;
	// const expr_type_setter tmp(*this, PRS_LITERAL_TYPE_ENUM);
	const footprint::invariant_pool_type& ip(pfp.get_invariant_pool());
	const PRS_footprint_expr_pool_type& ep(pfp.get_expr_pool());
	const_iterator i(ip.begin()), e(ip.end());
	size_t j = 0;	// footprint's invariant_pool index
	for ( ; i!=e; ++i, ++j) {
		// construct invariant expression
	//	ep[*i].accept(*this);
#if INVARIANT_BACK_REFS
		visit(ep[i->first]);
#else
		visit(ep[*i]);
#endif
		link_invariant_expr(ret_ex_index, j);
	}
	}
#if PRSIM_PRECHARGE_INVARIANTS
if (flags.auto_precharge_invariants && netlists) {
	// NEW: process precharge invariants
	const entity::footprint* const fp = g->_footprint;
	NEVER_NULL(fp);
	const NET::netlist& nl(netlists->lookup_netlist(*fp));
	current_path_graph G(nl);
	// TODO: iterate over precharge nodes and generate expressions
	const current_path_graph::precharge_map_type&
		pcm(G.get_precharge_set());
	static const size_t nullgate = size_t(-1);
	size_t j = g->rule_pool.size();		// new indices
	current_path_graph::precharge_map_type::const_iterator
		mi(pcm.begin()), me(pcm.end());
for ( ; mi!=me; ++mi) {
	const netgraph_node& pn(G.get_node(*mi));
	// TODO: eliminate tautological/invariant precharge expressions
	// needs static analysis and a basic SAT solver
	if (pn.dir) {		// is logically P-stack
		const expr_index_type pe =
			__visit_current_path_graph_node_precharge_ground(
				G, *mi, nullgate);
		const expr_index_type le =
			__visit_current_path_graph_node_logic_power(
				G, *mi, nullgate);
		const expr_index_type inv =	// NAND
			allocate_new_not_expr(
			allocate_new_Nary_expr(
				entity::PRS::PRS_AND_EXPR_TYPE_ENUM, 2));
		link_child_expr(inv, pe, 0);
		link_child_expr(inv, le, 1);
		link_invariant_expr(inv, j);
		++j;
		// TODO: check against precharge sneak paths to output
	} else {		// is logically N-stack (common)
		const expr_index_type pe =
			__visit_current_path_graph_node_precharge_power(
				G, *mi, nullgate);
		const expr_index_type le =
			__visit_current_path_graph_node_logic_ground(
				G, *mi, nullgate);
		const expr_index_type inv =	// NAND
			allocate_new_not_expr(
			allocate_new_Nary_expr(
				entity::PRS::PRS_AND_EXPR_TYPE_ENUM, 2));
		link_child_expr(inv, pe, 0);
		link_child_expr(inv, le, 1);
		link_invariant_expr(inv, j);	// FINISH ME
		++j;
		// TODO: check against precharge sneak paths to output
	}
}	// end for all precharged internal nodes
}
#endif	// PRSIM_PRECHARGE_INVARIANTS
#if 0
	// definitely want to keep this
	if (flags.any_optimize() && expr_free_list.size()) {
		compact_expr_pools();
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visiting every unique bool once, setting attributes based on
	the canonical attributes associated at the owner level.  
 */
void
ExprAlloc::visit(const state_instance<bool_tag>& b) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	topfp->dump_canonical_name<bool_tag>(
		STACKTRACE_INDENT_PRINT("unique bool: "),
		lookup_global_id<bool_tag>(
			b.get_back_ref()->instance_index) -1, 
			dump_flags::default_value) << endl;
#endif
	const never_ptr<const instance_alias_info<bool_tag> >
		bref(b.get_back_ref());
	NEVER_NULL(bref);
	const size_t lbi = bref->instance_index;
	const size_t gbi = lookup_global_id<bool_tag>(lbi);
	state.node_pool[gbi].import_attributes(*bref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return index into state.unique_graph_pool that points to 
	the corresponding type.
 */
size_t
ExprAlloc::auto_create_unique_process_graph(const footprint& gpfp) {
	STACKTRACE_VERBOSE;
	typedef	process_footprint_map_type::const_iterator	const_iterator;
	size_t type_index;	// unique process type index
	const const_iterator f(process_footprint_map.find(&gpfp));
if (f == process_footprint_map.end()) {
	type_index = state.unique_process_pool.size();
	STACKTRACE_INDENT_PRINT("first time with this type, assigned id " << type_index << endl);
	process_footprint_map[&gpfp] = type_index;

	const state_instance<bool_tag>::pool_type&
		bmap(gpfp.get_instance_pool<bool_tag>());
	const node_index_type node_pool_size = bmap.local_entries();
	STACKTRACE_INDENT_PRINT("node_pool_size = " << node_pool_size << endl);
		state.unique_process_pool.push_back(unique_process_subgraph());
	unique_process_subgraph& u(state.unique_process_pool.back());
	u._footprint = &gpfp;
	const util::value_saver<unique_process_subgraph*> tmp(g, &u);
	// resize the faninout_struct array as if it were local nodes
	// kludgy way of inferring the correct footprint
	u.local_faninout_map.resize(node_pool_size);
	// map is now sized without sentinel, will need index adjustment!
	STACKTRACE_INDENT_PRINT("u.fan_map.size = " << u.local_faninout_map.size() << endl);
	visit_rules_and_directives(gpfp);
	// gpfp.accept(AS_A(cflat_visitor&, *this));
	// won't recurse to global_entry_context
	// definitely want to keep this
	if (flags.any_optimize() && expr_free_list.size()) {
		compact_expr_pools();
	}
#if ENABLE_STACKTRACE
	u.dump_struct(cerr << "Final local struct:" << endl) << endl;
#endif
	// auto-restore graph pointer
} else {
	// found existing type
	type_index = f->second;
	STACKTRACE_INDENT_PRINT("found existing type " <<
		type_index << endl);
}
	return type_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// needs to be re-written without use of global state_manager
/**
	Lookup this PRS_footprint in the address map.
	If this is the first time this type is visited, then allocate
	appropriate structures uniquely for this type.  
	Then all subsequent references to this type can just reference
	this and need no further allocation.  
	Note that the first process (index 0) should be reserved
	for the top-level process.  
	TODO: optimization: eliminate empty types, i.e. those without
		any expressions or rules, from the unique_process_pool.
 */
void
ExprAlloc::visit(const state_instance<process_tag>& gp) {
	STACKTRACE_VERBOSE;
	const size_t lpid = gp.get_back_ref()->instance_index;
	const util::value_saver<size_t>
		__pi__(current_process_index,
			lookup_global_id<process_tag>(lpid));
#if ENABLE_STACKTRACE
	topfp->dump_canonical_name<process_tag>(
		STACKTRACE_INDENT_PRINT("In process: "),
		current_process_index -1, dump_flags::default_value) << endl;
#endif
	const footprint_frame& gpff(gp._frame);
	const footprint& gpfp(*gpff._footprint);
	const global_offset& sgo(*g_offset);
	const global_offset
		b(sgo, gpfp, entity::add_local_private_tag()),
		c(sgo, gpfp, entity::add_total_private_tag());
	// set footprint frame using local frame? see global_entry_dumper::visit
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("offset: " << sgo << endl);
	STACKTRACE_INDENT_PRINT("local: " << b << endl);
	STACKTRACE_INDENT_PRINT("local-bnd: " << b << endl);
	gpff.dump_frame(STACKTRACE_INDENT_PRINT("instance-frame:")) << endl;
	fpf->dump_frame(STACKTRACE_INDENT_PRINT("actuals-frame:")) << endl;
#endif
	footprint_frame af(gpff, *fpf);
	af.extend_frame(sgo, b);
	// TODO: call construct_global_context instead
#if ENABLE_STACKTRACE
	af.dump_frame(STACKTRACE_INDENT_PRINT("EXT-frame:")) << endl;
#endif
	const value_saver<const footprint_frame*> _ff_(fpf, &af);
	// global_offset is used by spec_footprint visit (for now)
	const value_saver<const global_offset*> _g_(parent_offset, &sgo);
	// also set up proper unique_process references
	const entity::footprint* const fp(&gpfp);
	// this now works for pid=0, top-level
	const entity::footprint_frame_map_type&
		bmap(af.get_frame_map<bool_tag>());
	const node_index_type node_pool_size =
		gpff._footprint->get_instance_pool<bool_tag>().local_entries();
	STACKTRACE_INDENT_PRINT("node_pool_size = " << node_pool_size << endl);
	// append-allocate new state, based on type
	// now, allocate state for instance of this process type
	const size_t type_index = auto_create_unique_process_graph(gpfp);
	const unique_process_subgraph&
		ptemplate(state.unique_process_pool[type_index]);
	// TODO: ALERT! if (pxs == 0) this will still cost to 
	// lookup empty process with binary search through process_state_array
	process_sim_state& ps(state.process_state_array[current_process_index]);
	ps.allocate_from_type(ptemplate, type_index, total_exprs);
	// mapping update: for expr->process map, assign value
	// We are careful not to add an entry for EMPTY processes!
	// nice side effect of optimization: only map leaf cells with PRS!
	STACKTRACE_INDENT_PRINT("current process index = "
		<< current_process_index << endl);
	STACKTRACE_INDENT_PRINT("current type index = " << type_index << endl);
#if ENABLE_STACKTRACE
	copy(bmap.begin(), bmap.end(), std::ostream_iterator<size_t>(
		STACKTRACE_INDENT_PRINT("bmap: "), ","));
	STACKTRACE_STREAM << endl;
#endif
	update_expr_maps(ptemplate, node_pool_size, bmap, ps.get_offset());
try {
	// problem: spec directives are still global, not per-process
	const entity::SPEC::footprint& sfp(fp->get_spec_footprint());
	sfp.accept(*this);
} catch (...) {
	report_instantiation_error(cerr);
	throw;
}
	*g_offset = c;
}	// end method visit(const state_instance<process_tag>&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAlloc::update_expr_maps(const unique_process_subgraph& ptemplate,
		const size_t node_pool_size, 
		const footprint_frame_map_type& bmap, 
		const size_t pso) {
	STACKTRACE_VERBOSE;
	// mapping update: for expr->process map, assign value
	// We are careful not to add an entry for EMPTY processes!
	// nice side effect of optimization: only map leaf cells with PRS!
	const expr_index_type pxs = ptemplate.expr_pool.size();
	total_exprs += pxs;
	STACKTRACE_INDENT_PRINT("has " << pxs << " exprs" << endl);
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	INVARIANT(!state.global_expr_process_id_map.empty());
#endif
#if 0
	// problem: spec directives are still global, not per-process
	const entity::SPEC::footprint& sfp(fp->get_spec_footprint());
	sfp.accept(*this);
#endif
if (pxs) {
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	// Each process appends an entry for 
	// the process that *follows* it (P+1)!  (optionally delete last one)
	typedef state_type::global_expr_process_id_map_type::const_iterator
			map_iterator;
	const map_iterator x(--state.global_expr_process_id_map.end());	// last
	// get the cumulative number of expressions (state)
	// by adding the lower bound of the last appended entry
	// to its corresponding size.  
	const expr_index_type ex_offset = x->first +pxs;
	// STACKTRACE_INDENT_PRINT("prev offset = " << x->first << endl);
	STACKTRACE_INDENT_PRINT("offset[" << ex_offset << "] -> process " <<
		current_process_index +1 << endl);
	state.global_expr_process_id_map[ex_offset] = current_process_index +1;
#endif
	// connect global nodes to global fanout expressions
	node_index_type lni = 0;	// frame-map is 0-indexed
	for ( ; lni < node_pool_size; ++lni) {
		STACKTRACE_INDENT_PRINT("local node: " << lni << endl);
		INVARIANT(lni < bmap.size());
		const node_index_type gni = bmap[lni];
		// global index conversion, or local if top-level (pid=0)
		const faninout_struct_type&
			ff(ptemplate.local_faninout_map[lni]);
		const fanout_array_type& lfo(ff.fanout);
		State::node_type& n(state.node_pool[gni]);
		transform(lfo.begin(), lfo.end(), back_inserter(n.fanout), 
			bind2nd(std::plus<expr_index_type>(), pso));
		if (ff.has_fanin()) {
#if VECTOR_NODE_FANIN
			STACKTRACE_INDENT_PRINT("fanin-process: "
				<< current_process_index << endl);
			n.fanin.push_back(current_process_index);
#else
			finish(me);
#endif
			n.count_fanins(ff);	// count total OR-fanins
		}
	}
} else {
#if PRSIM_SEPARATE_PROCESS_EXPR_MAP
	// we have an empty process, but previous entry already added an 
	// entry pointing to this one.  
	// so we just "modify" the key in place without removing/re-inserting
	// a new entry in the sorted map, preserving order.
	typedef state_type::global_expr_process_id_map_type::iterator
			map_iterator;
	const map_iterator x(--state.global_expr_process_id_map.end());	// last
	x->second = current_process_index +1;	// equiv: ++(x->second);
	STACKTRACE_INDENT_PRINT("offset[" << x->first << "] -> process "
		<< x->second << endl);
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEBUG_CLEANUP		(0 && ENABLE_STACKTRACE)
/**
	Optimization transformations may have resulted in some holes in
	the expression pool, so we manually move them to end up with 
	a dense pool collection.  
 */
void
ExprAlloc::compact_expr_pools(void) {
	STACKTRACE_VERBOSE;
	const size_t holes = expr_free_list.size();
#if DEBUG_CLEANUP
	// may need to clean up
	cerr << "expr_free_list has " << holes <<
		" entries remaining."  << endl;
	cerr << "before compaction:" << endl;
	g->dump_struct(cerr);
#endif
	const size_t eps = g->expr_pool.size();
	const size_t move_end = eps -holes;
	// all expression indices from [move_end, eps) need to be relocated.
	std::set<expr_index_type> free_set, discard_set;
	while (expr_free_list.size()) {
		const expr_index_type i = free_list_acquire(expr_free_list);
		if (i >= move_end) {
#if DEBUG_CLEANUP
			cerr << "going to discard expr " << i << endl;
#endif
			discard_set.insert(i);
		} else {
#if DEBUG_CLEANUP
			cerr << "expr " << i << " saved for recycling" << endl;
#endif
			free_set.insert(i);
		}
	}
	expr_index_type i = move_end;
	for ( ; i < eps; ++i) {
	if (discard_set.find(i) == discard_set.end()) {
		// then move valid expression entry to the free_set
		const expr_index_type n = free_list_acquire(free_set);
#if DEBUG_CLEANUP
		cerr << "moving expr " << i << " to slot " << n << endl;
#endif
		{
			// move rule_map_entry, if applicable
			typedef	rule_map_type::iterator	rule_map_iterator;
			const rule_map_iterator f(g->rule_map.find(i));
			if (f != g->rule_map.end()) {
				g->rule_map[n] = f->second;
				g->rule_map.erase(f);
			}
			// this operation may disrupt the otherwise
			// bi-sorted (monotonic) relationship between
			// expr indices and rule indices...
		}
		expr_type& e(g->expr_pool[n]);
		graph_node_type& gn(g->expr_graph_node_pool[n]);
		INVARIANT(e.wiped());
		INVARIANT(gn.wiped());
		e = g->expr_pool[i];
		gn = g->expr_graph_node_pool[i];
		// relink parent, which may be node or expression
		if (e.is_root()) {
			const bool dir = g->lookup_rule(n)->direction();
#if PRSIM_WEAK_RULES
			const rule_strength
				str(REF_RULE_MAP(g, n).is_weak() ?
					WEAK_RULE : NORMAL_RULE);
#endif
			faninout_struct_type&
				f(g->local_faninout_map[e.parent]);
			fanin_array_type&
				fi(dir ? 
					f.pull_up STR_INDEX(str) :
					f.pull_dn STR_INDEX(str));
			replace(fi.begin(), fi.end(), i, n);
#undef	STR_INDEX
		} else {
			graph_node_type::child_entry_type&
				c(g->expr_graph_node_pool[e.parent]
					.children[gn.offset]);
			c.first = false;	// mark as expression
			c.second = n;		// re-establish child reference
		}
		// relink children
		size_t j = 0;
		INVARIANT(e.size == gn.children.size());
		for ( ; j<e.size; ++j) {
			graph_node_type::child_entry_type& c(gn.children[j]);
			if (c.first) {
				// then child is a node, update its fanout
				faninout_struct_type&
					f(g->local_faninout_map[c.second]);
				fanout_array_type&
					fo(f.fanout);
				replace(fo.begin(), fo.end(), i, n);
			} else {
				// child is an expression, update its parent
				g->expr_pool[c.second].parent = n;
			}
		}
	} // else just discard it
	} // end for-all expressions from the move point to the end
	INVARIANT(free_set.empty());
	// define an expression MOVE that reassigns IDs
	// and maintains connectivity
	g->expr_pool.resize(move_end);
#if 0
	// interface is missing right now from list_vector
	g->expr_graph_node_pool.resize(move_end);
#else
	{
	size_t k = holes;
	for ( ; k; --k) {
		g->expr_graph_node_pool.pop_back();
	}
	}
#endif
#if DEBUG_CLEANUP
	cerr << "after compaction:" << endl;
	g->dump_struct(cerr);
#endif
}	// end visit(const state_manager&)
#undef	DEBUG_CLEANUP

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update node fanin, and keep it consistent.  
	\pre state's node_array is already allocated.  
 */
void
ExprAlloc::visit(const footprint_rule& r) {
	STACKTRACE_VERBOSE;
//	STACKTRACE("ExprAlloc::visit(footprint_rule&)");
try {
	rule_type dummy_rule;
{
	// first, unconditionally create a rule entry in the state's rule
	// map for every top-level (root) expression that affects a node.  
//	rule_type& rule __ATTRIBUTE_UNUSED_CTOR__((st_rule_map[ret_ex_index]));
	const util::value_saver<rule_type*> rs(temp_rule, &dummy_rule);
	// now iterate over attributes to apply changes
	typedef footprint_rule::attributes_list_type	attr_list_type;
	typedef	attr_list_type::const_iterator		const_iterator;
	const_iterator i(r.attributes.begin()), e(r.attributes.end());
	entity::resolved_attribute::values_type empty;
	for ( ; i!=e; ++i) {
		ExprAlloc_attribute_registry[i->key].main(*this, 
			(i->values ? *i->values : empty));
	}
}
if (suppress_keeper_rule) {
	// do nothing, suppress rule
	suppress_keeper_rule = false;	// reset it for next rule
} else {
#if ENABLE_STACKTRACE
	get_current_footprint().dump_type(STACKTRACE_INDENT_PRINT("In type: ")) << endl;
#endif
	const entity::PRS::footprint::expr_pool_type& expr_pool(
		get_current_footprint().get_prs_footprint().get_expr_pool());
	STACKTRACE_INDENT_PRINT("expr_pool.size = " << expr_pool.size() << endl);
	STACKTRACE_INDENT_PRINT("r.expr_index = " << r.expr_index << endl);
	INVARIANT(size_t(r.expr_index) <= expr_pool.size());
	expr_pool[r.expr_index].accept(*this);
	const size_t top_ex_index = ret_ex_index;
	// r.output_index gives the local unique ID,
	// which needs to be translated to global ID.
	// bfm[...] refers to a state_instance<bool_tag> (1-indexed)
	// const size_t j = bfm[r.output_index-1];
	const size_t ni = lookup_local_bool_id(r.output_index);

	// ignored: state.expr_expr_graph_node_pool[top_ex_index].offset
	// not computing node fanin?  this would be the place to do it...
	// can always compute it (cacheable) offline
	STACKTRACE_INDENT_PRINT("expr " << top_ex_index << " pulls node " <<
		ni << (r.dir ? " up" : " down") << endl);
	// need to process attributes BEFORE linking to node
	// because depends on weak attribute
#if ENABLE_STACKTRACE
	g->dump_struct(cerr) << endl;
#endif
	INVARIANT(top_ex_index == ret_ex_index);	// sanity check
	// following order matters b/c of rule_map access
	link_node_to_root_expr(ni, top_ex_index, r.dir, dummy_rule
#if PRSIM_WEAK_RULES
		, rule_strength(dummy_rule.is_weak())
#endif
		);
	// REF_RULE_MAP(g, ret_ex_index) = dummy_rule;	// copy over temporary
}
} catch (...) {
	cerr << "FATAL: error during prs rule allocation." << endl;
#if 0
	// botched attempt to print error message
	typedef	entity::PRS::cflat_prs_printer	cflat_prs_printer;
	cflat_options cfo;
	cfo.primary_tool = cflat_options::TOOL_PRSIM;
	entity::PRS::cflat_prs_printer pp(cerr, cfo);
	const cflat_prs_printer::module_setter mtmp(pp, *this);
	const cflat_prs_printer::footprint_frame_setter ftmp(pp, *this);
	const cflat_prs_printer::expr_pool_setter etmp(pp, *this);
	r.accept(pp);	// do we guarantee that cflat doesn't throw?
#endif
	throw;
}
}	// end method visit(const footprint_rule&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new leaf literal expression in the sim state object.  
	\param ni resolved global ID of unique node.
		NOW is LOCAL node index.  
	\return index of new literal expression, allocated in the state. 
		NOTE: this does not set ret_ex_index on return, it is the
		caller's responsibility to use it.  
 */
expr_index_type
ExprAlloc::allocate_new_literal_expr(const node_index_type ni) {
	// NOTE: Expr's parent and ExprGraphNode's offset
	// fields are not set until returned to caller!
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("node id = " << ni << endl);
	expr_index_type ret;
	if (expr_free_list.size()) {
		ret = free_list_acquire(expr_free_list);
		g->expr_pool[ret] =
			expr_type(expr_struct_type::EXPR_NODE,1);
		g->expr_graph_node_pool[ret] = graph_node_type();
	} else {
		ret = g->expr_pool.size();
		g->expr_pool.push_back(expr_type(
			expr_struct_type::EXPR_NODE,1));
		g->expr_graph_node_pool.push_back(graph_node_type());
		STACKTRACE_INDENT_PRINT("appending graph_expr_pool..." << endl);
	}
	g->local_faninout_map[ni].fanout.push_back(ret);
	g->expr_graph_node_pool[ret].push_back_node(ni);
	// literal graph node has no children
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new NOT expression in the sim state object.  
	\param ei the index of the input expression.
	\return newly allocated ID of the negated expression, 
		except that now, no allocation takes place, just a bit flip.  
 */
expr_index_type
ExprAlloc::allocate_new_not_expr(const expr_index_type ei) {
	STACKTRACE_INDENT_PRINT("sub_ex_index = " << ei << endl);
	// just negate the expression, duh!
	g->expr_pool[ei].toggle_not();
	return ei;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param parent index of the parent expression
	\param child index of the child subexpression
	\param (0-indexed) offset of the child relative to parent, 
		the ordinal number of child.  
 */
void
ExprAlloc::link_child_expr(const expr_index_type parent,
		const expr_index_type child, const size_t offset) {
	STACKTRACE_VERBOSE;
	// g->expr_pool[child].parent = parent;
	g->expr_pool[child].set_parent_expr(parent);
	graph_node_type& child_node(g->expr_graph_node_pool[child]);
	child_node.offset = offset;
	g->expr_graph_node_pool[parent].push_back_expr(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Links child expression to a new slot by resizing parent.
	Made possible by using vector for children_array_type.
 */
void
ExprAlloc::append_child_expr(const expr_index_type parent,
		const expr_index_type child) {
	STACKTRACE_VERBOSE;
	graph_node_type& pg(g->expr_graph_node_pool[parent]);
	const size_t offset = pg.children.size();	// 0-based index
	++(g->expr_pool[parent].size);
	g->expr_pool[child].set_parent_expr(parent);
	graph_node_type& child_node(g->expr_graph_node_pool[child]);
	child_node.offset = offset;
	pg.push_back_expr(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param type one of the entity::PRS::PRS_* enums defined in
		"Object/lan/PRS_enum.h", must be either AND or OR.  
	\param sz the number of children of this expression.  
	\return index of newly allocated N-ary expression (and/or).
 */
expr_index_type
ExprAlloc::allocate_new_Nary_expr(const char type, const size_t sz) {
	STACKTRACE_VERBOSE;
	static const size_t lim = std::numeric_limits<expr_count_type>::max();
	INVARIANT(sz);
	if (sz > lim) {
		// assert(sz < lim);
		cerr << "FATAL: size of prsim expression exceeded limit: " <<
			sz << " > " << lim << endl;
		THROW_EXIT;
	}
	const expr_type
		temp((type == entity::PRS::PRS_AND_EXPR_TYPE_ENUM ?
		expr_struct_type::EXPR_AND : expr_struct_type::EXPR_OR), sz);
if (expr_free_list.size()) {
	const expr_index_type ret = free_list_acquire(expr_free_list);
	g->expr_pool[ret] = temp;
	g->expr_graph_node_pool[ret] = graph_node_type();
	return ret;
} else {
	const expr_index_type ret = g->expr_pool.size();
	g->expr_pool.push_back(temp);
	g->expr_graph_node_pool.push_back(graph_node_type());
	STACKTRACE_INDENT_PRINT("appending graph_expr_pool..." << endl);
	return ret;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param _e the index of the newly constructed (and/or) expression
		whose subexpressions are individually scanned and 
		transformed out if they are plain literals.  
 */
void
ExprAlloc::fold_literal(const expr_index_type _e) {
	STACKTRACE_VERBOSE;
	// inspect each child: if is a literal, 
	// then transform the leaf node out.
	typedef	graph_node_type::child_entry_type	child_entry_type;
	size_t j = 0;
	graph_node_type& gn(g->expr_graph_node_pool[_e]);
	const size_t sz = g->expr_pool[_e].size;
	for ( ; j<sz; ++j) {
		child_entry_type& c(gn.children[j]);
		// child indexes expr, not node
		if (!c.first) {
		// index of entry refers to expression
		const expr_index_type ei(c.second);
		const expr_type& oe(g->expr_pool[ei]);
		const graph_node_type& og(g->expr_graph_node_pool[ei]);
		const child_entry_type& ogc(og.children[0]);
		// if is single node-child parent
		if (oe.is_trivial() && ogc.first) {
			STACKTRACE_INDENT_PRINT("releasing " << ei << endl);
			// re-connect graph, skipping over
			// the intermediate trivial node
			const node_index_type cn = ogc.second;
			// replace grandparent's child connect.
			c = ogc;
			fanout_array_type& fo(g->local_faninout_map[cn].fanout);
			// grandchild's has no parent connect.
			// replace child's fanout: ei -> _e
			replace(fo.begin(), fo.end(), ei, _e);
			// there should be no references to 
			// expression ei remaining
			g->void_expr(ei);
			free_list_release(expr_free_list, ei);
		}	// else no excision
		}	// end if is node_index
	}	// end for all subexpressions
}	// end ExprAlloc::fold_literal()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param _e the index of the newly constructed (and/or) expression
		whose subexpressions are scanned to see if they are
		negated literals.  If all are such, then we can apply
		DeMorgan's transformation to eliminate those literals.  
 */
void
ExprAlloc::denormalize_negation(const expr_index_type _e) {
	STACKTRACE_VERBOSE;
	// inspect each child: determine if is
	// negated expression (no bare nodes)
	typedef	graph_node_type::child_entry_type	child_entry_type;
	size_t j = 0;
	bool denorm = true;
	const size_t sz = g->expr_pool[_e].size;
	graph_node_type& gn(g->expr_graph_node_pool[_e]);
	// first pass: 
	for ( ; j<sz && denorm; ++j) {
		const child_entry_type& c(gn.children[j]);
		if (!c.first) {		// is expression
			const expr_index_type ei(c.second);
			const expr_type& oe(g->expr_pool[ei]);
			// const graph_node_type& og(g->expr_graph_node_pool[ei]);
			// const child_entry_type& ogc(og.children[0]);
			// if is single node-child parent
			if (oe.size == 1 && oe.is_not()) {
					// && ogc.first
				// ok, leave denorm as is
			} else	denorm = false;
		} else	denorm = false;
	}	// end for scanning for denormalization cond.
	if (denorm) {
		// second pass, qualified for denormalization
		STACKTRACE_INDENT_PRINT("denormalizing... " << endl);
		for (j=0; j<sz; ++j) {
			child_entry_type& c(gn.children[j]);
			// child indexes expr, not node
			// index of entry refers to expression
			const expr_index_type ei(c.second);
			const graph_node_type& og(g->expr_graph_node_pool[ei]);
			const child_entry_type& ogc(og.children[0]);
			// if is single node-child parent
			// re-connect graph, skipping over
			// the intermediate trivial node
			// replace grandparent's child connect.
			if (ogc.first) {
				c = ogc;
				const node_index_type cn = ogc.second;
				fanout_array_type&
					fo(g->local_faninout_map[cn].fanout);
				// grandchild's has no parent connect.
				// replace child's fanout: ei -> _e
				replace(fo.begin(), fo.end(), ei, _e);
				// there should be no references to 
				// expression ei remaining
			STACKTRACE_INDENT_PRINT("releasing " << ei << endl);
				g->void_expr(ei);
				free_list_release(expr_free_list, ei);
			} else {
				// is an expression, just keep structure
				// the same and negate the type
				const expr_index_type ce = ogc.second;
				g->expr_pool[ce].toggle_not();
			}
		}	// end for all subexpressions
		// apply DeMorgan's law
		g->expr_pool[_e].toggle_demorgan();
	}	// end if denorm
} 	// end ExprAlloc::denoramlize_negation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create expressions bottom-up.  
	\pre state's node_array is already allocated.  
 */
void
ExprAlloc::visit(const footprint_expr_node& e) {
	STACKTRACE("ExprAlloc::visit(footprint_expr_node&)");
	const size_t sz = e.size();
	const char type = e.get_type();
	// NOTE: 1-indexed
	const entity::PRS::footprint::expr_pool_type& expr_pool(
		get_current_footprint().get_prs_footprint().get_expr_pool());
switch (type) {
	// enumerations from "Object/lang/PRS_enum.h"
	case entity::PRS::PRS_LITERAL_TYPE_ENUM: {
		STACKTRACE_INDENT_PRINT("literal" << endl);
		// leaf node
		INVARIANT(sz == 1);
		// lookup global bool ID
		const node_index_type ni = lookup_local_bool_id(e.only());
		ret_ex_index = allocate_new_literal_expr(ni);
		break;
	}
	case entity::PRS::PRS_NOT_EXPR_TYPE_ENUM: {
		STACKTRACE_INDENT_PRINT("not" << endl);
		INVARIANT(sz == 1);
		expr_pool[e.only()].accept(*this);
		const size_t sub_ex_index = ret_ex_index;
		ret_ex_index = allocate_new_not_expr(sub_ex_index);
		break;
	}
	case entity::PRS::PRS_AND_EXPR_TYPE_ENUM:
		// yes, fall-through
	case entity::PRS::PRS_OR_EXPR_TYPE_ENUM: {
		STACKTRACE_INDENT_PRINT("or/and" << endl);
		const expr_index_type last = allocate_new_Nary_expr(type, sz);
		size_t i = 1;
		for ( ; i<=sz; i++) {
			// reminder: e is 1-indexed while 
			// ExprGraphNode::children is 0-indexed
			expr_pool[e[i]].accept(*this);
			const size_t sub_ex_index = ret_ex_index;
			link_child_expr(last, sub_ex_index, i-1);
		}
		// welcome to graph surgery!
		if (flags.is_fold_literals()) {
			fold_literal(last);
		}	// end if is_fold_literals()
		if (flags.is_denormalize_negations()) {
			denormalize_negation(last);
		}	// end if is_denormalize_negations()
		ret_ex_index = last;
		break;
	}
	case entity::PRS::PRS_NODE_TYPE_ENUM: {
		STACKTRACE_INDENT_PRINT("node" << endl);
		INVARIANT(sz == 1);
		expr_pool[e.only()].accept(*this);
		break;
	}
	default:
		THROW_EXIT;
		break;
}	// end switch
	// 'return' the index of the expr just allocated in ret_ex_index
#if 0 && ENABLE_STACKTRACE
	g->dump_struct(cerr) << endl;
#endif
}	// end method visit(const footprint_expr_node&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAlloc::visit(const footprint_macro& m) {
	STACKTRACE_VERBOSE;
	const ExprAlloc_macro_definition_entry&
		d(ExprAlloc_macro_registry[m.name]);
if (d) {
	if (!d.check_param_args(m.params).good
			|| !d.check_node_args(m.nodes).good) {
		cerr << "Error with PRS macro." << endl;
		THROW_EXIT;
	} else {
		d.main(*this, m.params, m.nodes);
	}
} else {
	// b/c some macros are undefined for specific tools
	cerr << "WARNING: ignoring unknown macro \'"
		<< m.name << "\'." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ExprAlloc::visit(const footprint_directive& s) {
	STACKTRACE_VERBOSE;
	const ExprAlloc_spec_definition_entry&
		d(ExprAlloc_spec_registry[s.name]);
if (d) {
	if (!d.check_param_args(s.params).good
			|| !d.check_node_args(s.nodes).good) {
		cerr << "Error with SPEC directive." << endl;
		THROW_EXIT;
	} else {
		d.main(*this, s.params, s.nodes);
	}
} else {
	// b/c some directives are undefined for specific tools
	cerr << "WARNING: ignoring unknown directive \'"
		<< s.name << "\'." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fake an invariant as a rule that doesn't really pull any node.
	\param top_ex_index local graph's expression index that was allocated.
	\param src_inv_index footprint's invariant index.  
 */
void
ExprAlloc::link_invariant_expr(const expr_index_type top_ex_index, 
		const size_t src_inv_index) {
	expr_type& ne(g->expr_pool[top_ex_index]);
//	graph_node_type& ng(g->expr_graph_node_pool[top_ex_index]);
	rule_type dummy;
	dummy.set_invariant();
//	ng.offset = fin.size();	
	ne.invariant_root();
	g->rule_map[top_ex_index] = g->rule_pool.size();	// map
	g->invariant_map[top_ex_index] = src_inv_index;
	g->rule_pool.push_back(dummy);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Utility routine for connecting an output node to
	its root pull-up or pull-down expression.  
	This automatically takes care of OR-combination by allocating
	new root expressions when necessary.  
	\param ni is the target output node (1-based).
	\param top_ex_index is the root expression pulling the target node.
	\param dir is true if pulling up, false if pulling down.
	\param w is true if rule is weak.  
 */
void
ExprAlloc::link_node_to_root_expr(const node_index_type ni,
		const expr_index_type top_ex_index, const bool dir, 
		const rule_type& dummy
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) {
	STACKTRACE("ExprAlloc::link_node_to_root_expr(...)");
	STACKTRACE_INDENT_PRINT("linking expr " << top_ex_index <<
		" to node " << ni << ' ' << (dir ? '+' : '-') << endl);
	NEVER_NULL(g);
	STACKTRACE_INDENT_PRINT("expr-pool.size = " <<
		g->expr_pool.size() << endl);
	STACKTRACE_INDENT_PRINT("expr-node-pool.size = " <<
		g->expr_graph_node_pool.size() << endl);
	STACKTRACE_INDENT_PRINT("fan-map.size = " <<
		g->local_faninout_map.size() << endl);
	expr_type& ne(g->expr_pool[top_ex_index]);
	graph_node_type& ng(g->expr_graph_node_pool[top_ex_index]);
	fanin_array_type& fin(g->local_faninout_map[ni].get_pull_expr(dir
#if PRSIM_WEAK_RULES
			, w
#endif
		));
	// root expression's position in node's fanin (OR-combination)
	ng.offset = fin.size();	
	fin.push_back(top_ex_index);		// append to fanin rules
	ne.pull(ni);				// set as a root expression
	g->rule_map[top_ex_index] = g->rule_pool.size();	// map
	g->rule_pool.push_back(dummy);
	g->rule_pool.back().set_direction(dir);
}	// end ExprAlloc::link_node_to_root_expr(...)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_PRECHARGE_INVARIANTS
/**
	TODO: this is written for pull-up PFETs only, generalize me.
	\param terminals predicate that returns true for nodes in terminal set
	\param path_pred predicate or types of FETs to visit.
	\param ni the internal graph node index to visit (not bool footprint),
		should be an internal node, else terminate on output node.
	\param ti the index of the transistor edge taken to reach here
		If this value is -1, it is only start of recursion.
	\return index to newly allocated sub-expression in pool.
 */
template <bool (current_path_graph::*terminals)(const size_t) const,
		 vector<transistor_edge> netgraph_node::*edge_set,
		bool (NET::transistor_base::*path_pred)(void) const>
expr_index_type
ExprAlloc::__visit_current_path_graph_generic(
		const current_path_graph& G, const size_t ni, 
		const size_t ti) {
//	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("netgraph node: " << ni <<
		", transistor: " << ti << endl);
	static const size_t nullgate = size_t(-1);
	const bool tv = (ti != nullgate);
	expr_index_type next = 0;
	if (tv) {
		const transistor_base&
			gt(G.get_edge(ti));	// may be invalid
		STACKTRACE_INDENT_PRINT("gate node " << gt.gate << endl);
		const node_index_type gi =
			G.translate_logical_bool_index(gt.gate);
		INVARIANT(gi);
		STACKTRACE_INDENT_PRINT("literal index " << gi << endl);
		next = allocate_new_literal_expr(lookup_local_bool_id(gi));
		// depending on PFET or NFET
		if (gt.is_PFET()) {
			next = allocate_new_not_expr(next);
		}
	}
if ((G.*terminals)(ni)) {
	// have reached power supply, just return literal expression of gi
	return next;
} else {
	const netgraph_node& n(G.get_node(ni));
	// TODO: include bidirectional edges
	INVARIANT((n.*edge_set).size());
	vector<transistor_edge>::const_iterator
		i((n.*edge_set).begin()), e((n.*edge_set).end());
	vector<expr_index_type> se;	// collect subexpressions here
	for ( ; i!=e; ++i) {
		const transistor_base& t(G.get_edge(i->index));
		// should we use the subgraph_paths to check?
		// were they needed?
	if ((t.*path_pred)()) {
		// recursively build up expression tree
		se.push_back(__visit_current_path_graph_generic<
			terminals, edge_set, path_pred>(
			G, i->destination, i->index));
	}	// else skip
	}
	// OR-combine terms if necessary
	const size_t sz = se.size();
	INVARIANT(sz);
	expr_index_type last;
	if (sz > 1) {
		last = allocate_new_Nary_expr(
			entity::PRS::PRS_OR_EXPR_TYPE_ENUM, sz);
		size_t j = 0;
		for ( ; j<sz; ++j) {
			// ExprGraphNode::children is 0-indexed
			link_child_expr(last, se[j], j);
		}
	} else {
		last = se.front();
	}
	if (next) {
		const expr_type& l(g->expr_pool[last]);
		if (l.is_conjunctive() && !l.is_not()) {	// AND
			// just append to previous AND expression
			append_child_expr(last, next);
			return last;
		} else {
			// create a new AND expression
			const expr_index_type ret = allocate_new_Nary_expr(
				entity::PRS::PRS_AND_EXPR_TYPE_ENUM, 2);
			link_child_expr(ret, last, 0);
			link_child_expr(ret, next, 1);
			return ret;
		}
	} else {
		return last;
	}
}
}	// end __visit_current_path_graph_generic

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_index_type
ExprAlloc::__visit_current_path_graph_node_precharge_power(
		const current_path_graph& G, const size_t ni, 
		const size_t ti) {
	STACKTRACE_BRIEF;
	return __visit_current_path_graph_generic<
		&current_path_graph::node_is_power,
		&netgraph_node::up_edges,
		&NET::transistor_base::is_precharge>(G, ni, ti);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_index_type
ExprAlloc::__visit_current_path_graph_node_precharge_ground(
		const current_path_graph& G, const size_t ni, 
		const size_t ti) {
	STACKTRACE_BRIEF;
	return __visit_current_path_graph_generic<
		&current_path_graph::node_is_ground,
		&netgraph_node::dn_edges,
		&NET::transistor_base::is_precharge>(G, ni, ti);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_index_type
ExprAlloc::__visit_current_path_graph_node_logic_power(
		const current_path_graph& G, const size_t ni, 
		const size_t ti) {
	STACKTRACE_BRIEF;
	return __visit_current_path_graph_generic<
		&current_path_graph::node_is_power,
		&netgraph_node::up_edges,
		&NET::transistor_base::is_not_precharge>(G, ni, ti);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_index_type
ExprAlloc::__visit_current_path_graph_node_logic_ground(
		const current_path_graph& G, const size_t ni, 
		const size_t ti) {
	STACKTRACE_BRIEF;
	return __visit_current_path_graph_generic<
		&current_path_graph::node_is_ground,
		&netgraph_node::dn_edges,
		&NET::transistor_base::is_not_precharge>(G, ni, ti);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_index_type
ExprAlloc::__visit_current_path_graph_node_logic_output_up(
		const current_path_graph& G, const size_t ni, 
		const size_t ti) {
	STACKTRACE_BRIEF;
	return __visit_current_path_graph_generic<
		&current_path_graph::node_is_logic_signal,
		&netgraph_node::up_edges,
		&NET::transistor_base::is_not_precharge>(G, ni, ti);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_index_type
ExprAlloc::__visit_current_path_graph_node_logic_output_down(
		const current_path_graph& G, const size_t ni, 
		const size_t ti) {
	STACKTRACE_BRIEF;
	return __visit_current_path_graph_generic<
		&current_path_graph::node_is_logic_signal,
		&netgraph_node::dn_edges,
		&NET::transistor_base::is_not_precharge>(G, ni, ti);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif	// PRSIM_PRECHARGE_INVARIANTS

//=============================================================================
/**
	Local namespace for prsim rule attributes.  
 */
namespace prsim_rule_attributes {

/**
	Macro for declaring attribute classes.  
	Here, the vistor_type is prsim's ExprAlloc.
	NOTE: these classes should have hidden visibility.  
 */
#define DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(class_name, att_name) \
	DECLARE_PRS_RULE_ATTRIBUTE_CLASS(class_name, ExprAlloc)		\
	DEFINE_PRS_RULE_ATTRIBUTE_CLASS(class_name, att_name, 		\
		register_ExprAlloc_attribute_class)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(After, "after")

/**
	Delay values real or floating or both?
 */
void
After::main(visitor_type& v, const values_type& a) {
	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.get_temp_rule());
	const values_type::value_type& d(a.front());
	// assert type cast, b/c already checked
	r.set_delay(d.is_a<const pint_const>()->static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_AFTER_RANGE
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(AfterMin, "after_min")
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(AfterMax, "after_max")

static
void
check_after_min_max(const State::rule_type& r) {
	if (r.after_max < r.after_min) {
		cerr << "Error: after_min must be <= after_max! (got: "
			<< r.after_min << " and " << r.after_max
			<< ")" << endl;
		THROW_EXIT;
	}
}

void
AfterMin::main(visitor_type& v, const values_type& a) {
	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.get_temp_rule());
	const values_type::value_type& d(a.front());
	// assert type cast, b/c already checked
	r.after_min = d.is_a<const pint_const>()->static_constant_value();
	typedef	State::time_traits	time_traits;
	if (!time_traits::is_zero(r.after_max)) {
		check_after_min_max(r);
	}
}

void
AfterMax::main(visitor_type& v, const values_type& a) {
	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.get_temp_rule());
	const values_type::value_type& d(a.front());
	// assert type cast, b/c already checked
	r.after_max = d.is_a<const pint_const>()->static_constant_value();
	typedef	State::time_traits	time_traits;
	if (!time_traits::is_zero(r.after_min)) {
		check_after_min_max(r);
	}
}
#endif	// PRSIM_AFTER_RANGE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Always_Random, "always_random")

/**
        Sets or clears always_random flag on a rule.
        Value is 1 (true) or 0.
 */
void
Always_Random::main(visitor_type& v, const values_type& a) {
	typedef visitor_type::rule_type rule_type;
	rule_type& r(v.get_temp_rule());
if (a.size()) {
	const values_type::value_type& w(a.front());
	if (w.is_a<const pint_const>()->static_constant_value())
		r.set_always_random();
	else    r.clear_always_random();
} else {
	// unspecified value, default true
	r.set_always_random();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Weak, "weak")

/**
	Sets or clears weak flag on a rule.  
	Value is 1 (true) or 0.
 */
void
Weak::main(visitor_type& v, const values_type& a) {
	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.get_temp_rule());
if (a.size()) {
	const values_type::value_type& w(a.front());
	if (w.is_a<const pint_const>()->static_constant_value())
		r.set_weak();
	else	r.clear_weak();
} else {
	// unspecified value, default true
	r.set_weak();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Unstab, "unstab")

/**
	Sets or clears unstab flag on a rule.  
	Value is 1 (true) or 0.
 */
void
Unstab::main(visitor_type& v, const values_type& a) {
	typedef	visitor_type::rule_type	rule_type;
	rule_type& r(v.get_temp_rule());
if (a.size()) {
	const values_type::value_type& w(a.front());
	if (w.is_a<const pint_const>()->static_constant_value())
		r.set_unstable();
	else	r.clear_unstable();
} else {
	// unspecified value, default true
	r.set_unstable();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Keeper, "keeper")

/**
	No effect in prsim, since nodes are implicitly state holding, 
	so ignored.  
 */
void
Keeper::main(visitor_type& v, const values_type& a) {
	// do nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(IsKeeper, "iskeeper")

/**
	Since prsim does not actually simulate staticizers (keepers), 
	this tells prsim to completely ignore these rules.  
	The exception is to honor the fast-weak-keepers mode, 
	and treat such rules as a weak and fast.  
 */
void
IsKeeper::main(visitor_type& v, const values_type& a) {
	if (a.empty()) {
		// default when no value given
		v.suppress_keeper_rule = true;
	} else {
		// honor the value given
		const values_type::value_type& k(a.front());
		v.suppress_keeper_rule = 
			k.is_a<const pint_const>()->static_constant_value();
	}
#if PRSIM_WEAK_RULES
	if (v.suppress_keeper_rule && v.get_flags().fast_weak_keepers) {
	// in this mode, interpret iskeeper rules as weak=1,after=0
		typedef	visitor_type::rule_type	rule_type;
		rule_type& r(v.get_temp_rule());
		r.set_weak();
		r.set_delay(State::time_traits::zero);
		v.suppress_keeper_rule = false;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(IsCKeeper, "isckeeper")

/**
	Combinational feedback rules should be simulated in prsim.
 */
void
IsCKeeper::main(visitor_type& v, const values_type& a) {
#if 0
	if (a.empty()) {
		// default when no value given
		v.suppress_keeper_rule = true;
	} else {
		// honor the value given
		const values_type::value_type& k(a.front());
		v.suppress_keeper_rule = 
			k.is_a<const pint_const>()->static_constant_value();
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Diode, "diode")

/**
	For convenience, overload the use of suppress_keeper_rule
	to suppress diode rules.
 */
void
Diode::main(visitor_type& v, const values_type& a) {
	if (a.empty()) {
		// default when no value given
		v.suppress_keeper_rule = true;
	} else {
		// honor the value given
		const values_type::value_type& k(a.front());
		v.suppress_keeper_rule = 
			k.is_a<const pint_const>()->static_constant_value();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Comb, "comb")

/**
	No effect in prsim, since nodes are implicitly state holding, 
	so ignored.  
 */
void
Comb::main(visitor_type& v, const values_type& a) {
	// do nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(Output, "output")

/**
	No effect in prsim.  
 */
void
Output::main(visitor_type& v, const values_type& a) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(LoadCap, "loadcap")

/**
	No effect in prsim.  
	Eventually will have effect in more realistic sprsim.  
 */
void
LoadCap::main(visitor_type& v, const values_type& a) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(N_reff, "N_reff")
DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS(P_reff, "P_reff")

/**
	No effect in prsim.  
 */
void
N_reff::main(visitor_type& v, const values_type& a) { }
void
P_reff::main(visitor_type& v, const values_type& a) { }

#undef	DECLARE_AND_DEFINE_PRSIM_RULE_ATTRIBUTE_CLASS
}	// end namespace prsim_rule_attributes

//=============================================================================
/**
	Namespace for macro implementations specific to prsim.  
 */
namespace prsim_macros {
// DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(Echo, "echo")	// ignored

/**
	Convenient macro for declaring prsim PRS macros.  
 */
#define DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(class_name, att_name)	\
	DECLARE_PRS_MACRO_CLASS(class_name, ExprAlloc)			\
	DEFINE_PRS_MACRO_CLASS(class_name, att_name, 			\
		register_ExprAlloc_macro_class)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(PassN, "passn")

/**
	Produce the rule:
		after 0	g & ~s -> d-
	\param params unused.  
	\pre each node group argument contains exactly one node.  
 */
void
PassN::main(visitor_type& v, const param_args_type& params,
		const node_args_type& nodes) {
	STACKTRACE_VERBOSE;
	const expr_index_type g =
		v.allocate_new_literal_expr(
			v.lookup_local_bool_id(*nodes[0].begin()));
	const expr_index_type s =
		v.allocate_new_literal_expr(
			v.lookup_local_bool_id(*nodes[1].begin()));
	const node_index_type d = v.lookup_local_bool_id(*nodes[2].begin());
	// construct and allocate rule
	const expr_index_type ns = v.allocate_new_not_expr(s);
	const expr_index_type pe =
		v.allocate_new_Nary_expr(entity::PRS::PRS_AND_EXPR_TYPE_ENUM,2);
	typedef	visitor_type::rule_type	rule_type;
	v.link_child_expr(pe, g, 0);
	v.link_child_expr(pe, ns, 1);
	v.link_node_to_root_expr(d, pe, false, rule_type()
#if PRSIM_WEAK_RULES
		, NORMAL_RULE
#endif
		);	// pull-down

	rule_type& r(REF_RULE_MAP(v.g, pe));
	r.set_delay(visitor_type::state_type::time_traits::zero);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS(PassP, "passp")

/**
	Produce the rule:
		after 0	~g & s -> d+
	\param params unused.  
	\pre each node group argument contains exactly one node.  
 */
void
PassP::main(visitor_type& v, const param_args_type& params,
		const node_args_type& nodes) {
	STACKTRACE_VERBOSE;
	const expr_index_type g =
		v.allocate_new_literal_expr(
			v.lookup_local_bool_id(*nodes[0].begin()));
	const expr_index_type s =
		v.allocate_new_literal_expr(
			v.lookup_local_bool_id(*nodes[1].begin()));
	const node_index_type d = v.lookup_local_bool_id(*nodes[2].begin());
	// construct and allocate rule
	const expr_index_type ng = v.allocate_new_not_expr(g);
	const expr_index_type pe =
		v.allocate_new_Nary_expr(entity::PRS::PRS_AND_EXPR_TYPE_ENUM,2);
	typedef	visitor_type::rule_type	rule_type;
	v.link_child_expr(pe, ng, 0);
	v.link_child_expr(pe, s, 1);
	v.link_node_to_root_expr(d, pe, true, rule_type()
#if PRSIM_WEAK_RULES
		, NORMAL_RULE
#endif
		);	// pull-up

	rule_type& r(REF_RULE_MAP(v.g, pe));
	r.set_delay(visitor_type::state_type::time_traits::zero);
}

#undef	DECLARE_AND_DEFINE_PRSIM_MACRO_CLASS
}	// end namespace prsim_macros

//=============================================================================
/**
	Namespace for prsim-specific implementations of spec directives.  
 */
namespace prsim_spec_directives {

/**
	Convenient macro for declaring prsim SPEC directives.  
	For now, all PRSIM spec directives are for bool meta-type only.
 */
#define	DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(class_name, spec_name) \
	DECLARE_SPEC_DIRECTIVE_CLASS(class_name, ExprAlloc)		\
	DEFINE_SPEC_DIRECTIVE_CLASS(class_name, spec_name, 		\
		register_ExprAlloc_spec_class)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(UnAliased, "unaliased")

void
UnAliased::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	if (!__main(v, nodes).good) {
		cerr << "Error: detected aliased nodes during "
			"processing of \'unaliased\' directive."
			<< endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(Assert, "assert")

void
Assert::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	// does absolutely nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this is useful for both LVS and hackt-prsim
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(LVS_exclhi, "exclhi")

/**
	Allocates a lock flag for checking exclusiveness.  
	First collect all nodes into a set (for uniqueness) of node indices,
	then registers each node with lock flag index.  
	Rings of size 1 are automatically ignored.  
 */
void
LVS_exclhi::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	typedef	node_args_type::const_iterator		const_iterator;
	typedef	visitor_type::state_type::ring_set_type	ring_set_type;
	ring_set_type temp;
	const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		typedef	const_iterator::value_type::const_iterator
					index_iterator;
		index_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			const node_index_type ni =
				v.lookup_global_bool_id(*ii);
			temp.insert(ni);
		}
	}
	if (temp.size() > 1) {
		v.state.append_check_exclhi_ring(temp);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(LVS_excllo, "excllo")

/**
	Allocates a lock flag for checking exclusiveness.  
	First collect all nodes into a set (for uniqueness) of node indices,
	then registers each node with lock flag index.  
	Rings of size 1 are automatically ignored.  
 */
void
LVS_excllo::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	typedef	node_args_type::const_iterator		const_iterator;
	typedef	visitor_type::state_type::ring_set_type	ring_set_type;
	ring_set_type temp;
	const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		typedef	const_iterator::value_type::const_iterator
					index_iterator;
		index_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			const node_index_type ni =
				v.lookup_global_bool_id(*ii);
			temp.insert(ni);
		}
	}
	if (temp.size() > 1) {
		v.state.append_check_excllo_ring(temp);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(SIM_force_exclhi, "mk_exclhi")

/**
	Implementation ripped off of original prsim's parse_excl function.  
	Just appends each node into excl ring list and sets exclhi flag.  
	\param params unused.  
	TODO: is there any way of checking uniqueness of nodes?
		only if State's excl_ring_map_type is a map/set.  
 */
void
SIM_force_exclhi::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
#if PRSIM_SIMPLE_EVENT_QUEUE
	// remove this comment when complete
	cerr <<
"WARNING: force exclhi is incomplete on the prsim-engine-rewrite branch!"
	<< endl;
#endif
	typedef	node_args_type::const_iterator		const_iterator;
	typedef	visitor_type::state_type::ring_set_type	ring_set_type;
	const_iterator i(nodes.begin()), e(nodes.end());
	ring_set_type r;
#if 1
	for ( ; i!=e; ++i) {
		typedef	const_iterator::value_type::const_iterator
					index_iterator;
		index_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			const node_index_type ni =
				v.lookup_global_bool_id(*ii);
			r.insert(ni);
		}
	}
#else
	for ( ; i!=e; ++i) {
		INVARIANT(i->size() == 1);
		const node_index_type ni =
			v.lookup_global_bool_id(*i->begin());
		r.insert(ni);
	}
#endif
	INVARIANT(r.size() > 1);
	v.state.append_mk_exclhi_ring(r);
	INVARIANT(r.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(SIM_force_excllo, "mk_excllo")

/**
	Implementation ripped off of original prsim's parse_excl function.  
	Just appends each node into excl ring list and sets excllo flag.  
	\param params unused.  
	TODO: is there any way of checking uniqueness of nodes?
		only if State's excl_ring_map_type is a map/set.  
 */
void
SIM_force_excllo::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
#if PRSIM_SIMPLE_EVENT_QUEUE
	// remove this comment when complete
	cerr <<
"WARNING: force excllo is incomplete on the prsim-engine-rewrite branch!"
	<< endl;
#endif
	typedef	node_args_type::const_iterator		const_iterator;
	typedef	visitor_type::state_type::ring_set_type	ring_set_type;
	ring_set_type r;
	const_iterator i(nodes.begin()), e(nodes.end());
#if 1
	for ( ; i!=e; ++i) {
		typedef	const_iterator::value_type::const_iterator
					index_iterator;
		index_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			const node_index_type ni =
				v.lookup_global_bool_id(*ii);
			r.insert(ni);
		}
	}
#else
	for ( ; i!=e; ++i) {
		INVARIANT(i->size() == 1);
		const node_index_type ni =
			v.lookup_global_bool_id(*i->begin());
		r.insert(ni);
	}
#endif
	INVARIANT(r.size() > 1);
	v.state.append_mk_excllo_ring(r);
	INVARIANT(r.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO:
// layout_min_sep -- reserved for SEU prsim... later
// layout_min_sep_proc -- reserved for SEU prsim... later

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(supply_x, "supply_x")

void
supply_x::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	// ignore
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS(RunModeStatic, "runmodestatic")

void
RunModeStatic::main(visitor_type& v, const param_args_type& params, 
		const node_args_type& nodes) {
	// ignore
}

#undef	DECLARE_AND_DEFINE_PRSIM_SPEC_DIRECTIVE_CLASS
}	// end namespace prsim_spec_directives

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

