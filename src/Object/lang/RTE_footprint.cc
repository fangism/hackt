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
				const char* const op = 
					(type == PRS::PRS_AND_EXPR_TYPE_ENUM) ?
						" &" : " |";
				size_t i = 2;
				const size_t s = e.size();
				for ( ; i<=s; ++i) {
					o << op;
					dump_expr(ep[e[i]], o << ' ', fp, type);
				}
			}
			if (paren) o << ')';
			break;
		}
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
void
footprint::collect_atomic_dependencies(const node_pool_type& np, 
		atomic_update_graph& G) const {
	// compute dependency graph
	typedef	assignment_pool_type::const_iterator	const_assignment_iterator;
	const_assignment_iterator i(assignment_pool.begin());
	const const_assignment_iterator e(assignment_pool.end());
	for ( ; i!=e; ++i) {
		__collect_atomic_dependencies_assign(np, *i, G);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::__collect_atomic_dependencies_assign(const node_pool_type& np,
		const assignment& r, atomic_update_graph& G) const {
	set<node_index_type>& deps(G[r.output_index]);
	__collect_atomic_dependencies_expr(np, expr_pool[r.expr_index], deps);
	// take all dependencies, and make sure they have their own entry
	set<node_index_type>::const_iterator i(deps.begin()), e(deps.end());
	for ( ; i!=e; ++i) {
		G[*i];	// create if doesn't yet exist
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::__collect_atomic_dependencies_expr(const node_pool_type& np,
		const expr_node& e, set<node_index_type>& deps) const {
	const expr_pool_type& ep(expr_pool);
	const char type = e.get_type();
	switch (type) {
	case PRS::PRS_LITERAL_TYPE_ENUM: {
		const size_t only = e.only();
		INVARIANT(only);
		// don't bother with non-atomic deps
		if (np[only-1].get_back_ref()->is_atomic()) {
			deps.insert(only);
		}
		break;
	}
	case PRS::PRS_NOT_EXPR_TYPE_ENUM: {
		__collect_atomic_dependencies_expr(np, ep[e.only()], deps);
		break;
	}
	case PRS::PRS_AND_EXPR_TYPE_ENUM:
		// yes, fall-through
	case PRS::PRS_OR_EXPR_TYPE_ENUM: {
		const size_t s = e.size();
	if (s) {
		size_t i = 1;
		for ( ; i<=s; ++i) {
			__collect_atomic_dependencies_expr(np, ep[e[i]], deps);
		}
	}
		break;
	}
	default:
		ICE(cerr, 
		cerr << "Invalid RTE expr type enumeration: "
			<< type << endl;
		)
	}
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
	} else {
		// is some normal expression (NOT, AND, OR)
		expr_index_type i;
		for (i=1; i<=e.size(); ++i) {
			collect_literal_indices(ret, e[i]);
		}
	}
}

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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_expr_node::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// class footprint_assignment method defintions

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

