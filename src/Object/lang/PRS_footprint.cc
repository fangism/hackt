/**
	\file "Object/lang/PRS_footprint.cc"
	$Id: PRS_footprint.cc,v 1.36 2010/09/29 00:13:37 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_IOS			(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_DUMPS		(0 && ENABLE_STACKTRACE)

#include <iostream>
#include "Object/lang/PRS_footprint.hh"
#include "Object/lang/PRS.hh"
#include "Object/lang/SPEC.hh"		// for invariant
#include "Object/def/footprint.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/global_entry.hh"
#include "Object/global_entry_context.hh"
#include "Object/common/dump_flags.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/expr/const_param.hh"
#include "Object/lang/cflat_visitor.hh"
#include "Object/lang/directive_base.hh"
#include "Object/traits/instance_traits.hh"
#include "Object/global_channel_entry.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_pool.hh"
#include "main/cflat_options.hh"
#include "util/indent.hh"
#include "util/persistent_object_manager.tcc"	// includes "IO_utils.tcc"
#include "util/persistent_functor.tcc"
#include "util/stacktrace.hh"
#include "util/memory/count_ptr.tcc"
#include "common/ICE.hh"
#include "common/TODO.hh"

#if STACKTRACE_DUMPS
#define	STACKTRACE_DUMP_PRINT(x)		STACKTRACE_INDENT_PRINT(x)
#else
#define	STACKTRACE_DUMP_PRINT(x)
#endif

//=============================================================================
// i/o specializations
namespace util {
using namespace HAC::entity::PRS;
using std::ostream;
using std::istream;

template <>
void
write_value(ostream& os, const footprint::resource_map_entry& r) {
	write_value(os, r.rules);
	write_value(os, r.macros);
	write_value(os, r.int_nodes);
}

template <>
void
read_value(istream& is, footprint::resource_map_entry& r) {
	read_value(is, r.rules);
	read_value(is, r.macros);
	read_value(is, r.int_nodes);
}

}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
namespace PRS {
using std::set;
using std::make_pair;
#include "util/using_ostream.hh"
using util::auto_indent;
using util::write_value;
using util::write_array;
using util::write_sequence;
using util::read_value;
using util::read_sequence_prealloc;
using util::read_sequence_resize;

//=============================================================================
#if PRS_INTERNAL_NODE_ATTRIBUTES
// class footprint::node_expr_type method definitions
void
footprint::node_expr_type::collect_transient_info_base(
		persistent_object_manager& m) const {
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
	for_each(attributes.begin(), attributes.end(), 
		util::persistent_collector_ref(m)
	);
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::node_expr_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, name);
	write_value(o, first);
	write_value(o, second);
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
	util::write_persistent_sequence(m, o, attributes);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::node_expr_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	read_value(i, name);
	read_value(i, first);
	read_value(i, second);
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
	util::read_persistent_sequence_resize(m, i, attributes);
#endif
}

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
	write_value<resource_map_entry>(o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::subcircuit_map_entry::load_object(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, back_ref);
	read_value<resource_map_entry>(i, *this);
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
#if PRS_SUBSTRATE_OVERRIDES
		, current_Vdd_substrate(0)	// INVALID_NODE_INDEX
		, current_GND_substrate(0)	// INVALID_NODE_INDEX
#endif
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
	\param np the scope's node pool, now 0-indexed.
	\param ep the scope's expression pool.
	\param ps the print_stamp corresponding to callee, 
		should be compared with e.type.  
	See PRS classes implementations of dump for parenthesization rules. 
	Remember, sub-expressions are 1-indexed.  
 */
ostream&
footprint::dump_expr(const expr_node& e, ostream& o, 
		const entity::footprint& fp, const char ps) const {
	STACKTRACE("PRS::footprint::dump_expr()");
	const node_pool_type& np(fp.get_instance_pool<bool_tag>());
	const expr_pool_type& ep(expr_pool);
	STACKTRACE_INDENT_PRINT(" at " << &e << ":" << endl);
	const size_t one __ATTRIBUTE_UNUSED__ = e.size();
	const char type = e.get_type();
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Literal ");
			INVARIANT(one == 1);
			const size_t only = e.only();
			INVARIANT(only);
			INVARIANT(only <= np.local_entries());
			np[only -1].get_back_ref()
				->dump_hierarchical_name(o,
					dump_flags::no_definition_owner);
			if (e.params.size() || e.attributes.size()) {
			o << '<';
			directive_base::dump_params_bare(e.params, o);
			if (e.attributes.size()) {
				e.attributes.dump(o << ';');
			}
			o << '>';
			}
			break;
		}
		case PRS_NOT_EXPR_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Not ");
			INVARIANT(one == 1);
			dump_expr(ep[e.only()], o << '~', fp, type);
			break;
		}
		case PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case PRS_OR_EXPR_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Or/And ");
			const bool paren = ps && (type != ps);
			if (paren) o << '(';
			if (e.size()) {
				dump_expr(ep[e.only()], o, fp, type);
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
							o, fp, 
							PRS_NODE_TYPE_ENUM);
						// type doesn't really matter?
						o << '}';
						++pi;
					}
					dump_expr(ep[e[i]], o << ' ', fp, type);
				}
			}
			if (paren) o << ')';
			break;
		}
		case PRS_NODE_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Node ");
			INVARIANT(one == 1);
#if 0
			// expand internal node
			dump_expr(ep[e.only()], o, fp, type);
#else
			// print the name of the internal node only
			const internal_node_pool_type::const_iterator
				f(find_internal_node(e.only()));
			o << '@' << f->name;
#endif
			break;
		}
		default:
			ICE(cerr, 
			cerr << "Invalid PRS expr type enumeration: "
				<< type << endl;
			)
	}
	return o;
}	// end footprint::dump_expr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print a rule.
 */
ostream&
footprint::dump_rule(const rule& r, ostream& o, 
		const entity::footprint& fp) const {
#if STACKTRACE_DUMPS
	STACKTRACE("PRS::footprint::dump_rule()");
#endif
	const node_pool_type& np(fp.get_instance_pool<bool_tag>());
	const expr_pool_type& ep(expr_pool);
	dump_expr(ep[r.expr_index],
		o, fp, PRS_LITERAL_TYPE_ENUM) << " -> ";
	const node_index_type ni = r.output_index -1;	// 0-indexed node_pool
	np[ni].get_back_ref()
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
footprint::dump_macro(const macro& m, ostream& o,
		const entity::footprint& fp) const {
	const node_pool_type& np(fp.get_instance_pool<bool_tag>());
	o << m.name;
if (m.params.size() || m.attributes.size()) {
	o << '<';
	directive_base::dump_params_bare(m.params, o);
	if (m.attributes.size()) {
		m.attributes.dump(o << ';');
	}
	o << '>';
}
	o << '(';
	typedef	macro::nodes_type::const_iterator const_iterator;
	const_iterator i(m.nodes.begin());
	const const_iterator e(m.nodes.end());
	INVARIANT(i!=e);
	bool_directive_base::dump_node_group(*i, o, np);
	for (++i; i!=e; ++i) {
		bool_directive_base::dump_node_group(*i, o << ',', np);
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience printer.
 */
ostream&
footprint::print_range(ostream& o, const index_range& r) {
	INVARIANT(r.first <= r.second);
	if (r.first != r.second) {
		o << r.first << ".." << r.second -1;
	} else {
		o << "none";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint::resource_map_entry::dump(ostream& o) const {
	print_range(o, rules) << ' ';
	print_range(o, macros) << ' ';
	print_range(o, int_nodes);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need the footprint, which contains the node pool information.  
 */
ostream&
footprint::dump(ostream& o, const entity::footprint& f) const {
if (rule_pool.size()) {
	o << auto_indent << "resolved prs:" << endl;
	typedef	rule_pool_type::const_iterator	const_rule_iterator;
	const_rule_iterator i(rule_pool.begin());
	const const_rule_iterator e(rule_pool.end());
	for ( ; i!=e; i++) {
		dump_rule(*i, o << auto_indent, f) << endl;
	}
}
if (macro_pool.size()) {
	o << auto_indent << "resolved macros:" << endl;
	typedef	macro_pool_type::const_iterator	const_macro_iterator;
	const_macro_iterator i(macro_pool.begin());
	const const_macro_iterator e(macro_pool.end());
	for ( ; i!=e; i++) {
		dump_macro(*i, o << auto_indent, f) << endl;
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
		const node_index_type int_node_index = i->second;
		const node_expr_type&
			int_node(internal_node_pool[int_node_index]);
		const bool dir = int_node.second;
		const expr_index_type ex = int_node.first;
		o << auto_indent << '@' << name;
		o << (dir ? '+' : '-') << " <- ";
		const footprint_expr_node& en(expr_pool[ex]);
		const resolved_attribute_list_type&
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
			att(int_node.attributes);
#elif PRS_INTERNAL_NODE_ATTRIBUTES_AT_EXPR
			att(en.attributes);
#endif
		dump_expr(en, o, f, PRS_LITERAL_TYPE_ENUM);
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
		if (att.size()) {
			att.dump(o << " [") << ']';
		}
#endif
		o << endl;
	}
}
if (invariant_pool.size()) {
	o << auto_indent << "invariant exprs: " << endl;
	typedef	invariant_pool_type::const_iterator	const_iterator;
	const_iterator i(invariant_pool.begin()), e(invariant_pool.end());
	for ( ; i!=e; ++i) {
		o << auto_indent << "$(";
#if INVARIANT_BACK_REFS
		dump_expr(expr_pool[i->first], o, f, PRS_LITERAL_TYPE_ENUM);
#else
		dump_expr(expr_pool[*i], o, f, PRS_LITERAL_TYPE_ENUM);
#endif
	// TODO: print the associated string if any
		o << ')' << endl;
	}
}
if (subcircuit_map.size()) {
	// print name of subcircuit?
	rule_index_type j = 1;		// 1-indexed
	o << auto_indent << "subcircuit (rules, macros, @nodes): " << endl;
	typedef	subcircuit_map_type::const_iterator	const_iterator;
	const_iterator i(subcircuit_map.begin()), e(subcircuit_map.end());
	for ( ; i!=e; ++i, ++j) {
		o << auto_indent << j << ": ";
		i->dump(o) << ' ' << i->get_name() << endl;
	}
}
#if PRS_SUPPLY_OVERRIDES
if (supply_map.size()) {
	o << auto_indent << "rule supply map: (rules, macros, @nodes : Vdd, GND)" << endl;
	typedef	supply_map_type::const_iterator	const_iterator;
	const_iterator i(supply_map.begin()), e(supply_map.end());
	for ( ; i!=e; ++i) {
		o << auto_indent;
		i->dump(o) << " : ";
		o << i->Vdd << ", " << i->GND;
#if PRS_SUBSTRATE_OVERRIDES
		o << " | " << i->Vdd_substrate << ", " << i->GND_substrate;
#endif
		o << endl;
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
footprint::push_back_expr(const char t, const expr_index_type s) {
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
footprint::collect_literal_indices(set<node_index_type>& ret,
		const expr_index_type ei) const {
	const footprint_expr_node& e(expr_pool[ei]);
	if (e.is_literal()) {
		ret.insert(e.only());
	} else if (e.is_internal_node()) {
		// precharge nodes do not count as fanin/fanout
	} else {
		// is some normal expression (NOT, AND, OR)
		expr_index_type i;
		for (i=1; i<=e.size(); ++i) {
			collect_literal_indices(ret, e[i]);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param k name of internal node (identifier)
	\param eid root id of the expression that defines the internal node
	\param dir direction that internal node is pulled (1:up, 0:dn)
 */
good_bool
footprint::register_internal_node_expr(const string& k, 
		const expr_index_type eid, const bool dir) {
	typedef	internal_node_expr_map_type::const_iterator
						const_iterator;
	const_iterator f(internal_node_expr_map.find(k));
	if (f != internal_node_expr_map.end()) {
		cerr << "Error: internal node rule for `" << k <<
			"\' already registered." << endl;
		return good_bool(false);
	} else {
		const node_index_type i = internal_node_pool.size();
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
footprint::expr_index_type
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
/**
	Linear search through internal node pool to find
	node that matches the given expression index ei.  
 */
footprint::internal_node_pool_type::const_iterator
footprint::find_internal_node(const expr_index_type ei) const {
	typedef	internal_node_pool_type::const_iterator	const_iterator;
	const const_iterator b(internal_node_pool.begin()),
		e(internal_node_pool.end());
	const_iterator i(b);
	for ( ;i!=e; ++i) {
		if (i->first == ei) {
			return i;
		}
	}
	INVARIANT(i != e);
	return i;		// match not found
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRS_SUPPLY_OVERRIDES
/**
	Routines to lookup voltage supply in rule map.  
 */
footprint::supply_map_type::const_iterator
footprint::lookup_rule_supply(const rule_index_type i) const {
	const supply_map_type& m(supply_map);
	typedef supply_map_type::const_iterator  const_iterator;
	const_iterator f(upper_bound(m.begin(), m.end(), i,
		&rule_supply_map_compare));
	INVARIANT(f != m.begin());
	--f;
	return f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::supply_map_type::const_iterator
footprint::lookup_macro_supply(const macro_index_type i) const {
	const supply_map_type& m(supply_map);
	typedef supply_map_type::const_iterator  const_iterator;
	const_iterator f(upper_bound(m.begin(), m.end(), i,
		&macro_supply_map_compare));
	INVARIANT(f != m.begin());
	--f;
	return f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::supply_map_type::const_iterator
footprint::lookup_internal_node_supply(const node_index_type i) const {
	const supply_map_type& m(supply_map);
	typedef supply_map_type::const_iterator  const_iterator;
	const_iterator f(upper_bound(m.begin(), m.end(), i,
		&internal_node_supply_map_compare));
	INVARIANT(f != m.begin());
	--f;
	return f;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const util::persistent_sequence_collector_ref c(m);
	c(rule_pool);
	c(expr_pool);
	c(macro_pool);
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
	c(internal_node_pool);
#endif
	c(subcircuit_map);
	// the expr_pool doesn't need persistence management yet
	// the internal_node_expr_map doesn't contain pointers
	// invariant_pool contains only never_ptr pointers
	// supply_map contains no pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRS_SUPPLY_OVERRIDES
void
footprint::supply_override_entry::write_object(
		const persistent_object_manager&, ostream& o) const {
	write_value<resource_map_entry>(o, *this);
	write_value(o, Vdd);
	write_value(o, GND);
#if PRS_SUBSTRATE_OVERRIDES
	write_value(o, Vdd_substrate);
	write_value(o, GND_substrate);
#endif
}

void
footprint::supply_override_entry::load_object(
		const persistent_object_manager&, istream& i) {
	read_value<resource_map_entry>(i, *this);
	read_value(i, Vdd);
	read_value(i, GND);
#if PRS_SUBSTRATE_OVERRIDES
	read_value(i, Vdd_substrate);
	read_value(i, GND_substrate);
#endif
}
#endif

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
#if 0
	typedef	rule_pool_type::const_iterator	const_iterator;
	const rule_index_type s = rule_pool.size();
	write_value(o, s);
	const_iterator i(rule_pool.begin());
	const const_iterator e(rule_pool.end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
#else
	util::write_persistent_sequence(m, o, rule_pool);
#endif
}{
#if 0
	typedef	expr_pool_type::const_iterator	const_iterator;
	const expr_index_type s = expr_pool.size();
	write_value(o, s);
	const_iterator i(expr_pool.begin());
	const const_iterator e(expr_pool.end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
#else
	util::write_persistent_sequence(m, o, expr_pool);
#endif
}{
#if 0
	typedef	macro_pool_type::const_iterator	const_iterator;
	const macro_index_type s = macro_pool.size();
	write_value(o, s);
	const_iterator i(macro_pool.begin());
	const const_iterator e(macro_pool.end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
#else
	util::write_persistent_sequence(m, o, macro_pool);
#endif
}{
	// only save non-redundant information from pool
#if 0
	typedef internal_node_pool_type::const_iterator		const_iterator;
	write_value(o, internal_node_pool.size());
	const_iterator i(internal_node_pool.begin()),
		e(internal_node_pool.end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
#else
	util::write_persistent_sequence(m, o, internal_node_pool);
#endif
	// ignore internal_node_expr_map, restore later...
}{
#if INVARIANT_BACK_REFS
	util::write_persistent_sequence(m, o, invariant_pool);
#else
	util::write_sequence(o, invariant_pool);
#endif
	util::write_persistent_sequence(m, o, subcircuit_map);
#if PRS_SUPPLY_OVERRIDES
	util::write_persistent_sequence(m, o, supply_map);
#endif
	// ignore internal_node_expr_map, restore later...
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
#if 0
	rule_index_type s;
	read_value(i, s);
	rule_pool.reserve(s);
	rule_index_type j = 0;
	for ( ; j<s; ++j) {
		rule_pool.push_back(rule());
		rule_pool.back().load_object_base(m, i);
	}
#else
	util::read_persistent_sequence_back_insert(m, i, rule_pool);
#endif
}{
#if 0
	expr_index_type s;
	read_value(i, s);
	expr_pool.reserve(s);
	expr_index_type j = 0;
	for ( ; j<s; ++j) {
		expr_pool.push_back(expr_node());
		expr_pool.back().load_object_base(m, i);
	}
#else
	util::read_persistent_sequence_back_insert(m, i, expr_pool);
#endif
}{
#if 0
	macro_index_type s;
	read_value(i, s);
	macro_pool.reserve(s);
	macro_index_type j = 0;
	for ( ; j<s; ++j) {
		macro_pool.push_back(macro());
		macro_pool.back().load_object_base(m, i);
	}
#else
	util::read_persistent_sequence_back_insert(m, i, macro_pool);
#endif
}{
#if 0
	node_index_type s;
	read_value(i, s);
	node_index_type j = 0;
	internal_node_pool.reserve(s);
	for ( ; j<s; ++j) {
		node_expr_type n;
		n.load_object_base(m, i);
		internal_node_pool.push_back(n);
		internal_node_expr_map[n.name] = j;	// reverse-map
	}
#else
	util::read_persistent_sequence_back_insert(m, i, internal_node_pool);
	const node_index_type s = internal_node_pool.size();
	// reconstruct reverse-map
	size_t j = 0;
	for ( ; j<s; ++j) {
		internal_node_expr_map[internal_node_pool[j].name] = j;
	}
#endif
	INVARIANT(internal_node_expr_map.size() == s);
}{
#if INVARIANT_BACK_REFS
	util::read_persistent_sequence_resize(m, i, invariant_pool);
#else
	util::read_sequence_resize(i, invariant_pool);
#endif
	util::read_persistent_sequence_resize(m, i, subcircuit_map);
#if PRS_SUPPLY_OVERRIDES
	util::read_persistent_sequence_resize(m, i, supply_map);
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
	} else	INVARIANT(params.empty());
	if (type == PRS_LITERAL_TYPE_ENUM
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_EXPR
			|| type == PRS_NODE_TYPE_ENUM
#endif
			) {
		attributes.collect_transient_info_base(m);
	}
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
	} else	INVARIANT(params.empty());
	if (type == PRS_LITERAL_TYPE_ENUM
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_EXPR
			|| type == PRS_NODE_TYPE_ENUM
#endif
			) {
		attributes.write_object_base(m, o);
	}
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
	}
	if (type == PRS_LITERAL_TYPE_ENUM
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_EXPR
			|| type == PRS_NODE_TYPE_ENUM
#endif
			) {
		attributes.load_object_base(m, i);
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
#if INVARIANT_BACK_REFS
// class footprint::invariant_type method definitions

#if 0
void
footprint::invariant_type::collect_transient_info(
		persistent_object_manager&) const {
}
#endif

void
footprint::invariant_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, first);
	NEVER_NULL(second);
	m.write_pointer(o, second);
}

void
footprint::invariant_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	read_value(i, first);
	m.read_pointer(i, second);
	NEVER_NULL(second);
}
#endif

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
void
footprint_macro::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	attributes.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_macro::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	attributes.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_macro::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
	attributes.load_object_base(m, i);
}

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

