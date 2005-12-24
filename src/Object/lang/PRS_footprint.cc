/**
	\file "Object/lang/PRS_footprint.cc"
	$Id: PRS_footprint.cc,v 1.4.2.2 2005/12/24 02:33:33 fang Exp $
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
#include "Object/lang/cflat_visitor.h"
#include "main/cflat_options.h"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/list_vector.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
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
// class footprint method definitions

footprint::footprint() : rule_pool(), expr_pool() {
	rule_pool.set_chunk_size(8);
	expr_pool.set_chunk_size(16);
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
					dump_flags::no_owner);
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
ostream&
footprint::dump_rule(const rule& r, ostream& o, const node_pool_type& np, 
		const expr_pool_type& ep) {
#if STACKTRACE_DUMPS
	STACKTRACE("PRS::footprint::dump_rule()");
#endif
	dump_expr(ep[r.expr_index],
		o, np, ep, PRS_LITERAL_TYPE_ENUM) << " -> ";
	np[r.output_index].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_owner);
	return o << (r.dir ? '+' : '-');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints production rule expression suitable for a cflat tool.  
	TODO: convert method to take template functor argument.  
 */
void
footprint::cflat_expr(const expr_node& e, ostream& o, 
		const footprint_frame_map<bool_tag>& bfm,
		const entity::footprint& topfp, const cflat_options& cf, 
		const state_manager& sm, 
		const expr_pool_type& ep, const char ps) {
#if STACKTRACE_DUMPS
	STACKTRACE("PRS::footprint::cflat_expr()");
	STACKTRACE_INDENT << " at " << &e << ":" << endl;
#endif
	const size_t one = e.size();
	const char type = e.get_type();
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM:
			INVARIANT(one == 1);
			if (!cf.check_prs) {
				if (cf.enquote_names) o << '\"';
				sm.get_pool<bool_tag>()[bfm[e.only()-1]]
					.dump_canonical_name(o, topfp, sm);
				if (cf.enquote_names) o << '\"';
			}
			break;
		case PRS_NOT_EXPR_TYPE_ENUM:
			INVARIANT(one == 1);
			if (!cf.check_prs)
				o << '~';
			cflat_expr(ep[e.only()], o, bfm, topfp, cf, 
				sm, ep, type);
			break;
		case PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case PRS_OR_EXPR_TYPE_ENUM: {
			const bool paren = ps && (type != ps);
			if (!cf.check_prs && paren) o << '(';
			if (e.size()) {
				cflat_expr(ep[e.only()], o, bfm, 
					topfp, cf, sm, ep, type);
				const char* const op = 
					(type == PRS_AND_EXPR_TYPE_ENUM) ?
						" & " : " | ";
				int i = 2;
				const int s = e.size();
				for ( ; i<=s; i++) {
					if (!cf.check_prs)
						o << op;
					cflat_expr(ep[e[i]],
						o, bfm, topfp, cf, 
						sm, ep, type);
				}
			}
			if (!cf.check_prs && paren) o << ')';
			break;
		}
		default:
			ICE(cerr, 
			cerr << "Invalid PRS expr type enumeration: "
				<< type << endl;
			)
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints production rule suitable for a cflat tool.  
 */
void
footprint::cflat_rule(const rule& r, ostream& o, 
		const footprint_frame_map<bool_tag>& bfm,
		const entity::footprint& topfp, const cflat_options& cf, 
		const state_manager& sm, const expr_pool_type& ep) {
#if STACKTRACE_DUMPS
	STACKTRACE("PRS::footprint::cflat_rule()");
	STACKTRACE_INDENT << " at " << &e << ":" << endl;
#endif
	cflat_expr(ep[r.expr_index],
		o, bfm, topfp, cf, sm, ep, PRS_LITERAL_TYPE_ENUM);
if (!cf.check_prs) {
	o << " -> ";
	// r.output_index gives the local unique ID, 
	// which needs to be translated to global ID.  
	// bfm[...] refers to a global_entry<bool_tag> (1-indexed)
	// const size_t j = bfm[r.output_index-1];
	if (cf.enquote_names) o << '\"';
	sm.get_pool<bool_tag>()[bfm[r.output_index-1]]
		.dump_canonical_name(o, topfp, sm);
	if (cf.enquote_names) o << '\"';
	o << (r.dir ? '+' : '-') << endl;
}
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
	const state_instance<bool_tag>::pool_type&
		bpool(f.get_pool<bool_tag>());
	const_rule_iterator i(rule_pool.begin());
	const const_rule_iterator e(rule_pool.end());
	for ( ; i!=e; i++) {
		dump_rule(*i, o << auto_indent, bpool, expr_pool) << endl;
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
/**
	Iterates over each local production rule, and replaces
	local literals with globally allocated node IDs.  
 */
void
footprint::cflat_prs(ostream& o, const footprint_frame_map<bool_tag>& bfm, 
		const entity::footprint& topfp, const cflat_options& cf, 
		const state_manager& sm) const {
	typedef	rule_pool_type::const_iterator	const_rule_iterator;
	const_rule_iterator i(rule_pool.begin());
	const const_rule_iterator e(rule_pool.end());
	for ( ; i!=e; i++) {
		cflat_rule(*i, o, bfm, topfp, cf, sm, expr_pool);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves unrolled expressions and production rules to binary stream.  
 */
void
footprint::write_object_base(ostream& o) const {
{
	typedef	rule_pool_type::const_iterator	const_rule_iterator;
	const size_t s = rule_pool.size();
	write_value(o, s);
	size_t j = 0;
	const_rule_iterator i(rule_pool.begin());
	for ( ; j<s; i++, j++) {
		i->write_object_base(o);
	}
}{
	typedef	expr_pool_type::const_iterator	const_pool_iterator;
	const size_t s = expr_pool.size();
	write_value(o, s);
	size_t j = 0;
	const_pool_iterator i(expr_pool.begin());
	for ( ; j<s; i++, j++) {
		i->write_object_base(o);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores unrolled expressions and production rules from binary stream.  
 */
void
footprint::load_object_base(istream& i) {
{
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; j++) {
		rule_pool.push_back(rule());
		rule_pool.back().load_object_base(i);
	}
}{
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; j++) {
		expr_pool.push_back(expr_node());
		expr_pool.back().load_object_base(i);
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
footprint_rule::write_object_base(ostream& o) const {
	STACKTRACE_PERSISTENT("rule::write_object_base()");
	write_value(o, expr_index);
	write_value(o, output_index);
	write_value(o, dir);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::load_object_base(istream& i) {
	STACKTRACE_PERSISTENT("rule::load_object_base()");
	read_value(i, expr_index);
	read_value(i, output_index);
	read_value(i, dir);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_rule::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// explicit template instantiations
// really, only need to instantiate copy-constructures, nothing else referenced

// Apple gcc-4 rejects, probably fixed on the 4.0 branch...
#if 0 
template class footprint::rule_pool_type;
template class footprint::expr_pool_type;
#else
#if 0
template class util::offset_array<util::list_vector<footprint::expr_node> >;
#else
template class util::list_vector<footprint::expr_node>;
#endif
template class util::list_vector<footprint::rule>;
#endif

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

