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
#include <sstream>
#include "sim/prsim/State-prsim.hh"
#include "sim/prsim/util.tcc"
#include "sim/prsim/Rule.tcc"
#include "sim/ISE.hh"
#include "Object/def/footprint.hh"

#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/common/dump_flags.hh"

#include "Object/lang/PRS_footprint.hh"
#include "Object/lang/SPEC.hh"
#include "util/stacktrace.hh"
#include "util/compose.hh"

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
#include "util/using_ostream.hh"
USING_UTIL_COMPOSE
using entity::bool_tag;

//=============================================================================
// utility functions
/**
	Print footprint-local name of node.
	Just a convenience function.
	\param lni local node index, 1-based.
 */
static
ostream&
__dump_node_local_name(ostream& o, 
		const entity::state_instance<bool_tag>::pool_type& bp, 
		const node_index_type lni) {
	// default dump flags
	static const entity::dump_flags&
		df(dump_flags::no_definition_owner_no_ns);
	INVARIANT(lni);
	// convert 1-based to 0-based
	bp[lni -1].get_back_ref()->dump_hierarchical_name(o, df);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
min_delay_entry::dump_raw(ostream& o) const {
	return o << "{{{" << time[0][0] << ',' << time[0][1] << "},{" <<
		time[1][0] << ',' << time[1][1] << "}},{{" <<
		predicate[0][0] << ',' << predicate[0][1] << "},{" <<
		predicate[1][0] << ',' << predicate[1][1] << "}}}";
}

//=============================================================================
// class unique_process_subgraph method definitions
unique_process_subgraph::unique_process_subgraph(const entity::footprint* f) :
		_footprint(f), 
		expr_pool(), expr_graph_node_pool(),
		rule_pool(), rule_map(), invariant_map(), 
		local_faninout_map()
#if PRSIM_SETUP_HOLD
		, setup_constraints(), hold_constraints()
#endif
		{
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
	if (e.size != g.children.size()) {
		cerr << "Error: Inconsistency in process graph expr "
			<< i << endl;
		assert(e.size == g.children.size());
	}
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
	if (is_atomic) {
		o << " [atomic]";	// this node updates atomically
	}
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
/**
	FIXME: O(n) search for reverse map.
	TODO: replace hash_map with vector<pair>
	and use the fact that rule_map is maintained with monotonic
	keys and values, so a reverse lookup is just a matter of binary_search.
	\param ri a valid rule index
	\return guard expression index corresponding to rule
 */
expr_index_type
unique_process_subgraph::lookup_expr(const rule_index_type ri) const {
	typedef	rule_map_type::const_iterator	rule_map_iterator;
	rule_map_iterator i(rule_map.begin()), e(rule_map.end());
	for ( ; i!=e; ++i) {
		if (i->second == ri) {
			return i->first;
		}
	}
	// should be unreachable
	DIE;
	return 0;
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
	Traces up the expression tree until a root is reached.
	The root expression is guaranteed to map to a rule index.
 */
rule_index_type
unique_process_subgraph::follow_expr_to_root(const expr_index_type _ei) const {
	expr_index_type ei = _ei;
	const expr_struct_type* ex = &expr_pool[ei];
	// follow expressions' parents up to root
	// see State::evaluate_kernel for similar impl
	while (!ex->is_root()) {
		ei = ex->parent;
		ex = &expr_pool[ei];
	}
	// map root expr to rule index
	const rule_map_type::const_iterator f(rule_map.find(ei));
	INVARIANT(f != rule_map.end());
	return f->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param lni match rules that drive this node (1-based).
	\param dir pull-up (1) or pull-dn (0)
	\return list of rule indices that match.
	Weak rules are ignored.
 */
void
unique_process_subgraph::rules_matching_fanin(const node_index_type lni,
	const bool dir, vector<rule_index_type>& ret) const {
	STACKTRACE_BRIEF;
	// see process_sim_state::dump_node_fanin
	INVARIANT(lni-1 < local_faninout_map.size());
	const faninout_struct_type& fia(local_faninout_map[lni -1]);
	const size_t w = NORMAL_RULE;
//	fia.dump_struct(cerr);
	const fanout_array_type&
		fo(dir ? fia.pull_up STR_INDEX(w) : fia.pull_dn STR_INDEX(w));
	// these expr_indices need to map to root rule indices
	// unique-sort
	set<rule_index_type> r;
	fanout_array_type::const_iterator i(fo.begin()), e(fo.end());
	for ( ; i!=e; ++i) {
		r.insert(follow_expr_to_root(*i));
	}
	ret.clear();
	copy(r.begin(), r.end(), back_inserter(ret));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param lni match rules that this node drives (1-based).
	\return list of rule indices that match.
	Weak rules are ignored.
 */
void
unique_process_subgraph::rules_matching_fanout(const node_index_type lni,
	vector<rule_index_type>& ret) const {
	STACKTRACE_BRIEF;
	INVARIANT(lni-1 < local_faninout_map.size());
	const fanout_array_type& foa(local_faninout_map[lni -1].fanout);
	// expr_indices need to be translated and unique-sorted to rule indices
	set<rule_index_type> s;
	fanout_array_type::const_iterator i(foa.begin()), e(foa.end());
	for ( ; i!=e; ++i) {
		s.insert(follow_expr_to_root(*i));
	}
	ret.clear();
	copy(s.begin(), s.end(), back_inserter(ret));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Match rules with lns in guard, and fan out to lnd, in direction ddir.
 */
void
unique_process_subgraph::rules_matching_faninout(const node_index_type lns,
	const node_index_type lnd, const bool ddir,
	vector<rule_index_type>& ret) const {
	STACKTRACE_BRIEF;
	vector<rule_index_type> tmp1, tmp2;
	rules_matching_fanin(lnd, ddir, tmp1);
	rules_matching_fanout(lns, tmp2);
	ret.clear();
	set_intersection(tmp1.begin(), tmp1.end(), tmp2.begin(), tmp2.end(), 
		back_inserter(ret));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
unique_process_subgraph::print_rules_matching_faninout(ostream& o,
		const node_index_type lns,
		const node_index_type lnd, const bool ddir) const {
	STACKTRACE_BRIEF;
	vector<rule_index_type> r;
	rules_matching_faninout(lns, lnd, ddir, r);
	dump_rules(o, r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param r list of rule indices to rules to apply change.
	\param prop name of property
	\param val new value of property to set
	\return true on error
 */
bool
unique_process_subgraph::edit_rule_property(const vector<rule_index_type>& r,
		const string& prop, const string& val) {
	vector<rule_index_type>::const_iterator
		ri(r.begin()), re(r.end());
	for ( ; ri!=re; ++ri) {
		if (rule_pool[*ri].edit_property(prop, val))
			return true;
	}
	return false;
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
	invariant_map_type::const_iterator
		f(invariant_map.find(ri));
	INVARIANT(f != invariant_map.end());
	const size_t sz = pfp.get_invariant_pool().size();
	if (f->second < sz) {
	const never_ptr<const entity::SPEC::invariant>
		ivs(pfp.get_invariant_pool()[f->second].second);
	NEVER_NULL(ivs);
	const string& invstr(ivs->get_message());
	if (invstr.length()) {
		o << pre << invstr << post;
	}
	} else {
		// NEW with automatic precharge invariants!
		o << pre << "[auto] precharge interference" << post;
		// TODO: another type will be "precharge sneak-path"
		// need some way to distinguish...
	}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped from process_state::dump_subexpr().
	Recursive expression printer.  
	Should be modeled after cflat's expression printer.  
	\param ei is the *LOCAL* expression index within the owning process.  
	\param ptype the parent's expression type, only used if pr is true.
	\param pr whether or not parent is root
		(if so, ignore type comparison for parenthesization).  
 */
ostream&
unique_process_subgraph::dump_subexpr(ostream& o, const expr_index_type ei, 
		const uchar ptype, const bool pr) const {
	ISE_INVARIANT(ei < expr_pool.size());
	const expr_struct_type& e(expr_pool[ei]);
	const graph_node_type& g(expr_graph_node_pool[ei]);
	// can elaborate more on when parens are needed
	const bool need_parens = e.parenthesize(ptype, pr);
	const uchar _type = e.type;
	// rule attribute printing has moved! (was here)
	if (e.is_not()) {
		o << '~';
	}
	const char* op = e.is_disjunctive() ? " | " : " & ";
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	if (need_parens) {
		o << '(';
	}
	// peel out first iteration for infix printing
	if (ci->first) {
		// node is 0-indexed, convert to 1-based
		dump_node_local_name(o, ci->second +1);
	} else {
		dump_subexpr(o, ci->second, _type);
	}
	if (g.children.size() >= 1) {
	for (++ci; ci!=ce; ++ci) {
		o << op;
		if (ci->first) {
			// node is 0-indexed, convert to 1-based
			dump_node_local_name(o, ci->second +1);
		} else {
			if (e.is_or() && is_rule_expr(ci->second)) {
				// to place each 'rule' on its own line
				o << endl;
			}
			dump_subexpr(o, ci->second, _type);
		}
	}
	}
	if (need_parens) {
		o << ')';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped from process_state::dump_rule().
	\param lri local rule index, also doubles as an expr index.
 */
ostream&
unique_process_subgraph::dump_rule(ostream& o,
		const rule_index_type lri) const {
	const rule_type& r(rule_pool[lri]);
	const expr_index_type ei = lookup_expr(lri);
	const expr_struct_type& e(expr_pool[ei]);
	ISE_INVARIANT(e.is_root());
	const bool dir = r.direction();
	const node_index_type nr = e.parent;
	const faninout_struct_type& n(local_faninout_map[nr]);
if (n.is_atomic) {
	// node is 0-indexed, convert to 1-based
	dump_node_local_name(o << '\t', nr +1);
	o << " = ";
} else {
	r.dump(o << '[') << "]\t";	// moved here from dump_subexpr
}
	dump_subexpr(o, ei, expr_struct_type::EXPR_ROOT, true);
if (!n.is_atomic) {
	// node is 0-indexed, convert to 1-based
	dump_node_local_name(o << " -> ", nr +1) << (dir ? '+' : '-');
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
unique_process_subgraph::dump_rules(ostream& o,
		const vector<rule_index_type>& r) const {
	vector<rule_index_type>::const_iterator i(r.begin()), e(r.end());
	for ( ; i!=e; ++i) {
		dump_rule(o, *i) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
unique_process_subgraph::dump_all_rules(ostream& o) const {
	size_t i = 0;
	for ( ; i < rule_pool.size(); ++i) {
		dump_rule(o, i) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
unique_process_subgraph::dump_struct(ostream& o) const {
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
		const expr_struct_type& e(expr_pool[i]);
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
		const expr_index_type offset, 
		const footprint_frame_map_type& bfm) const {
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
			bfm, offset,
			(e.is_root() ? lookup_rule(i)->direction() : false)
			) << ';'<< endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SETUP_HOLD
// TODO: print local node names, not just IDs
// see also process_sim_state::dump_timing_constraints
ostream&
unique_process_subgraph::dump_timing_constraints(ostream& o) const {
{
	o << "# Setup constraints:" << endl;
	setup_constraint_set_type::const_iterator
		i(setup_constraints.begin()), e(setup_constraints.end());
	for ( ; i!=e; ++i) {
		// node indices are local
#if PRSIM_FWD_POST_TIMING_CHECKS
		o << i->first << " ->" << endl;
#else
		o << "-> " << i->first.first <<
			(i->first.second ? '+' : '-') << endl;
#endif
		vector<setup_constraint_entry>::const_iterator
			ci(i->second.begin()), ce(i->second.end());
		for ( ; ci!=ce; ++ci) {
			o << '\t' <<
#if PRSIM_FWD_POST_TIMING_CHECKS
				ci->trig_node << (ci->dir ? '+' : '-')
#else
				ci->ref_node
#endif
				<< ": " << ci->time << endl;	
		}
	}
}{
	o << "# Hold constraints:" << endl;
	hold_constraint_set_type::const_iterator
		i(hold_constraints.begin()), e(hold_constraints.end());
	for ( ; i!=e; ++i) {
		// node indices are local
#if PRSIM_FWD_POST_TIMING_CHECKS
		o << i->first.first << (i->first.second ? '+' : '-')
			<< " ->" << endl;
#else
		o << "-> " << i->first << endl;
#endif
		vector<hold_constraint_entry>::const_iterator
			ci(i->second.begin()), ce(i->second.end());
		for ( ; ci!=ce; ++ci) {
			o << '\t' <<
#if PRSIM_FWD_POST_TIMING_CHECKS
				ci->trig_node
#else
				ci->ref_node << (ci->dir ? '+' : '-')
#endif
				<< ": " << ci->time << endl;	
		}
	}
}
	return o;
}
#endif	// PRSIM_SETUP_HOLD

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print a local node name.
	\param lni local node index, 1-based.
 */
ostream&
unique_process_subgraph::dump_node_local_name(ostream& o,
		const node_index_type lni) const {
	NEVER_NULL(_footprint);
//	o << "(N" << lni << ')';	// debug only
	const entity::state_instance<bool_tag>::pool_type&
		bp(_footprint->get_instance_pool<bool_tag>());
	return __dump_node_local_name(o, bp, lni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_TIMING_BACKANNOTATE
ostream&
unique_process_subgraph::print_backannotated_delay_single(ostream& o,
	const min_delay_set_type::value_type& mds) const {
	std::ostringstream oss;
	const node_index_type tgt = mds.first;
	INVARIANT(tgt);
	dump_node_local_name(oss, tgt);
	min_delay_ref_set_type::const_iterator
		si(mds.second.begin()), se(mds.second.end());
	for ( ; si!=se; ++si) {
		const node_index_type local_ref_node = si->first;
		const min_delay_entry& m(si->second);
		// TODO: this printing is very verbose, often resulting
		// in quadruplicate entries printed.
		// Should find a more compact way to print these
		// in the common case where they are all equal.
		// source -> destination
	if (m.is_uniform()) {
		// fields are identical in all direction cases
		o << "\tt( ";
		dump_node_local_name(o, local_ref_node) << '*' <<
			" -> " << oss.str() << '*' <<
			" ) >= " << m.time[0][0];
		if (m.predicate[0][0]) {
			o << ", if (";
			dump_node_local_name(o, m.predicate[0][0]) << ')';
		}
		o << endl;
	} else {
		bool rd = false;
		do {
		bool td = false;
		do {
		if (m.time[rd][td] > 0) {
			o << "\tt( ";
			dump_node_local_name(o, local_ref_node) <<
				(rd ? '+' : '-') <<
				" -> " << oss.str() <<
				(td ? '+' : '-') <<
				" ) >= " << m.time[rd][td];
			if (m.predicate[rd][td]) {
				o << ", if (";
				dump_node_local_name(o, m.predicate[rd][td]) << ')';
			}
			o << endl;
		}	// else don't print, as delta of 0 is no-op
		td = !td;
		} while (td);
		rd = !rd;
		} while (rd);
	}
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
unique_process_subgraph::dump_backannotated_delays(ostream& o) const {
	NEVER_NULL(_footprint);
	_footprint->dump_type(
		o << "Min-delay bounds in type \'") << "\':" << endl;
	min_delay_set_type::const_iterator
		mdi(min_delays.begin()), mde(min_delays.end());
	// print using type-local node names
	for ( ; mdi != mde; ++mdi) {
		print_backannotated_delay_single(o, *mdi);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
unique_process_subgraph::dump_backannotated_delays_targeting(
		ostream& o, const vector<node_index_type>& t) const {
	NEVER_NULL(_footprint);
	_footprint->dump_type(
		o << "Min-delay bounds in type \'") << "\':" << endl;
	vector<node_index_type>::const_iterator ti(t.begin()), te(t.end());
	// print using type-local node names
	for ( ; ti != te; ++ti) {
		INVARIANT(*ti);
		const min_delay_set_type::const_iterator
			mdi(min_delays.find(*ti));
		print_backannotated_delay_single(o, *mdi);
	}
	return o;
}
#endif

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_TIMING_BACKANNOTATE
/**
	Adds a timing constraint to unique graph, but doesn't
	apply it globally to all instances.  
	\param ref the reference node (earlier)
	\param refdir direction specifications to apply [+-*] to ref node
	\param tgt the target node (later)
	\param tgtdir direction specifications to apply [+-*] to tgt node
	\param del min-delay value to apply
	\param pred optional predicate, 0 to signal no predicate (always)
 */
void
unique_process_subgraph::add_min_delay_constraint(const node_index_type ref,
		const char refdir,
		const node_index_type tgt,
		const char tgtdir,
		const rule_time_type del,
		const node_index_type pred) {
	bool r_apply[2];
	bool t_apply[2];
	r_apply[0] = refdir != '+';
	r_apply[1] = refdir != '-';
	t_apply[0] = tgtdir != '+';
	t_apply[1] = tgtdir != '-';
	min_delay_entry& md(min_delays[tgt][ref]);
	// initialize, if needed
	bool rd = false;
	do {
	if (r_apply[rd]) {
		bool td = false;
		do {
		if (t_apply[td]) {
			md.time[rd][td] = del;
			md.predicate[rd][td] = pred;
		}
			td = !td;
		} while (td);
	}
		rd = !rd;
	} while (rd);
}
#endif

//=============================================================================
// explicit class template instantiations
template class Rule<State::time_type>;
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

