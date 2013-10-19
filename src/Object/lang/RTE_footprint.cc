/**
	\file "Object/lang/RTE_footprint.cc"
	$Id: RTE_footprint.cc,v 1.36 2010/09/29 00:13:37 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_IOS			(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_DUMPS		(0 && ENABLE_STACKTRACE)

#include <iostream>
#include "Object/lang/RTE_footprint.hh"
#include "Object/lang/RTE.hh"
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
#if 0
namespace util {
using namespace HAC::entity::RTE;
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
#endif

//=============================================================================
namespace HAC {
namespace entity {
namespace RTE {
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
#if 0
void
footprint::node_expr_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, name);
	write_value(o, first);
	write_value(o, second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::node_expr_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	read_value(i, name);
	read_value(i, first);
	read_value(i, second);
}
#endif

//=============================================================================
// class footprint method definitions

footprint::footprint() : assignment_pool(), expr_pool() {
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
	See RTE classes implementations of dump for parenthesization rules. 
	Remember, sub-expressions are 1-indexed.  
 */
ostream&
footprint::dump_expr(const expr_node& e, ostream& o, 
		const entity::footprint& fp, const char ps) const {
	STACKTRACE("RTE::footprint::dump_expr()");
	const node_pool_type& np(fp.get_instance_pool<bool_tag>());
	const expr_pool_type& ep(expr_pool);
	STACKTRACE_INDENT_PRINT(" at " << &e << ":" << endl);
	const size_t one __ATTRIBUTE_UNUSED__ = e.size();
	const char type = e.get_type();
	switch (type) {
		case PRS::PRS_LITERAL_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Literal ");
			INVARIANT(one == 1);
			const size_t only = e.only();
			INVARIANT(only);
			INVARIANT(only <= np.local_entries());
			np[only -1].get_back_ref()
				->dump_hierarchical_name(o,
					dump_flags::no_definition_owner);
			break;
		}
		case PRS::PRS_NOT_EXPR_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Not ");
			INVARIANT(one == 1);
			dump_expr(ep[e.only()], o << '~', fp, type);
			break;
		}
		case PRS::PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case PRS::PRS_OR_EXPR_TYPE_ENUM: {
			STACKTRACE_DUMP_PRINT("Or/And ");
			const bool paren = ps && (type != ps);
			if (paren) o << '(';
			if (e.size()) {
				dump_expr(ep[e.only()], o, fp, type);
#if 0
				// also print precharges
				const footprint_expr_node::precharge_map_type&
					pm(e.get_precharges());
				footprint_expr_node::precharge_map_type::const_iterator
					pi(pm.begin()), pe(pm.end());
#endif
				const char* const op = 
					(type == PRS::PRS_AND_EXPR_TYPE_ENUM) ?
						" &" : " |";
				size_t i = 2;
				const size_t s = e.size();
				for ( ; i<=s; ++i) {
					o << op;
#if 0
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
#endif
					dump_expr(ep[e[i]], o << ' ', fp, type);
				}
			}
			if (paren) o << ')';
			break;
		}
#if 0
		case RTE_NODE_TYPE_ENUM: {
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
#endif
		default:
			ICE(cerr, 
			cerr << "Invalid RTE expr type enumeration: "
				<< type << endl;
			)
	}
	return o;
}	// end footprint::dump_expr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print an assignment.
 */
ostream&
footprint::dump_assignment(const assignment& r, ostream& o, 
		const entity::footprint& fp) const {
#if STACKTRACE_DUMPS
	STACKTRACE("RTE::footprint::dump_assignment()");
#endif
	const node_pool_type& np(fp.get_instance_pool<bool_tag>());
	const node_index_type ni = r.output_index -1;	// 0-indexed node_pool
	np[ni].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_definition_owner);
	o << " = ";
	const expr_pool_type& ep(expr_pool);
	dump_expr(ep[r.expr_index],
		o, fp, PRS::PRS_LITERAL_TYPE_ENUM);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Macro must have at least one argument.  
	TODO: dump_node_group to support wrapper/delimiter string arguments.  
 */
ostream&
footprint::dump_macro(const macro& m, ostream& o,
		const entity::footprint& fp) const {
	const node_pool_type& np(fp.get_instance_pool<bool_tag>());
	o << m.name;
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
	print_range(o, assignments);
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need the footprint, which contains the node pool information.  
 */
ostream&
footprint::dump(ostream& o, const entity::footprint& f) const {
if (assignment_pool.size()) {
	o << auto_indent << "resolved rte:" << endl;
	typedef	assignment_pool_type::const_iterator	const_assignment_iterator;
	const_assignment_iterator i(assignment_pool.begin());
	const const_assignment_iterator e(assignment_pool.end());
	for ( ; i!=e; i++) {
		dump_assignment(*i, o << auto_indent, f) << endl;
	}
}
#if 0
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
		dump_expr(en, o, f, RTE_LITERAL_TYPE_ENUM);
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
		dump_expr(expr_pool[i->first], o, f, RTE_LITERAL_TYPE_ENUM);
#else
		dump_expr(expr_pool[*i], o, f, RTE_LITERAL_TYPE_ENUM);
#endif
	// TODO: print the associated string if any
		o << ')' << endl;
	}
}
if (subcircuit_map.size()) {
	// print name of subcircuit?
	assignment_index_type j = 1;		// 1-indexed
	o << auto_indent << "subcircuit (rules, macros, @nodes): " << endl;
	typedef	subcircuit_map_type::const_iterator	const_iterator;
	const_iterator i(subcircuit_map.begin()), e(subcircuit_map.end());
	for ( ; i!=e; ++i, ++j) {
		o << auto_indent << j << ": ";
		i->dump(o) << ' ' << i->get_name() << endl;
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
 */
footprint::assignment&
footprint::push_back_assignment(const int e, const int o) {
	assignment_pool.push_back(assignment(e, o));
	return assignment_pool.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
footprint::macro&
footprint::push_back_macro(const string& s) {
	macro_pool.push_back(macro(s));
	return macro_pool.back();
}
#endif

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
	} else {
		// is some normal expression (NOT, AND, OR)
		expr_index_type i;
		for (i=1; i<=e.size(); ++i) {
			collect_literal_indices(ret, e[i]);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const util::persistent_sequence_collector_ref c(m);
	c(assignment_pool);
	c(expr_pool);
	// the expr_pool doesn't need persistence management yet
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
	util::write_persistent_sequence(m, o, assignment_pool);
}{
	util::write_persistent_sequence(m, o, expr_pool);
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
	util::read_persistent_sequence_back_insert(m, i, assignment_pool);
}{
	util::read_persistent_sequence_back_insert(m, i, expr_pool);
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
		type(t), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_expr_node::footprint_expr_node(const char t, const size_t s) :
		type(t), nodes(s) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
void
footprint_expr_node::push_back_precharge(const size_t i, 
		const expr_index_type e, const bool d) {
	precharge_map.push_back(make_pair(i, make_pair(e, d)));
}
#endif

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
		persistent_object_manager&) const {
// no pointers
#if 0
	STACKTRACE_PERSISTENT_VERBOSE;
	if (type == RTE_LITERAL_TYPE_ENUM) {
		m.collect_pointer_list(params);
	} else	INVARIANT(params.empty());
	if (type == RTE_LITERAL_TYPE_ENUM) {
		attributes.collect_transient_info_base(m);
	}
#endif
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
#if 0
	if (type == RTE_LITERAL_TYPE_ENUM) {
		m.write_pointer_list(o, params);
	} else	INVARIANT(params.empty());
	if (type == RTE_LITERAL_TYPE_ENUM) {
		attributes.write_object_base(m, o);
	}
	if (type == RTE_AND_EXPR_TYPE_ENUM) {
		write_sequence(o, precharge_map);
	} else	INVARIANT(precharge_map.empty());
#endif
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
#if 0
	if (type == RTE_LITERAL_TYPE_ENUM) {
		m.read_pointer_list(i, params);
	}
	if (type == RTE_LITERAL_TYPE_ENUM) {
		attributes.load_object_base(m, i);
	}
	if (type == RTE_AND_EXPR_TYPE_ENUM) {
		read_sequence_resize(i, precharge_map);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_expr_node::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// class footprint_assignment method defintions

#if 0
void
footprint_assignment::push_back(const footprint_assignment_attribute& a) {
	attributes.push_back(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_assignment::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	// no-op
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_assignment::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, expr_index);
	write_value(o, output_index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_assignment::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, expr_index);
	read_value(i, output_index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_assignment::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
}	// end namespace RTE
}	// end namespace entity
}	// end namespace HAC

//=============================================================================
// explicit template instantiations
// really, only need to instantiate copy-constructures, nothing else referenced

namespace util {
using HAC::entity::RTE::footprint;
// Apple gcc-4 rejects, probably fixed on the 4.0 branch...
#if 0 
template class footprint::assignment_pool_type;
template class footprint::expr_pool_type;
#endif
}

//=============================================================================

