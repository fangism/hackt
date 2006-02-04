/**
	\file "Object/lang/PRS_footprint.cc"
	$Id: PRS_footprint.cc,v 1.8.2.2 2006/02/04 05:45:48 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_DUMPS		0 && ENABLE_STACKTRACE

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
#include "Object/lang/cflat_visitor.h"
#include "main/cflat_options.h"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/list_vector.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/stacktrace.h"
// #include "util/memory/count_ptr.tcc"
#include "common/ICE.h"

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"
using util::auto_indent;
using util::write_value;
using util::write_array;
using util::read_value;
using util::read_sequence_prealloc;

//=============================================================================
// class footprint_rule_attribute method definitions

footprint_rule_attribute::footprint_rule_attribute() : key(), values(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_rule_attribute::footprint_rule_attribute(const string& k, 
		const values_type& v) : key(k), values(v) {
	NEVER_NULL(values);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_rule_attribute::~footprint_rule_attribute() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule_attribute::collect_transient_info_base(
		persistent_object_manager& m) const {
	NEVER_NULL(values);
	values->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule_attribute::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	write_value(o, key);
	m.write_pointer(o, values);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule_attribute::load_object(const persistent_object_manager& m, 
		istream& i) {
	read_value(i, key);
	m.read_pointer(i, values);
}

//=============================================================================
// class footprint method definitions

footprint::footprint() : rule_pool(), expr_pool(), macro_pool() {
	rule_pool.set_chunk_size(8);
	expr_pool.set_chunk_size(16);
	macro_pool.set_chunk_size(8);
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
#if STACKTRACE_DUMPS
	STACKTRACE("PRS::footprint::dump_expr()");
	STACKTRACE_INDENT << " at " << &e << ":" << endl;
#endif
	const size_t one = e.size();
	const char type = e.get_type();
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM:
#if STACKTRACE_DUMPS
			STACKTRACE_INDENT << "Literal ";
#endif
#if 0
			if (one != 1) {
				cerr << "size is " << one << endl;
			}
#endif
			INVARIANT(one == 1);
			np[e.only()].get_back_ref()
				->dump_hierarchical_name(o,
					dump_flags::no_definition_owner);
			break;
		case PRS_NOT_EXPR_TYPE_ENUM:
#if STACKTRACE_DUMPS
			STACKTRACE_INDENT << "Not ";
#endif
			INVARIANT(one == 1);
			dump_expr(ep[e.only()], o << '~', np, ep, type);
			break;
		case PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case PRS_OR_EXPR_TYPE_ENUM: {
#if STACKTRACE_DUMPS
			STACKTRACE_INDENT << "Or/And ";
#endif
			const bool paren = ps && (type != ps);
			if (paren) o << '(';
			if (e.size()) {
				dump_expr(ep[e.only()], o, np, ep, type);
				const char* const op = 
					(type == PRS_AND_EXPR_TYPE_ENUM) ?
						" & " : " | ";
				int i = 2;
				const int s = e.size();
				for ( ; i<=s; i++) {
					dump_expr(ep[e[i]],
						o << op, np, ep, type);
				}
			}
			if (paren) o << ')';
			break;
		}
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
	TODO: attributes
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
	o << " [";
	typedef	rule::attributes_list_type::const_iterator	const_iterator;
	const_iterator i(r.attributes.begin());
	const const_iterator e(r.attributes.end());
	for ( ; i!=e; ++i) {
		o << i->key << '=';
		NEVER_NULL(i->values);
		i->values->dump(o, entity::expr_dump_context::default_value);
		o << "; ";
	}
	o << ']';
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Macro must have at least one argument.  
 */
ostream&
footprint::dump_macro(const macro& m, ostream& o, const node_pool_type& np) {
	o << m.name << '(';
	typedef	macro::const_iterator const_iterator;
	const_iterator i(m.begin());
	const const_iterator e(m.end());
	INVARIANT(i!=e);
	np[*i].get_back_ref()->dump_hierarchical_name(o,
		dump_flags::no_definition_owner);
	for (++i; i!=e; ++i) {
		np[*i].get_back_ref()->dump_hierarchical_name(
			o << ',', dump_flags::no_definition_owner);
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
		bpool(f.get_pool<bool_tag>());
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
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
{
	typedef	rule_pool_type::const_iterator	const_rule_iterator;
	const_rule_iterator i(rule_pool.begin());
	const const_rule_iterator e(rule_pool.end());
	for ( ; i!=e; ++i) {
		i->collect_transient_info_base(m);
	}
}
	// the expr_pool and macro_pool don't need persistence management yet
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
{
	typedef	rule_pool_type::const_iterator	const_rule_iterator;
	const size_t s = rule_pool.size();
	write_value(o, s);
	const_rule_iterator i(rule_pool.begin());
	const const_rule_iterator e(rule_pool.end());
	for ( ; i!=e; i++) {
		i->write_object_base(m, o);
	}
}{
	typedef	expr_pool_type::const_iterator	const_expr_iterator;
	const size_t s = expr_pool.size();
	write_value(o, s);
	const_expr_iterator i(expr_pool.begin());
	const const_expr_iterator e(expr_pool.end());
	for ( ; i!=e; i++) {
		i->write_object_base(o);
	}
}{
	typedef	macro_pool_type::const_iterator	const_macro_iterator;
	const size_t s = macro_pool.size();
	write_value(o, s);
	const_macro_iterator i(macro_pool.begin());
	const const_macro_iterator e(macro_pool.end());
	for ( ; i!=e; i++) {
		i->write_object_base(o);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores unrolled expressions and production rules from binary stream.  
	Each subobject is loaded in place.  
 */
void
footprint::load_object_base(const persistent_object_manager& m, istream& i) {
{
	size_t s;
	read_value(i, s);
	// opportunity to resize (chunk) list_vector_pool
	size_t j = 0;
	for ( ; j<s; j++) {
		rule_pool.push_back(rule());
		rule_pool.back().load_object_base(m, i);
	}
}{
	size_t s;
	read_value(i, s);
	// opportunity to resize (chunk) list_vector_pool
	size_t j = 0;
	for ( ; j<s; j++) {
		expr_pool.push_back(expr_node());
		expr_pool.back().load_object_base(i);
	}
}{
	size_t s;
	read_value(i, s);
	// opportunity to resize (chunk) list_vector_pool
	size_t j = 0;
	for ( ; j<s; j++) {
		macro_pool.push_back(macro());
		macro_pool.back().load_object_base(i);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// class footprint_expr_node method defintions

/**
	\return the index of the first zero entry, if any, 
		indicating where an error occurred.  
 */
size_t
footprint_expr_node::first_error(void) const {
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
footprint_expr_node::write_object_base(ostream& o) const {
	STACKTRACE_PERSISTENT("expr_node::write_object_base()");
	write_value(o, type);
	write_array(o, nodes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_expr_node::load_object_base(istream& i) {
	STACKTRACE_PERSISTENT("expr_node::load_object_base()");
	read_value(i, type);
	read_sequence_prealloc(i, nodes);
#if STACKTRACE_PERSISTENTS
	STACKTRACE_INDENT << "at " << this << ":" << endl;
	STACKTRACE_INDENT << "nodes size = " << nodes.size() << endl;
#endif
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
	for_each(attributes.begin(), attributes.end(), 
		util::persistent_collector_ref(m)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT("rule::write_object_base()");
	write_value(o, expr_index);
	write_value(o, output_index);
	write_value(o, dir);
	util::write_persistent_sequence(m, o, attributes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT("rule::load_object_base()");
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

/**
	\return 1-indexed offset of earliest error, or 0 if none found.  
 */
size_t
footprint_macro::first_error(void) const {
	const size_t s = node_args.size();
	if (s) {
		size_t i = 0;
		for ( ; i<s; i++) {
			if (!node_args[i]) {
				cerr << "Error resolving literal " << i <<
					"." << endl;
				return i+1;
			}
		}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_macro::write_object_base(ostream& o) const {
	write_value(o, name);
	util::write_sequence(o, node_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_macro::load_object_base(istream& i) {
	read_value(i, name);
	util::read_sequence_resize(i, node_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_macro::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// explicit template instantiations
// really, only need to instantiate copy-constructures, nothing else referenced

// Apple gcc-4 rejects, probably fixed on the 4.0 branch...
#if 0 
template class footprint::rule_pool_type;
template class footprint::expr_pool_type;
template class footprint::macro_pool_type;
#else
template class util::list_vector<footprint::expr_node>;
template class util::list_vector<footprint::rule>;
template class util::list_vector<footprint::macro>;
#endif

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

