/**
	\file "sim/prsim/process_state.cc"
	Implementation of process graph structure for prsim rules.
	$Id: process_state.cc,v 1.6 2010/09/29 00:13:43 fang Exp $
	Most of this file was ripped from "sim/prsim/State-prsim.cc"
	for the sake of cleanup.  
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_FANOUT			(0 && ENABLE_STACKTRACE)
#define	DEBUG_WHY			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include "sim/prsim/State-prsim.hh"
#include "sim/prsim/Rule.tcc"
#include "sim/ISE.hh"
#include "util/stacktrace.hh"
#include "util/indent.hh"

#if	DEBUG_WHY
#define	DEBUG_WHY_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#define	STACKTRACE_VERBOSE_WHY		STACKTRACE_VERBOSE
#else
#define	DEBUG_WHY_PRINT(x)
#define	STACKTRACE_VERBOSE_WHY
#endif

#if	DEBUG_FANOUT
#define	DEBUG_FANOUT_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#else
#define	DEBUG_FANOUT_PRINT(x)
#endif

namespace HAC {
namespace SIM {
namespace PRSIM {
using util::auto_indent;
using util::indent;
#include "util/using_ostream.hh"

//=============================================================================
// class process_sim_state_base method definitions

/**
	\param gei global expression index (from fanout)
	\return global rule index of the root.
 */
expr_index_type
process_sim_state_base::global_expr_to_root(const expr_index_type gei) const {
	const expr_index_type lei = local_expr_index(gei);
	const unique_process_subgraph& pg(type());
	const expr_index_type ei = global_expr_index(pg.local_root_expr(lei));
	return ei;
}

//=============================================================================
// class process_sim_state method definitions
void
process_sim_state::allocate_from_type(const unique_process_subgraph& t, 
		const process_index_type tid, const expr_index_type ex_off) {
	STACKTRACE_VERBOSE;
	type_ref.index = tid;	// eventually link to pointer
	global_expr_offset = ex_off;
	expr_states.resize(t.expr_pool.size());
	rule_states.resize(t.rule_pool.size());
	// default constructors of these must initalize state values
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_sim_state::clear(void) {
	expr_states.resize(0);
	rule_states.resize(0);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_sim_state::initialize(void) {
	STACKTRACE_VERBOSE;
	const size_t s = expr_states.size();
	STACKTRACE_INDENT_PRINT("expr_state.size = " << s << endl);
	expr_state_type* i(&expr_states[0]), *e(&expr_states[s]);
	STACKTRACE_INDENT_PRINT("type @ " << &type() << endl);
	STACKTRACE_INDENT_PRINT("type.pool_size = " << type().expr_pool.size() << endl);
	unique_process_subgraph::expr_pool_type::const_iterator
		j(type().expr_pool.begin());
	for ( ; i!=e; ++i, ++j) {
		STACKTRACE_INDENT_PRINT("foo..." << endl);
		i->initialize(*j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_sim_state::dumper_base {
	ostream&			os;
	const State&			st;
	const bool			verbose;

	dumper_base(ostream& o, const State& s, const bool v) :
		os(o), st(s), verbose(v) { }

};	// end struct dumper_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_sim_state::rules_dumper : public dumper_base {

	rules_dumper(ostream& o, const State& s, const bool v) :
		dumper_base(o, s, v) { }

	ostream&
	operator () (const process_sim_state& s) {
		// skip empty processes
		const unique_process_subgraph& pg(s.type());
		// TODO: count number of non-invariant rules
	if (pg.rule_pool.size()) {
		st.dump_process_canonical_name(os << "process: ", s) << endl;
		// TODO: print type?
		return s.dump_rules(os, st, verbose);
	} else {
		return os;
	}
	}
};	// end struct rules_dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print all rules belonging to this process.
 */
ostream&
process_sim_state::dump_rules(ostream& o, const State& st, 
		const bool v) const {
	const unique_process_subgraph& pg(type());
if (pg.rule_pool.size()) {
	typedef	unique_process_subgraph::rule_map_type::const_iterator
						const_iterator;
	const_iterator i(pg.rule_map.begin()), e(pg.rule_map.end());
	for ( ; i!=e; ++i) {
		const rule_type& r(pg.rule_pool[i->second]);
	if (!r.is_invariant()) {
#if PRSIM_WEAK_RULES
	if (!r.is_weak() || st.weak_rules_shown()) {
#endif
		// what to assume about multi-fanin?
		dump_rule(o, i->first, st, v, true) << endl;
#if PRSIM_WEAK_RULES
	}
#endif
	}
	}	// end for
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenient accumulator functor.
 */
struct process_sim_state::invariant_checker {
	ostream&			os;
	const State&			st;

	invariant_checker(ostream& o, const State& s) : os(o), st(s) { }

	bool
	operator () (const bool e, const process_sim_state& s) {
		return s.check_invariants(os, st) || e;
	}
};	// end struct invariant_checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there are any errors.
 */
bool
process_sim_state::check_invariants(ostream& o, const State& st) const {
	const unique_process_subgraph& pg(type());
	bool ret = false;
	typedef	unique_process_subgraph::rule_map_type::const_iterator
						const_iterator;
	const_iterator i(pg.rule_map.begin()), e(pg.rule_map.end());
	const string pn(st.get_process_canonical_name(
		st.lookup_process_index(*this)));
	for ( ; i!=e; ++i) {
		const rule_type& r(pg.rule_pool[i->second]);
	if (r.is_invariant()) {
		const expr_index_type lei = i->first;
		switch (expr_states[lei].pull_state(pg.expr_pool[lei])) {
		case PULL_OFF:
			ret |= true;
			o << "Error: invariant violation in " << pn << ": (";
			dump_subexpr(o, lei, st, true);
			pg.dump_invariant_message(o, lei, ", \"", "\"");
			o << ')' << endl;
			break;
		case PULL_WEAK:
			o << "Warning: possible invariant violation in "
				<< pn << ": (";
			dump_subexpr(o, lei, st, true);
			pg.dump_invariant_message(o, lei, ", \"", "\"");
			o << ')' << endl;
			break;
		default: break;
		}
	}
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_sim_state::invariant_dumper : public dumper_base {

	invariant_dumper(ostream& o, const State& s, const bool v) :
		dumper_base(o, s, v) { }

	ostream&
	operator () (const process_sim_state& s) {
		const unique_process_subgraph& pg(s.type());
		// TODO: count number of invariant rules
	if (pg.rule_pool.size()) {
		st.dump_process_canonical_name(os << "process: ", s) << endl;
		return s.dump_invariants(os, st, verbose);
	} else {
		// skip empty processes
		return os;
	}
	}
};	// end struct invariant_checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there are any errors.
 */
ostream&
process_sim_state::dump_invariants(ostream& o, const State& st, 
		const bool v) const {
	const unique_process_subgraph& pg(type());
	typedef	unique_process_subgraph::rule_map_type::const_iterator
						const_iterator;
	const_iterator i(pg.rule_map.begin()), e(pg.rule_map.end());
	for ( ; i!=e; ++i) {
		const rule_type& r(pg.rule_pool[i->second]);
	if (r.is_invariant()) {
		dump_subexpr(o << "$(", i->first, st, v);
		pg.dump_invariant_message(o, i->first, ", \"", "\"")
			<< ')' << endl;
	}
	}
	return o;
}

//=============================================================================
/**
	Collects fanin literals into ret, unsorted, with duplicates.
 */
void
process_sim_state::collect_node_fanin(const node_index_type lni, 
		const State& st, vector<node_index_type>& ret) const {
	const faninout_struct_type& fia(type().local_faninout_map[lni]);
#if PRSIM_WEAK_RULES
	size_t w = NORMAL_RULE;
do {
#endif
	vector<expr_index_type>::const_iterator
		i(fia.pull_up STR_INDEX(w).begin()),
		e(fia.pull_up STR_INDEX(w).end());
	for ( ; i!=e; ++i) {
		const expr_index_type ui = *i;
		__collect_rule_literals(ui, st, ret);
	}
		i = fia.pull_dn STR_INDEX(w).begin();
		e = fia.pull_dn STR_INDEX(w).end();
	for ( ; i!=e; ++i) {
		const expr_index_type di = *i;
		__collect_rule_literals(di, st, ret);
	}
#if PRSIM_WEAK_RULES
	++w;		// always consider weak rules for feedback
} while (w<2);		// even if !weak_rules_enabled()
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Rajit's prsim suppresses weak rule fanins (copy?)
		For now, we print those as well.
	\param v true if literal should be printed with its value.  
		also print expression with pull-state.
 */
ostream&
process_sim_state::dump_node_fanin(ostream& o, const node_index_type lni, 
		const State& st, const bool v) const {
	const node_index_type ni = st.translate_to_global_node(*this, lni);
	const State::node_type& n(st.get_node(ni));
//	const string cn(st.get_node_canonical_name(ni));
	const faninout_struct_type& fia(type().local_faninout_map[lni]);
#if PRSIM_WEAK_RULES
	size_t w = NORMAL_RULE;
do {
#endif
	vector<expr_index_type>::const_iterator
		i(fia.pull_up STR_INDEX(w).begin()),
		e(fia.pull_up STR_INDEX(w).end());
	for ( ; i!=e; ++i) {
		const expr_index_type ui = *i;
		dump_rule(o, ui, st, v, (n.fanin.size() > 1)) << endl;
	}
		i = fia.pull_dn STR_INDEX(w).begin();
		e = fia.pull_dn STR_INDEX(w).end();
	for ( ; i!=e; ++i) {
		const expr_index_type di = *i;
		dump_rule(o, di, st, v, (n.fanin.size() < 1)) << endl;
	}
#if PRSIM_WEAK_RULES
	if (st.weak_rules_shown()) {
		++w;
	} else {
		break;
	}
} while (w<2);		// even if !weak_rules_enabled()
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints a single rule: expr -> node+/-
	Unlike the old implementation, this only prints rules in which
	the literal appears, other OR-combination fanins will not be shown.
	\param lri local expression index, also must be a valid rule index!
	\param ps current state of process instance
	\param st state of entire simulator (for node lookup)
	\param v verbosity level
	\param root_pull for verbose mode, print overall pull value on node.
	\param multi_fi is true if the affected node is driven by more than
		one process.  
	May need to lookup footprint for node translation.
 */
ostream&
process_sim_state::dump_rule(ostream& o, const rule_index_type lri, 
		const State& st, const bool v, 
		const bool multi_fi) const {
	const unique_process_subgraph& pg(type());
	const rule_type* const r = pg.lookup_rule(lri);
	NEVER_NULL(r);
	const expr_struct_type& e(pg.expr_pool[lri]);
	ISE_INVARIANT(e.is_root());
	const bool dir = r->direction();
	// print overall pull state (OR combined)
	const node_index_type nr = e.parent;
	const node_index_type gnr = st.translate_to_global_node(*this, nr);
	const State::node_type& n(st.get_node(gnr));
	const pull_set root_pull(n, true);	// st.weak_rules_enabled();
	// repetitive waste for fanin...
if (n.is_atomic()) {
	st.dump_node_canonical_name(o << '\t', gnr);
	if (v) {
		st.get_node(gnr).dump_value(o << ':');
	}
	o << " = ";
} else {
	r->dump(o << '[') << "]\t";	// moved here from dump_subexpr
}
	dump_subexpr(o, lri, st, v, expr_struct_type::EXPR_ROOT, true);
		// or pass (!v) to proot to parenthesize in verbose mode
	// atomic nodes only have single-fanin
	if (!n.is_atomic() && v && (multi_fi || 
		(pg.expr_graph_node_pool[lri].children.size() > 1))) {
		const bool w = r->is_weak();
		const pull_enum p = w ?
			(dir ? root_pull.wup : root_pull.wdn) :
			(dir ? root_pull.up : root_pull.dn);
		// rewritten this way because g++-3.3 ICEs-on-valid.
		// was: (dir ? root_pull.up : root_pull.dn) STR_INDEX(w);
		o << '<' << State::node_type::translate_value_to_char(p) << '>';
	}
if (!n.is_atomic()) {
	st.dump_node_canonical_name(o << " -> ", gnr) << (dir ? '+' : '-');
	if (v) {
		st.get_node(gnr).dump_value(o << ':');
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-reduced from process_sim_state::dump_rule().
 */
node_index_type
process_sim_state::rule_fanout(const rule_index_type lri, 
		const State& st) const {
	const unique_process_subgraph& pg(type());
	const rule_type* const r = pg.lookup_rule(lri);
	NEVER_NULL(r);
	const expr_struct_type& e(pg.expr_pool[lri]);
	ISE_INVARIANT(e.is_root());
	const node_index_type nr = e.parent;
	const node_index_type gnr = st.translate_to_global_node(*this, nr);
	return gnr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses local expression state lookup.  
	\param xi local expr index, whose pull state is X.
 */
void
process_sim_state::__get_local_X_fanins(const expr_index_type xi,
	const State& st, node_set_type& u) const {
	const unique_process_subgraph& pg(type());
	ISE_INVARIANT(xi < pg.expr_pool.size());
	const expr_struct_type& x(pg.expr_pool[xi]);
	const expr_state_type& xs(expr_states[xi]);
	ISE_INVARIANT(xs.pull_state(x) == PULL_WEAK);
	const graph_node_type& g(pg.expr_graph_node_pool[xi]);
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	for ( ; ci!=ce; ++ci) {
		INVARIANT(ci->second);
		if (ci->first) {
			// is a leaf node, visit if value is X
			const node_index_type gni =
				st.translate_to_global_node(*this, ci->second);
			if (
				st.get_node(gni).current_value()
					== LOGIC_OTHER) {
				u.insert(gni);
			}
		} else {
			// is a sub-expresion, recurse if pull is X
			const pull_enum p =
				expr_states[ci->second]
					.pull_state(pg.expr_pool[ci->second]);
			if (p == PULL_WEAK) {
				__get_local_X_fanins(ci->second, st, u);
			}
		}
	}	// end for
}	// end __get_X_fanins

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In an expression tree, find nodes that are cutting expressions off.
	now also finds subexpressions that are actively pulling.  
	Xs are not visited.  
	Should follow similar flow to dump_subexpr.
	\param off_on true asks why a node is/not on, false asks ... off
	\param why_not is true if asking why-not? (negative-query), 
		else is asking 'why'? (positive-query)
	\param u anti-cycle stack
	\param v globally visited stack
 */
void
process_sim_state::__local_expr_why_not(ostream& o, 
		const expr_index_type xi, const State& st, 
		const size_t limit, const bool why_not, const bool verbose,
		node_set_type& u, node_set_type& v) const {
	STACKTRACE_VERBOSE_WHY;
	const unique_process_subgraph& pg(type());
	ISE_INVARIANT(xi < pg.expr_pool.size());
	const expr_struct_type& x(pg.expr_pool[xi]);
	const expr_state_type& xs(expr_states[xi]);
	const pull_enum xp(xs.pull_state(x));
	const graph_node_type& g(pg.expr_graph_node_pool[xi]);
#define	STATE_MEM	st.
	const pull_enum match_pull = x.is_not() ?
		expr_state_type::negate_pull(xp) : xp;
	ISE_INVARIANT(xp != PULL_WEAK);
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	string ind_str;
	if (verbose) {
		ind_str += " ";
		if (x.is_not()) ind_str += "~";
		if (g.children.size() > 1) {
			ind_str += x.is_conjunctive() ? "&" : "|";
			o << auto_indent << "-+" << endl;
		}
		// ind_str += " ";
	}
	const indent __ind_ex(o, ind_str);	// INDENT_SCOPE(o);
	for ( ; ci!=ce; ++ci) {
		if (ci->first) {
		const node_index_type gni =
			st.translate_to_global_node(*this, ci->second);
			// is a leaf node, visit if value is not X
			switch (STATE_MEM get_node(gni).current_value()) {
			case LOGIC_LOW:
				STATE_MEM __node_why_not(o, gni, limit, 
					why_not, why_not, verbose, u, v);
			break;
			case LOGIC_HIGH:
				STATE_MEM __node_why_not(o, gni, limit,
					!why_not, why_not, verbose, u, v);
			break;
			default:
				break;
			}
		} else {
			// is a sub-expression, recurse if pull is off
#if 0
			o << auto_indent << "examining expr..." << endl;
			dump_subexpr(o, ci->second, false,
				expr_struct_type::EXPR_ROOT, false) << endl;
#endif
			__recurse_expr_why_not(o, ci->second, match_pull, 
				st, limit, why_not, verbose, u, v);
		}
	}
#undef	STATE_MEM
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience function for reuse.  
 */
void
process_sim_state::__recurse_expr_why_not(ostream& o, 
		const expr_index_type lei, const pull_enum match_pull, 
		const State& st, 
		const size_t limit, const bool why_not, const bool verbose,
		node_set_type& u, node_set_type& v) const {
	STACKTRACE_VERBOSE_WHY;
	const unique_process_subgraph& pg(type());
	const expr_struct_type& s(pg.expr_pool[lei]);
	const expr_state_type& ss(expr_states[lei]);
	const pull_enum sp(ss.pull_state(s));
// maintain same (positive/negative) query type recursively
	if (sp == match_pull) {
		__local_expr_why_not(o, lei, st,
			limit, why_not, verbose, u, v);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In an expression tree, find nodes that are X's that are not cut off
	(by and-0) nor overtaken (or or-1).  
	Ripped from __node_why_not, above.
	\param off_on true asks why a node is/not on, false asks ... off
	\param why_not is true if asking why-not? (negative-query), 
		else is asking 'why'? (positive-query)
	\param u anti-cycle stack
	\param v globally visited stack
 */
void
process_sim_state::__local_expr_why_X(ostream& o, 
		const expr_index_type xi, const State& st, 
		const size_t limit, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	STACKTRACE_VERBOSE_WHY;
	const unique_process_subgraph& pg(type());
	ISE_INVARIANT(xi < pg.expr_pool.size());
	const expr_struct_type& x(pg.expr_pool[xi]);
	const expr_state_type& xs(expr_states[xi]);
	const pull_enum xp(xs.pull_state(x));
	const graph_node_type& g(pg.expr_graph_node_pool[xi]);
#define STATE_MEM	st.
	INVARIANT(xp == PULL_WEAK);
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	string ind_str;
	if (verbose) {
		ind_str += " ";
		if (x.is_not()) ind_str += "~";
		if (g.children.size() > 1) {
			ind_str += x.is_conjunctive() ? "&" : "|";
			o << auto_indent << "-+" << endl;
		}
		// ind_str += " ";
	}
	const indent __ind_ex(o, ind_str);	// INDENT_SCOPE(o);
	for ( ; ci!=ce; ++ci) {
		if (ci->first) {
		const node_index_type gni =
			st.translate_to_global_node(*this, ci->second);
			// is a leaf node, visit if value is X
			if (STATE_MEM get_node(gni).current_value()
				== LOGIC_OTHER) {
				STATE_MEM __node_why_X(o, gni, 
					limit, verbose, u, v);
			}
		} else {
			// is a sub-expression, recurse if pull is X
			__recurse_expr_why_X(o, ci->second, st, 
				limit, verbose, u, v);
		}
	}	// end for
#undef	STATE_MEM
}	// end expr_why_X

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience function to reuse piece of code, kinda messy...
 */
void
process_sim_state::__recurse_expr_why_X(ostream& o, 
		const expr_index_type lei, 
		const State& st, const size_t limit, const bool verbose, 
		node_set_type& u, node_set_type& v) const {
	STACKTRACE_VERBOSE_WHY;
	// is a sub-expression, recurse if pull is X
	const expr_struct_type& s(type().expr_pool[lei]);
	const expr_state_type& ss(expr_states[lei]);
	const pull_enum sp(ss.pull_state(s));
	if (sp == PULL_WEAK) {
		__local_expr_why_X(o, lei, st, limit, verbose, u, v);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive expression printer.  
	Should be modeled after cflat's expression printer.  
	\param ei is the *LOCAL* expression index within the owning process.  
	\param pi is the global process index to which expression belongs.
	\param st is the global node state
	\param v true if literal should be printed with its value.  
	\param ptype the parent's expression type, only used if pr is true.
	\param pr whether or not parent is root
		(if so, ignore type comparison for parenthesization).  
 */
#define	CALL_DUMP_RECURSIVE(a,b,c,d,e)		dump_subexpr(a,b,c,d,e)
ostream&
process_sim_state::dump_subexpr(ostream& o, const expr_index_type ei, 
		const State& st, 	// for node state
		const bool v, 
		const uchar ptype, const bool pr) const {
	const unique_process_subgraph& pg(type());
	ISE_INVARIANT(ei < pg.expr_pool.size());
	const expr_struct_type& e(pg.expr_pool[ei]);
	const expr_state_type& es(expr_states[ei]);
	const graph_node_type& g(pg.expr_graph_node_pool[ei]);
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
	node_index_type gni = 0;
	if (ci->first) {
		gni = st.translate_to_global_node(*this, ci->second);
		st.dump_node_canonical_name(o, gni);
		if (v) {
			st.get_node(gni).dump_value(o << ':');
		}
	} else {
		CALL_DUMP_RECURSIVE(o, ci->second, st, v, _type);
	}
	if (g.children.size() >= 1) {
	for (++ci; ci!=ce; ++ci) {
		o << op;
		if (ci->first) {
			gni = st.translate_to_global_node(*this, ci->second);
			st.dump_node_canonical_name(o, gni);
			if (v) {
				st.get_node(gni).dump_value(o << ':');
			}
		} else {
			if (e.is_or() && pg.is_rule_expr(ci->second)) {
				// to place each 'rule' on its own line
				o << endl;
			}
			CALL_DUMP_RECURSIVE(o, ci->second, st, v, _type);
		}
	}
	}
	if (need_parens) {
		o << ')';
	}
	if (v && (e.size > 1)) {
		// if verbose, and expression has more than one subexpr
		// print pull-state
		o << '<' <<
			State::node_type::translate_value_to_char(es.pull_state(e))
			<< '>';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Find all leaf node literals.
 */
void
process_sim_state::__collect_expr_literals(const expr_index_type ei, 
		const State& st, vector<node_index_type>& ret) const {
	const unique_process_subgraph& pg(type());
	ISE_INVARIANT(ei < pg.expr_pool.size());
	const graph_node_type& g(pg.expr_graph_node_pool[ei]);
	// can elaborate more on when parens are needed
	// rule attribute printing has moved! (was here)
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	// peel out first iteration for infix printing
	node_index_type gni = 0;
	if (ci->first) {
		gni = st.translate_to_global_node(*this, ci->second);
		ret.push_back(gni);
	} else {
		__collect_expr_literals(ci->second, st, ret);
	}
	if (g.children.size() >= 1) {
	for (++ci; ci!=ce; ++ci) {
		if (ci->first) {
			gni = st.translate_to_global_node(*this, ci->second);
			ret.push_back(gni);
		} else {
			__collect_expr_literals(ci->second, st, ret);
		}
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SETUP_HOLD
/**
	TODO: option to show current status of each timing constraint
	w.r.t. time of last transition of the participating nodes.
	Also flag which ones are pending/active (difference is wrong sign).
 */
ostream&
process_sim_state::dump_timing_constraints(ostream& o,
		const State& st) const {
	const unique_process_subgraph& pg(type());
	// see unique_process_subgraph::dump_timing_constraints
{
	unique_process_subgraph::setup_constraint_set_type::const_iterator
		i(pg.setup_constraints.begin()), e(pg.setup_constraints.end());
	if (i!=e) {
		o << "setup-time constraints:" << endl;
	}
	for ( ; i!=e; ++i) {
		// translate local to global node indices
#if PRSIM_FWD_POST_TIMING_CHECKS
		const string ref(st.get_node_canonical_name(
			st.translate_to_global_node(*this, i->first)));
#else
		const string target(st.get_node_canonical_name(
			st.translate_to_global_node(*this, i->first.first)));
#endif
		vector<setup_constraint_entry>::const_iterator
			ci(i->second.begin()), ce(i->second.end());
		for ( ; ci!=ce; ++ci) {
#if PRSIM_FWD_POST_TIMING_CHECKS
			const string target(st.get_node_canonical_name(
				st.translate_to_global_node(*this, ci->trig_node)));
#else
			const string ref(st.get_node_canonical_name(
				st.translate_to_global_node(*this, ci->ref_node)));
#endif
			o << "t( " << ref << " -> " << target <<
#if PRSIM_FWD_POST_TIMING_CHECKS
				(ci->dir ? '+' : '-')
#else
				(i->first.second ? '+' : '-')
#endif
				<< " ) >= " << ci->time << endl;
		}
	}
}{
	unique_process_subgraph::hold_constraint_set_type::const_iterator
		i(pg.hold_constraints.begin()), e(pg.hold_constraints.end());
	if (i!=e) {
		o << "hold-time constraints:" << endl;
	}
	for ( ; i!=e; ++i) {
		// translate local to global node indices
#if PRSIM_FWD_POST_TIMING_CHECKS
		const string ref(st.get_node_canonical_name(
			st.translate_to_global_node(*this, i->first.first)));
#else
		const string target(st.get_node_canonical_name(
			st.translate_to_global_node(*this, i->first)));
#endif
		vector<hold_constraint_entry>::const_iterator
			ci(i->second.begin()), ce(i->second.end());
		for ( ; ci!=ce; ++ci) {
#if PRSIM_FWD_POST_TIMING_CHECKS
			const string target(st.get_node_canonical_name(
				st.translate_to_global_node(*this, ci->trig_node)));
#else
			const string ref(st.get_node_canonical_name(
				st.translate_to_global_node(*this, ci->ref_node)));
#endif
			o << "t( " << ref <<
#if PRSIM_FWD_POST_TIMING_CHECKS
				(i->first.second ? '+' : '-')
#else
				(ci->dir ? '+' : '-')
#endif
				<< " -> " << target << " ) >= " << ci->time << endl;
		}
	}

}
	return o;
}	// end dump_timing_constraints
#endif	// PRSIM_SETUP_HOLD

//=============================================================================
// explicit class template instantiations
template struct RuleState<State::time_type>;
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

