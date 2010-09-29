/**
	\file "sim/prsim/process_graph.cc"
	Implementation of process graph structure for prsim rules.
	$Id: process_graph.cc,v 1.9 2010/09/29 00:13:43 fang Exp $
	Most of this file was ripped from "sim/prsim/State-prsim.cc"
	for the sake of cleanup.  
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_FANOUT			(0 && ENABLE_STACKTRACE)
#define	DEBUG_CHECK			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <numeric>			// for accumulate
#include <functional>
#include "sim/prsim/State-prsim.h"
#include "sim/prsim/util.tcc"
#include "sim/prsim/Rule.tcc"
#include "sim/ISE.h"
#include "Object/def/footprint.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC.h"
#include "util/stacktrace.h"
#include "util/compose.h"

#if	DEBUG_CHECK
#define	DEBUG_CHECK_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_CHECK	STACKTRACE_VERBOSE
#else
#define	DEBUG_CHECK_PRINT(x)
#define	STACKTRACE_VERBOSE_CHECK
#endif

#if	DEBUG_FANOUT
#define	DEBUG_FANOUT_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#else
#define	DEBUG_FANOUT_PRINT(x)
#endif

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
USING_UTIL_COMPOSE

//=============================================================================
// class unique_process_subgraph method definitions
unique_process_subgraph::unique_process_subgraph() :
		expr_pool(), expr_graph_node_pool(),
		rule_pool(), rule_map(), invariant_map(), 
		local_faninout_map() {
	// local types are allowed to start at 0 index
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unique_process_subgraph::~unique_process_subgraph() { }

//=============================================================================
/**
	Wipes out the indexed node, to mark as deallocated and free.  
	Such nodes are skipped during dump.  
	Only called by ExprAlloc.  
 */
void
unique_process_subgraph::void_expr(const expr_index_type ei)
{
	STACKTRACE_VERBOSE;
	expr_pool[ei].wipe();
	expr_graph_node_pool[ei].wipe();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
unique_process_subgraph::check_node(const node_index_type i) const {
	STACKTRACE_VERBOSE_CHECK;
	INVARIANT(i < local_faninout_map.size());
	const node_type& n(local_faninout_map[i]);
	// check pull-up/dn if applicable
#if PRSIM_WEAK_RULES
size_t k = 0;	// NORMAL_RULE = 0, WEAK_RULE = 1 (Node.h)
for ( ; k<2; ++k) {
#endif
	const fanin_array_type& fu(n.pull_up STR_INDEX(k));
	fanin_array_type::const_iterator fui(fu.begin()), fue(fu.end());
	for ( ; fui!=fue; ++fui) {
	const expr_index_type upi = *fui;
	if (is_valid_expr_index(upi)) {
		const expr_struct_type& e
			__ATTRIBUTE_UNUSED_CTOR__((expr_pool[upi]));
		assert(e.is_root());
		const rule_type& r(*lookup_rule(upi));
		assert(r.direction());
		assert(e.parent == i);
	}
	}
	const fanin_array_type& fd(n.pull_dn STR_INDEX(k));
	fanin_array_type::const_iterator fdi(fd.begin()), fde(fd.end());
	for ( ; fdi!=fde; ++fdi) {
	const expr_index_type dni = *fdi;
	if (is_valid_expr_index(dni)) {
		const expr_struct_type& e
			__ATTRIBUTE_UNUSED_CTOR__((expr_pool[dni]));
		const rule_type& r(*lookup_rule(dni));
		assert(e.is_root());
		assert(!r.direction());
		assert(e.parent == i);
	}
	}
#if PRSIM_WEAK_RULES
}
#endif
	// check fanout
	const size_t fs = n.fanout.size();
	size_t j = 0;
	for ( ; j < fs; ++j) {
		assert(expr_graph_node_pool[n.fanout[j]]
			.contains_node_fanin(i));
	}
}	// end method check_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Double-checks parent-child relationships.  
	\param i index of the expression, must be < expr_pool.size().  
 */
void
unique_process_subgraph::check_expr(const expr_index_type i) const {
	STACKTRACE_VERBOSE_CHECK;
	const faninout_map_type& node_pool(local_faninout_map);
	const expr_struct_type& e(expr_pool[i]);
	const rule_type* const r(lookup_rule(i));
	const bool is_invariant = r && r->is_invariant();
	const graph_node_type& g(expr_graph_node_pool[i]);
if (!e.wiped()) {
	// check parent
	// local indices are allowed to start at 0
	if (e.is_root()) {
	if (!is_invariant) {
		assert(e.parent < node_pool.size());
		const node_type& n
			__ATTRIBUTE_UNUSED_CTOR__((node_pool[e.parent]));
		const bool dir = r->direction();
#if PRSIM_WEAK_RULES
		const fanin_array_type&
			fin(n.get_pull_expr(dir, NORMAL_RULE));
		const fanin_array_type&
			wfin(n.get_pull_expr(dir, WEAK_RULE));
		// the following check is a linear search
		// can use binary search if sorted
		assert((count(fin.begin(), fin.end(), i) == 1) || 
			(count(wfin.begin(), wfin.end(), i) == 1));
#else
		const fanin_array_type& fin(n.get_pull_expr(e.direction()));
		assert(count(fin.begin(), fin.end(), i) == 1);
#endif	// PRSIM_WEAK_RULES
	}
	} else {
		assert(e.parent < expr_pool.size());
		// const Expr& pe(expr_pool[e.parent]);
		const graph_node_type& pg(expr_graph_node_pool[e.parent]);
		const graph_node_type::child_entry_type&
			pc __ATTRIBUTE_UNUSED_CTOR__((pg.children[g.offset]));
		assert(!pc.first);	// this is an expression, not node
		assert(pc.second == i);
	}
	// check children
	assert(e.size == g.children.size());
	size_t j = 0;
	for ( ; j<e.size; ++j) {
		const graph_node_type::child_entry_type& c(g.children[j]);
		// local indices are allowed to start at 0
		if (c.first) {		// points to leaf node
			assert(c.second < node_pool.size());
			assert(node_pool[c.second].contains_fanout(i));
		} else {		// points to expression
			assert(c.second < expr_pool.size());
			assert(expr_pool[c.second].parent == i);
			assert(expr_graph_node_pool[c.second].offset == j);
		}
	}
}	// else skip wiped node
}	// end method check_expr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
faninout_struct_type::has_fanin(void) const {
	return pull_up STR_INDEX(NORMAL_RULE).size() ||
		pull_dn STR_INDEX(NORMAL_RULE).size()
#if PRSIM_WEAK_RULES
		|| pull_up STR_INDEX(WEAK_RULE).size()
		|| pull_dn STR_INDEX(WEAK_RULE).size()
#endif
		;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
faninout_struct_type::contains_fanout(
		const expr_index_type ei) const {
	STACKTRACE_VERBOSE;
	return find(fanout.begin(), fanout.end(), ei) != fanout.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
faninout_struct_type::dump_faninout_list(
		ostream& o, const fanin_array_type& a) {
if (a.size()) {
	std::ostream_iterator<expr_index_type> osi(o, " ");
	copy(a.begin(), a.end(), osi);
} else {
	o << "- ";
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped from Node::dump_struct.
 */
ostream&
faninout_struct_type::dump_struct(ostream& o) const {
	o << "up: ";
	dump_faninout_list(o, pull_up STR_INDEX(NORMAL_RULE));
#if PRSIM_WEAK_RULES
	o << "< ";
	dump_faninout_list(o, pull_up STR_INDEX(WEAK_RULE));
#endif
	o << ", dn: ";
	dump_faninout_list(o, pull_dn STR_INDEX(NORMAL_RULE));
#if PRSIM_WEAK_RULES
	o << "< ";
	dump_faninout_list(o, pull_dn STR_INDEX(WEAK_RULE));
#endif
	o << " fanout: ";
//	o << '<' << fanout.size() << "> ";
	std::ostream_iterator<expr_index_type> osi(o, " ");
	std::copy(fanout.begin(), fanout.end(), osi);
	// std::copy(&fanout[0], &fanout[fanout.size()], osi);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ei is a 0-indexed local expression index.
 */
bool
unique_process_subgraph::is_rule_expr(const expr_index_type ei) const {
	STACKTRACE_VERBOSE;
	return rule_map.find(ei) != rule_map.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ei local 0-indexed expression index
 */
const unique_process_subgraph::rule_type*
unique_process_subgraph::lookup_rule(const expr_index_type ei) const {
	STACKTRACE_VERBOSE;
	typedef	rule_map_type::const_iterator	rule_map_iterator;
	const rule_map_iterator i(rule_map.find(ei));
	if (i != rule_map.end()) {
		return &rule_pool[i->second];
	} else	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
unique_process_subgraph::check_structure(void) const {
	STACKTRACE_VERBOSE_CHECK;
{
	const expr_index_type exprs = expr_pool.size();
	ISE_INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_LOCAL_EXPR;
	for ( ; i<exprs; ++i) {
		DEBUG_CHECK_PRINT("checking Expr " << i << ":" << endl);
		check_expr(i);
	}
}{
	node_index_type i = FIRST_VALID_LOCAL_NODE;
	const node_index_type nodes = local_faninout_map.size();
	for ( ; i<nodes; ++i) {
		DEBUG_CHECK_PRINT("checking Node " << i << ":" << endl);
		check_node(i);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints invariant message if any.
	\param ri root expression index.
 */
ostream&
unique_process_subgraph::dump_invariant_message(ostream& o, 
		const expr_index_type ri, 
		const char* pre, const char* post) const {
#if INVARIANT_BACK_REFS
	// also print optional message string
	// needs to use pg (unique_process_graph)
	const entity::PRS::footprint&
		pfp(_footprint->get_prs_footprint());
#if 0
	const entity::PRS::footprint::invariant_source_ptr_type
		ivs(pfp.lookup_invariant_source(ri));
#endif
	const never_ptr<const entity::SPEC::invariant>
		ivs(pfp.get_invariant_pool()[invariant_map.find(ri)
			->second].second);
	NEVER_NULL(ivs);
	const string& invstr(ivs->get_message());
	if (invstr.length()) {
		o << pre << invstr << post;
	}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
unique_process_subgraph::dump_struct(ostream& o) const {
{
{
	// Technically, top-level should omit reserved local node 0...
	o << "Local nodes: " << endl;
	node_index_type i = FIRST_VALID_LOCAL_NODE;
	for ( ; i<local_faninout_map.size(); ++i) {
		o << "node[" << i << "]: ";
		// TODO: print process-local name
		local_faninout_map[i].dump_struct(o) << endl;
	}
}
	o << "Local expressions: " << endl;
	const expr_index_type exprs = expr_pool.size();
	ISE_INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_LOCAL_EXPR;
	// is 0 valid? process-local?
	for ( ; i<exprs; ++i) {
		const expr_struct_type&
			e(expr_pool[i]);
	if (!e.wiped()) {
		e.dump_struct(o << "expr[" << i << "]: ",
			(e.is_root() ? lookup_rule(i)->direction() : false)
			) << endl;
		expr_graph_node_pool[i].dump_struct(o << '\t') << endl;
	}
	}
{
	o << "Local expression -> rule map:" << endl;	// hash_map is unsorted!
	dump_pair_map(o, rule_map);
	o << "Local rules:" << endl;
	rule_pool_type::const_iterator
		ri(rule_pool.begin()), re(rule_pool.end());
	rule_index_type j = 0;
	for ( ; ri!=re; ++ri, ++j) {
		ri->dump(o << '[' << j << "]\t") << endl;
	}
}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traverses a unique process subgraph to print rules that are owned
	by that process.
	\param offset expression index offset
	Q: need parameter to translate local node to global node?
	A: No, let the global node labels appear in the top-level super-graph.  
 */
ostream&
unique_process_subgraph::dump_struct_dot(ostream& o, 
		const expr_index_type offset) const {
	STACKTRACE_VERBOSE;
{
	o << "# Expressions: " << endl;
	const expr_index_type exprs = expr_pool.size();
	ISE_INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_LOCAL_EXPR;
	for ( ; i<exprs; ++i) {
		const expr_index_type gi = i +offset;
		o << "EXPR_" << gi << "\t[label=\"" << gi << "\", shape=";
		const expr_struct_type& e(expr_pool[i]);
		e.dump_type_dot_shape(o) << "];" << endl;
		e.dump_parent_dot_edge(o << "EXPR_" << gi << " -> ",
			(e.is_root() ? lookup_rule(i)->direction() : false)
			) << ';'<< endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param index of a local expression
	\return index of corresponding root expression, closest to node
 */
node_index_type
unique_process_subgraph::local_root_expr(expr_index_type ei) const {
	STACKTRACE_VERBOSE;
	const expr_struct_type* e = &expr_pool[ei];
	while (!e->is_root()) {
		DEBUG_FANOUT_PRINT("ei = " << ei << endl);
		ei = e->parent;
		e = &expr_pool[ei];
	}
	return ei;	// e is root means that ei is a *node* index
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the aggregate number of fanins and fanouts.
 */
expr_index_type
unique_process_subgraph::fan_count(void) const {
	STACKTRACE_VERBOSE;
	return std::accumulate(local_faninout_map.begin(), 
		local_faninout_map.end(), expr_index_type(0), 
		&faninout_struct_type::add_size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true for every node that is driven *locally*, 
		does not account for fanin from subprocesses.  
 */
void
unique_process_subgraph::has_local_fanin_map(vector<bool>& ret) const {
	ret.reserve(local_faninout_map.size());
	transform(local_faninout_map.begin(), local_faninout_map.end(), 
		back_inserter(ret), 
		std::mem_fun_ref(&faninout_struct_type::has_fanin));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true for every node that is not driven *locally*, 
		does not account for fanin from subprocesses.  
 */
void
unique_process_subgraph::has_not_local_fanin_map(vector<bool>& ret) const {
	ret.reserve(local_faninout_map.size());
	transform(local_faninout_map.begin(), local_faninout_map.end(), 
		back_inserter(ret), 
		unary_compose(std::logical_not<bool>(),
			std::mem_fun_ref(&faninout_struct_type::has_fanin)));
}

//=============================================================================
// explicit class template instantiations
template class Rule<State::time_type>;
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

