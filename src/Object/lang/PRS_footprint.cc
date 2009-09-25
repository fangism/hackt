/**
	\file "Object/lang/PRS_footprint.cc"
	$Id: PRS_footprint.cc,v 1.26.2.2 2009/09/25 01:21:39 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_IOS			(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_DUMPS		(0 && ENABLE_STACKTRACE)

#include <iostream>
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/PRS.h"
#include "Object/def/footprint.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_param.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/lang/directive_base.h"
#include "Object/traits/instance_traits.h"
#include "Object/global_channel_entry.h"
#include "main/cflat_options.h"
#include "util/indent.h"
#include "util/persistent_object_manager.tcc"	// includes "IO_utils.tcc"
#include "util/persistent_functor.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "common/ICE.h"

#if STACKTRACE_DUMPS
#define	STACKTRACE_DUMP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#else
#define	STACKTRACE_DUMP_PRINT(x)
#endif

namespace HAC {
namespace entity {
namespace PRS {
using std::set;
using std::make_pair;
#include "util/using_ostream.h"
using util::auto_indent;
using util::write_value;
using util::write_array;
using util::write_sequence;
using util::read_value;
using util::read_sequence_prealloc;
using util::read_sequence_resize;

//=============================================================================
// class footprint::subcircuit_map_entry method definitions

const string&
footprint::subcircuit_map_entry::get_name(void) const {
	NEVER_NULL(back_ref);
	return back_ref->get_name();
}

void
footprint::subcircuit_map_entry::collect_transient_info_base(
		persistent_object_manager& m) const {
	// technically don't need to collect as back-reference
	// is guaranteed to be reached first
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::subcircuit_map_entry::write_object(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, back_ref);
	write_value(o, rules);
	write_value(o, macros);
	write_value(o, int_nodes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::subcircuit_map_entry::load_object(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, back_ref);
	read_value(i, rules);
	read_value(i, macros);
	read_value(i, int_nodes);
}

//=============================================================================
// class footprint method definitions

footprint::footprint() : rule_pool(), expr_pool(), macro_pool(), 
		internal_node_pool(), 
		internal_node_expr_map(), invariant_pool(),
		subcircuit_map()
#if PRS_SUPPLY_OVERRIDES
		, supply_map()
		, current_Vdd(0)	// INVALID_NODE_INDEX
		, current_GND(0)	// INVALID_NODE_INDEX
#endif
	{
	// used to set_chunk_size of list_vector_pools here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::~footprint() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param e the expression node to print.
	\param o the output stream.
	\param np the scope's node pool.
	\param ep the scope's expression pool.
	\param ps the print_stamp corresponding to callee, 
		should be compared with e.type.  
	See PRS classes implementations of dump for parenthesization rules. 
	Remember, sub-expressions are 1-indexed.  
 */
ostream&
footprint::dump_expr(const expr_node& e, ostream& o, 
		const node_pool_type& np, const expr_pool_type& ep, 
		const char ps) {
	STACKTRACE("PRS::footprint::dump_expr()");
	STACKTRACE_INDENT_PRINT(" at " << &e << ":" << endl);
	const size_t one __ATTRIBUTE_UNUSED__ = e.size();
	const char type = e.get_type();
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM:
			STACKTRACE_DUMP_PRINT("Literal ");
			INVARIANT(one == 1);
			np[e.only()].get_back_ref()
				->dump_hierarchical_name(o,
					dump_flags::no_definition_owner);
			if (e.params.size()
#if PRS_LITERAL_ATTRIBUTES
				|| e.attributes.size()
#endif
				) {
			o << '<';
			directive_base::dump_params_bare(e.params, o);
#if PRS_LITERAL_ATTRIBUTES
			if (e.attributes.size()) {
				e.attributes.dump(o << ';');
			}
#endif
			o << '>';
			}
			break;
		case PRS_NOT_EXPR_TYPE_ENUM:
			STACKTRACE_DUMP_PRINT("Not ");
			INVARIANT(one == 1);
			dump_expr(ep[e.only()], o << '~', np, ep, type);
			break;
		case PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case PRS_OR_EXPR_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Or/And ");
			const bool paren = ps && (type != ps);
			if (paren) o << '(';
			if (e.size()) {
				dump_expr(ep[e.only()], o, np, ep, type);
				// also print precharges
				const footprint_expr_node::precharge_map_type&
					pm(e.get_precharges());
				footprint_expr_node::precharge_map_type::const_iterator
					pi(pm.begin()), pe(pm.end());
				const char* const op = 
					(type == PRS_AND_EXPR_TYPE_ENUM) ?
						" &" : " |";
				size_t i = 2;
				const size_t s = e.size();
				for ( ; i<=s; ++i) {
					o << op;
					if (pi != pe && i-2 == pi->first) {
						o << '{' <<
						(pi->second.second ? '+' : '-');
						dump_expr(ep[pi->second.first], 
							o, np, ep, 
							PRS_NODE_TYPE_ENUM);
						// type doesn't really matter?
						o << '}';
						++pi;
					}
					dump_expr(ep[e[i]], o << ' ', 
						np, ep, type);
				}
			}
			if (paren) o << ')';
			break;
		}
		case PRS_NODE_TYPE_ENUM:
			STACKTRACE_DUMP_PRINT("Node ");
			INVARIANT(one == 1);
			dump_expr(ep[e.only()], o, np, ep, type);
			break;
		default:
			ICE(cerr, 
			cerr << "Invalid PRS expr type enumeration: "
				<< type << endl;
			)
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print a rule.
 */
ostream&
footprint::dump_rule(const rule& r, ostream& o, const node_pool_type& np, 
		const expr_pool_type& ep) {
#if STACKTRACE_DUMPS
	STACKTRACE("PRS::footprint::dump_rule()");
#endif
	dump_expr(ep[r.expr_index],
		o, np, ep, PRS_LITERAL_TYPE_ENUM) << " -> ";
	np[r.output_index].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_definition_owner);
	o << (r.dir ? '+' : '-');
if (r.attributes.size()) {
	r.attributes.dump(o << " [") << ']';
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Macro must have at least one argument.  
	TODO: dump_node_group to support wrapper/delimiter string arguments.  
 */
ostream&
footprint::dump_macro(const macro& m, ostream& o, const node_pool_type& np) {
	o << m.name;
if (m.params.size() || m.attributes.size()) {
	o << '<';
	directive_base::dump_params_bare(m.params, o);
#if PRS_LITERAL_ATTRIBUTES
//	o << '[' << m.attributes.size() << ']';
	if (m.attributes.size()) {
		m.attributes.dump(o << ';');
	}
#endif
	o << '>';
}
	o << '(';
	typedef	macro::nodes_type::const_iterator const_iterator;
	const_iterator i(m.nodes.begin());
	const const_iterator e(m.nodes.end());
	INVARIANT(i!=e);
	directive_base::dump_node_group(*i, o, np);
	for (++i; i!=e; ++i) {
		directive_base::dump_node_group(*i, o << ',', np);
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need the footprint, which contains the node pool information.  
 */
ostream&
footprint::dump(ostream& o, const entity::footprint& f) const {
	const state_instance<bool_tag>::pool_type&
		bpool(f.get_instance_pool<bool_tag>());
if (rule_pool.size()) {
	o << auto_indent << "resolved prs:" << endl;
	typedef	rule_pool_type::const_iterator	const_rule_iterator;
	const_rule_iterator i(rule_pool.begin());
	const const_rule_iterator e(rule_pool.end());
	for ( ; i!=e; i++) {
		dump_rule(*i, o << auto_indent, bpool, expr_pool) << endl;
	}
}
if (macro_pool.size()) {
	o << auto_indent << "resolved macros:" << endl;
	typedef	macro_pool_type::const_iterator	const_macro_iterator;
	const_macro_iterator i(macro_pool.begin());
	const const_macro_iterator e(macro_pool.end());
	for ( ; i!=e; i++) {
		dump_macro(*i, o << auto_indent, bpool) << endl;
	}
}
if (internal_node_expr_map.size()) {
	o << auto_indent << "internal node exprs: " << endl;
	typedef	internal_node_expr_map_type::const_iterator
					const_map_iterator;
	const_map_iterator i(internal_node_expr_map.begin());
	const const_map_iterator e(internal_node_expr_map.end());
	for ( ; i!=e; ++i) {
		// is this dump format acceptable?
		const string& name(i->first);
		const size_t int_node_index = i->second;
		const bool dir = internal_node_pool[int_node_index].second;
		const expr_index_type ex =
			internal_node_pool[int_node_index].first;
		o << auto_indent << '@' << name;
		o << (dir ? '+' : '-') << " <- ";
		dump_expr(expr_pool[ex],
			o, bpool, expr_pool, PRS_LITERAL_TYPE_ENUM) << endl;
	}
}
if (invariant_pool.size()) {
	o << auto_indent << "invariant exprs: " << endl;
	typedef	invariant_pool_type::const_iterator	const_iterator;
	const_iterator i(invariant_pool.begin()), e(invariant_pool.end());
	for ( ; i!=e; ++i) {
		o << auto_indent << "$(";
		dump_expr(expr_pool[*i], o, bpool, expr_pool, 
			PRS_LITERAL_TYPE_ENUM);
		o << ')' << endl;
	}
}
	static const char none[] = "none";
if (subcircuit_map.size()) {
	// print name of subcircuit?
	size_t j = 1;		// 1-indexed
	o << auto_indent << "subcircuit (rules, macros, @nodes): " << endl;
	typedef	subcircuit_map_type::const_iterator	const_iterator;
	const_iterator i(subcircuit_map.begin()), e(subcircuit_map.end());
	for ( ; i!=e; ++i, ++j) {
		o << auto_indent << j << ": ";
		if (i->rules.second != i->rules.first) {
			o << i->rules.first << ".." << i->rules.second -1;
		} else {
			o << none;
		}
		o << ' ';
		if (i->macros.second != i->macros.first) {
			o << i->macros.first << ".." << i->macros.second -1;
		} else {
			o << none;
		}
		o << ' ';
		if (i->int_nodes.second != i->int_nodes.first) {
			o << i->int_nodes.first << ".." <<
				i->int_nodes.second -1;
		} else {
			o << none;
		}
		o << ' ' << i->get_name();
		o << endl;
	}
}
#if PRS_SUPPLY_OVERRIDES
if (supply_map.size()) {
	o << auto_indent << "rule supply map: (rules, macros, Vdd, GND)" << endl;
	typedef	supply_map_type::const_iterator	const_iterator;
	const_iterator i(supply_map.begin()), e(supply_map.end());
	for ( ; i!=e; ++i) {
		o << auto_indent;
		if (i->rules.second != i->rules.first) {
			o << i->rules.first << ".." << i->rules.second -1;
		} else {
			o << none;
		}
		o << ' ';
		if (i->macros.second != i->macros.first) {
			o << i->macros.first << ".." << i->macros.second -1;
		} else {
			o << none;
		}
		o << " : " << i->Vdd << ", " << i->GND << endl;
	}
}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t the expression type.  
 */
footprint::expr_node&
footprint::push_back_expr(const char t, const size_t s) {
	expr_pool.push_back(expr_node());
	expr_node& ret(expr_pool.back());
	ret.set_type(t);
	ret.resize(s);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a new footprint production rule.  
	\param e the guard expression local footprint ID.
	\param o the output node local footprint ID.  
	\param d the directionL true = up, false = down. 
 */
footprint::rule&
footprint::push_back_rule(const int e, const int o, const bool d) {
	rule_pool.push_back(rule(e, o, d));
	return rule_pool.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::macro&
footprint::push_back_macro(const string& s) {
	macro_pool.push_back(macro(s));
	return macro_pool.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Gather all node indices that appear in the expression's literals.
	\param ret return set of unique indices
	\param ei expression node index to visit.
 */
void
footprint::collect_literal_indices(set<size_t>& ret,
		const size_t ei) const {
	const footprint_expr_node& e(expr_pool[ei]);
	if (e.is_literal()) {
		ret.insert(e.only());
	} else if (e.is_internal_node()) {
		// precharge nodes do not count as fanin/fanout
	} else {
		// is some normal expression (NOT, AND, OR)
		size_t i;
		for (i=1; i<=e.size(); ++i) {
			collect_literal_indices(ret, e[i]);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
footprint::register_internal_node_expr(const string& k, const size_t eid, 
		const bool dir) {
	typedef	internal_node_expr_map_type::const_iterator
						const_iterator;
	const_iterator f(internal_node_expr_map.find(k));
	if (f != internal_node_expr_map.end()) {
		cerr << "Error: internal node rule for `" << k <<
			"\' already registered." << endl;
		return good_bool(false);
	} else {
		const size_t i = internal_node_pool.size();
		internal_node_pool.push_back(node_expr_type(eid, dir, k));
		internal_node_expr_map[k] = i;
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return index of expression representing internal node, 
		or throw exception to signal error or not found.
 */
size_t
footprint::lookup_internal_node_expr(const string& k, const bool dir) const {
	typedef	internal_node_expr_map_type::const_iterator
						const_iterator;
	const_iterator f(internal_node_expr_map.find(k));
	if (f != internal_node_expr_map.end()) {
		const bool ndir = 
			internal_node_pool[f->second].second;
		if (ndir == dir) {
			return internal_node_pool[f->second].first;
		} else {
			cerr << "Error: internal node `" << k <<
				"\' is used in the wrong sense." << endl;
			THROW_EXIT;	// no return
		}
	} else {
		cerr << "Error: undefined internal node rule: " << k << endl;
		THROW_EXIT;	// no return
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const util::persistent_sequence_collector_ref c(m);
	c(rule_pool);
	c(expr_pool);
	c(macro_pool);
	c(subcircuit_map);
	// the expr_pool doesn't need persistence management yet
	// the internal_node_expr_map doesn't contain pointers
	// invariant_pool contains no pointers
	// supply_map contains no pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves unrolled expressions and production rules to binary stream.  
	TODO: consider using value_writer and value_reader template classes
	and util::write_sequence and util::read_sequence...
 */
void
footprint::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
{
	typedef	rule_pool_type::const_iterator	const_iterator;
	const size_t s = rule_pool.size();
	write_value(o, s);
	const_iterator i(rule_pool.begin());
	const const_iterator e(rule_pool.end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
}{
	typedef	expr_pool_type::const_iterator	const_iterator;
	const size_t s = expr_pool.size();
	write_value(o, s);
	const_iterator i(expr_pool.begin());
	const const_iterator e(expr_pool.end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
}{
	typedef	macro_pool_type::const_iterator	const_iterator;
	const size_t s = macro_pool.size();
	write_value(o, s);
	const_iterator i(macro_pool.begin());
	const const_iterator e(macro_pool.end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
}{
	// only save non-redundant information from pool
	typedef internal_node_pool_type::const_iterator		const_iterator;
	write_value(o, internal_node_pool.size());
	const_iterator i(internal_node_pool.begin()),
		e(internal_node_pool.end());
	for ( ; i!=e; ++i) {
		write_value(o, i->name);
		write_value(o, i->first);
		write_value(o, i->second);
	}
	// ignore internal_node_expr_map, restore later...
}{
	util::write_sequence(o, invariant_pool);
	util::write_persistent_sequence(m, o, subcircuit_map);
#if PRS_SUPPLY_OVERRIDES
{
	// only save non-redundant information from pool
	typedef supply_map_type::const_iterator		const_iterator;
	write_value(o, supply_map.size());
	const_iterator i(supply_map.begin()), e(supply_map.end());
	for ( ; i!=e; ++i) {
		write_value(o, i->rules);
		write_value(o, i->macros);
		write_value(o, i->Vdd);
		write_value(o, i->GND);
	}
	// ignore internal_node_expr_map, restore later...
}
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores unrolled expressions and production rules from binary stream.  
	Each subobject is loaded in place.  
 */
void
footprint::load_object_base(const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
{
	size_t s;
	read_value(i, s);
	rule_pool.reserve(s);
	size_t j = 0;
	for ( ; j<s; ++j) {
		rule_pool.push_back(rule());
		rule_pool.back().load_object_base(m, i);
	}
}{
	size_t s;
	read_value(i, s);
	expr_pool.reserve(s);
	size_t j = 0;
	for ( ; j<s; ++j) {
		expr_pool.push_back(expr_node());
		expr_pool.back().load_object_base(m, i);
	}
}{
	size_t s;
	read_value(i, s);
	macro_pool.reserve(s);
	size_t j = 0;
	for ( ; j<s; ++j) {
		macro_pool.push_back(macro());
		macro_pool.back().load_object_base(m, i);
	}
}{
	size_t s;
	read_value(i, s);
	size_t j = 0;
	internal_node_pool.reserve(s);
	for ( ; j<s; ++j) {
		node_expr_type n;
		read_value(i, n.name);
		read_value(i, n.first);
		read_value(i, n.second);
		internal_node_pool.push_back(n);
		internal_node_expr_map[n.name] = j;	// reverse-map
	}
	INVARIANT(internal_node_expr_map.size() == s);
}{
	util::read_sequence_resize(i, invariant_pool);
	util::read_persistent_sequence_resize(m, i, subcircuit_map);
#if PRS_SUPPLY_OVERRIDES
{
	size_t s;
	read_value(i, s);
	size_t j = 0;
	supply_map.reserve(s);
	for ( ; j<s; ++j) {
		supply_override_entry n;
		read_value(i, n.rules);
		read_value(i, n.macros);
		read_value(i, n.Vdd);
		read_value(i, n.GND);
		supply_map.push_back(n);
	}
	INVARIANT(supply_map.size() == s);
}
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// class footprint_expr_node method defintions

footprint_expr_node::footprint_expr_node() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_expr_node::footprint_expr_node(const char t) :
		type(t), nodes(), params() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_expr_node::footprint_expr_node(const char t, const size_t s) :
		type(t), nodes(s), params() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_expr_node::push_back_precharge(const size_t i, 
		const expr_index_type e, const bool d) {
	precharge_map.push_back(make_pair(i, make_pair(e, d)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first zero entry, if any, 
		indicating where an error occurred.  
 */
size_t
footprint_expr_node::first_node_error(void) const {
	const size_t s = nodes.size();
	if (s) {
		size_t i = 0;
		for ( ; i<s; i++) {
			if (!nodes[i]) {
				cerr << "Error in sub-expression " << i <<
					"." << endl;
				return i+1;
			}
		}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_expr_node::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	if (type == PRS_LITERAL_TYPE_ENUM) {
		m.collect_pointer_list(params);
#if PRS_LITERAL_ATTRIBUTES
		attributes.collect_transient_info_base(m);
#endif
	} else	INVARIANT(params.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: (optimization) instead of writing nodes unconditionally, 
		write depending on the type, for singletons.  
		params can likewise be reduced where they are irrelevant.
 */
void
footprint_expr_node::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, type);
	write_array(o, nodes);
	if (type == PRS_LITERAL_TYPE_ENUM) {
		m.write_pointer_list(o, params);
#if PRS_LITERAL_ATTRIBUTES
		attributes.write_object_base(m, o);
#endif
	} else	INVARIANT(params.empty());
	if (type == PRS_AND_EXPR_TYPE_ENUM) {
		write_sequence(o, precharge_map);
	} else	INVARIANT(precharge_map.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_expr_node::load_object_base(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, type);
	read_sequence_prealloc(i, nodes);
	STACKTRACE_PERSISTENT_PRINT("at " << this << ":" << endl);
	STACKTRACE_PERSISTENT_PRINT("nodes size = " << nodes.size() << endl);
	if (type == PRS_LITERAL_TYPE_ENUM) {
		m.read_pointer_list(i, params);
#if PRS_LITERAL_ATTRIBUTES
		attributes.load_object_base(m, i);
#endif
	}
	if (type == PRS_AND_EXPR_TYPE_ENUM) {
		read_sequence_resize(i, precharge_map);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_expr_node::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// class footprint_rule method defintions

void
footprint_rule::push_back(const footprint_rule_attribute& a) {
	attributes.push_back(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	for_each(attributes.begin(), attributes.end(), 
		util::persistent_collector_ref(m)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, expr_index);
	write_value(o, output_index);
	write_value(o, dir);
	util::write_persistent_sequence(m, o, attributes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, expr_index);
	read_value(i, output_index);
	read_value(i, dir);
	util::read_persistent_sequence_resize(m, i, attributes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// struct footprint_macro method definitions

void
footprint_macro::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRS_LITERAL_ATTRIBUTES
void
footprint_macro::collect_transient_info_base(
		persistent_object_manager& m) const {
	directive_base::collect_transient_info_base(m);
	attributes.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_macro::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	directive_base::write_object_base(m, o);
	attributes.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_macro::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	directive_base::load_object_base(m, i);
	attributes.load_object_base(m, i);
}
#endif

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

//=============================================================================
// explicit template instantiations
// really, only need to instantiate copy-constructures, nothing else referenced

namespace util {
using HAC::entity::PRS::footprint;
// Apple gcc-4 rejects, probably fixed on the 4.0 branch...
#if 0 
template class footprint::rule_pool_type;
template class footprint::expr_pool_type;
template class footprint::macro_pool_type;
#endif
}

//=============================================================================

