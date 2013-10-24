/**
	\file "Object/lang/RTE.cc"
	Implementation of RTE objects.
	$Id: RTE.cc,v 1.47 2011/04/02 01:46:04 fang Exp $
 */

#define	ENABLE_STATIC_TRACE		0
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

#include <sstream>
#include <limits>
#include "Object/lang/RTE.hh"
#include "Object/lang/RTE_footprint.hh"
#if 0
#include "Object/lang/RTE_literal_unroller.hh"
#include "Object/unroll/meta_conditional.tcc"
#include "Object/unroll/meta_loop.tcc"
#endif
#include "Object/inst/connection_policy.hh"
#if BOOL_PRS_CONNECTIVITY_CHECKING
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#endif
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/def/footprint.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/traits/bool_traits.hh"
// #include "Object/ref/simple_meta_dummy_reference.hh"

#include "Object/expr/const_param_expr_list.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
// #include "Object/def/template_formals_manager.hh"
// #include "Object/type/template_actuals.hh"
#include "Object/persistent_type_hash.hh"

#include "common/TODO.hh"
#include "common/ICE.hh"
#include "util/IO_utils.hh"
#include "util/indent.hh"
#include "util/value_saver.hh"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"
#include "util/packed_array.hh"	// for bool_alias_collection_type
#include "util/likely.h"
#include "util/stacktrace.hh"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::assignment, "RTE-assign")
// SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::nested_assignments, "RTE-nested")
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::assignment_set, "RTE-set")
#if 0
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::assignment_loop, "RTE-loop")
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::assignment_conditional, "RTE-cond")
#endif
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::binop_expr, "RTE-and")
#if 0
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::or_expr, "RTE-or")
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::binop_expr_loop, "RTE-and-loop")
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::or_expr_loop, "RTE-or-loop")
#endif
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::not_expr, "RTE-not")
SPECIALIZE_UTIL_WHAT(HAC::entity::RTE::literal, "RTE-var")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::assignment, RTE_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::assignment_set, RTE_NESTED_ASSIGNMENTS_TYPE_KEY, 0)
#if 0
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::assignment_loop, RTE_ASSIGNMENT_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::assignment_conditional, RTE_ASSIGNMENT_COND_TYPE_KEY, 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::binop_expr, RTE_BINOP_TYPE_KEY, 0)
#if 0
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::or_expr, RTE_OR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::binop_expr_loop, RTE_BINOP_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::or_expr_loop, RTE_OR_LOOP_TYPE_KEY, 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::not_expr, RTE_NOT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::RTE::literal, RTE_LITERAL_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
namespace entity {
namespace RTE {
using std::copy;
using std::transform;
using std::back_inserter;
using util::persistent_traits;
using util::auto_indent;
#include "util/using_ostream.hh"
using util::write_value;
using util::read_value;
using util::value_saver;

//=============================================================================
// class atomic_assignment method definitions

//=============================================================================
// class atomic_assignment::dumper method definitions


struct atomic_assignment::checker {
	template <class P>
	void
	operator () (const P& r) const {
		STACKTRACE("atomic_assignment::checker::operator()");
		NEVER_NULL(r);
		r->check();
	}
};	// end struct atomic_assignment::checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct atomic_assignment::dumper {
	ostream&		os;
	assignment_dump_context	rdc;
	dumper(ostream& o, const assignment_dump_context& c) : os(o), rdc(c) { }

	template <class P>
	void
	operator () (const P& r) {
		NEVER_NULL(r);
		r->dump(os << auto_indent, rdc) << endl;
	}
};      // end struct atomic_assignment::dumper

//=============================================================================
// class rte_expr::negation_normalizer method definitions

struct rte_expr::checker {
	void
	operator () (const const_rte_expr_ptr_type& e) const {
		STACKTRACE("rte_expr::checker::operator ()");
		assert(e);
		e->check();
	}
};	// end struct checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	After simplification, could just use bind2nd(argval...)
 */
struct rte_expr::unroller {
	const unroll_context& _context;

	explicit
	unroller(const unroll_context& c) : _context(c) { }

	size_t
	operator () (const rte_expr_ptr_type& e) const {
		NEVER_NULL(e);
		return e->unroll(_context);
	}

};	// end struct unroller

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct rte_expr::unroll_copier {
	const unroll_context& _context;

	unroll_copier(const unroll_context& c) : 
		_context(c) {
	}

	rte_expr_ptr_type
	operator () (const rte_expr_ptr_type& e) const {
		NEVER_NULL(e);
		return e->unroll_copy(_context, e);
	}

};	// end struct unroller

//=============================================================================
// class assignment_set_base method definitions

assignment_set_base::assignment_set_base() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment_set_base::~assignment_set_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment_set::assignment_set() : atomic_assignment(), assignment_set_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment_set::~assignment_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
assignment_set_base::what(ostream& o) const {
	return o << "RTE::assignment_set_base";
}
#endif
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
assignment_set::what(ostream& o) const {
	return o << "RTE::assignment_set";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
assignment_set_base::dump(ostream& o, const assignment_dump_context& c) const {
	for_each(begin(), end(), atomic_assignment::dumper(o, c));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Includes optional supply overrides.  
 */
ostream&
assignment_set::dump(ostream& o, const assignment_dump_context& c) const {
	o << '{' << endl;
	assignment_set_base::dump(o, c);
	o << auto_indent << '}';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_CHECK_MEMFUN_DEFAULT(class_name)				\
void									\
class_name::check(void) const {						\
	assignment_set_base::check();						\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_CHECK_MEMFUN_DEFAULT(assignment_set)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Appends rule by transfer of ownership.
 */
void
assignment_set_base::append_assignment(excl_ptr<atomic_assignment>& r) {
	NEVER_NULL(r);
	r->check();             // paranoia
	parent_type::push_back(value_type());
	back() = r;
	INVARIANT(!r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_set_base::check(void) const {
	for_each(begin(), end(), atomic_assignment::checker());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
assignment_set_base::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		if (!(*i)->unroll(c).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the collection of un-resolved production rules
	into production rule footprint.  
 */
good_bool
assignment_set::unroll(const unroll_context& c ) const {
	STACKTRACE_VERBOSE;
//	RTE::footprint& pfp(c.get_target_footprint().get_rte_footprint());
	if (!assignment_set_base::unroll(c).good) {
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_set_base::collect_transient_info_base(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.collect_pointer_list(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_set::collect_transient_info(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_set_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.write_pointer_list(o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_set::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_set_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.read_pointer_list(i, AS_A(parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_set::load_object(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	load_object_base(m, i);
}

//=============================================================================
// class assignment method definitions

assignment::assignment() : atomic_assignment(), guard(), output() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment::assignment(const rte_expr_ptr_type& g, const bool_literal& o) :
		atomic_assignment(), guard(g), output(o) {
	NEVER_NULL(guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment::~assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
assignment::dump(ostream& o, const assignment_dump_context& c) const {
	output.dump(o, c);
	o << " = ";
	guard->dump(o, c);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::check(void) const {
	STACKTRACE("assignment::check()");
	assert(guard);
//	output.check();
	guard->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(assignment)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a production rule into a footprint template form.  
	\param c the context in which this production rule is unrolled.
	\param np the node pool from which to lookup unique local nodes.  
	\param pfp the production rule footprint in which to add
		newly resolved production rules.  
 */
good_bool
assignment::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// resolve guard expression
	RTE::footprint& pfp(c.get_target_footprint().get_rte_footprint());
	const size_t guard_expr_index = guard->unroll(c);
	if (!guard_expr_index) {
		this->dump(cerr << "Error unrolling atomic expression: "
			<< endl << '\t', assignment_dump_context()) << endl;
		// dump context too?
		return good_bool(false);
	}
	// TODO: assert attributes on output (must be atomic)

#if 0
if (output.is_internal()) {
	// we have an internal-node definition
	// resolve indices (if any) to constant.
	const node_literal_ptr_type
		nref(output.unroll_node_reference(c));
	if (!nref) {
		cerr << "Error resolving internal node reference: ";
		output.dump(cerr, assignment_dump_context()) << endl;
		return good_bool(false);
	}
	// register guard expression
	std::ostringstream oss;
	nref->dump_local(oss);
	if (!pfp.register_internal_node_expr(
			oss.str(), guard_expr_index).good) {
		return good_bool(false);
	}
} else {
#endif
	const size_t output_node_index = output.unroll_base(c);
	if (!output_node_index) {
		output.dump(cerr <<
			"Error resolving atomic bool expression: ", 
			assignment_dump_context())
			<< endl;
		return good_bool(false);
	}
	// make sure node is not already defined
#if BOOL_PRS_CONNECTIVITY_CHECKING
	// doing this at unroll-time, but we could do it in a later pass...
{
	entity::footprint& tfp(c.get_target_footprint());
	state_instance<bool_tag>::pool_type&
		bp(tfp.get_instance_pool<bool_tag>());
	// kludge: get_back_ref only returns const ptr ...
#if BOOL_CONNECTIVITY_CHECKING
	const good_bool fig(const_cast<instance_alias_info<bool_tag>&>(
		*bp[output_node_index -1].get_back_ref()).find()->rte_fanin());
	if (!fig.good) {
		cerr << "Attempting to define: ";
		output.dump(cerr, assignment_dump_context()) << endl;
		return good_bool(false);
	}
#endif
	std::set<size_t> f;	// node_index_type
	pfp.collect_literal_indices(f, guard_expr_index);
	std::set<size_t>::const_iterator
		i(f.begin()), e(f.end());
	for ( ; i!=e; ++i) {
		const_cast<instance_alias_info<bool_tag>&>(
			*bp[*i -1].get_back_ref()).find()->rte_fanout();
		// pool is 0-indexed
	}
}
#endif
	pfp.push_back_assignment(guard_expr_index, output_node_index);
#if 0
}	// end if output.is_internal()
#endif
	return good_bool(true);
}	// end assignment::unroll_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	guard->collect_transient_info(m);
	output.collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::write_object(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer(o, guard);
	output.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, guard);
	output.load_object_base(m, i);
}

//=============================================================================
#if 0
// class pull_up method definitions

pull_up::pull_up() : assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::pull_up(const rte_expr_ptr_type& g, 
		const bool_literal& o, const char c) :
		assignment(g, o, c) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::pull_up(const rte_expr_ptr_type& g, const bool_literal& o,
		const assignment_attribute_list_type& l) :
		assignment(g, o, l) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::~pull_up() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pull_up)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pull_up)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pull_up::dump(ostream& o, const assignment_dump_context& c) const {
	return dump_base(o, c, '+');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a production rule into a footprint template form.  
	\param c the context in which this production rule is unrolled.
	\param np the node pool from which to lookup unique local nodes.  
	\param pfp the production rule footprint in which to add
		newly resolved production rules.  
	TODO: check for complement bit
 */
good_bool
pull_up::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return unroll_base(c, true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_up::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_up::write_object(const persistent_object_manager& m, ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_up::load_object(const persistent_object_manager& m, istream& i) {
	load_object_base(m, i);
}

//=============================================================================
// class pull_dn method definitions

pull_dn::pull_dn() : assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::pull_dn(const rte_expr_ptr_type& g, 
		const bool_literal& o, const char c) :
		assignment(g, o, c) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::pull_dn(const rte_expr_ptr_type& g, const bool_literal& o,
		const assignment_attribute_list_type& l) :
		assignment(g, o, l) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::~pull_dn() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pull_dn)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pull_dn)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pull_dn::dump(ostream& o, const assignment_dump_context& c) const {
	return dump_base(o, c, '-');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a production rule into a footprint template form.  
	\param c the context in which this production rule is unrolled.
	\param np the node pool from which to lookup unique local nodes.  
	\param pfp the production rule footprint in which to add
		newly resolved production rules.  
	TODO: check for complement bit
 */
good_bool
pull_dn::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return unroll_base(c, false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_dn::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_dn::write_object(const persistent_object_manager& m, ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_dn::load_object(const persistent_object_manager& m, istream& i) {
	load_object_base(m, i);
}
#endif

//=============================================================================
// class rte_expr method definitions

//=============================================================================
#if 0
// class assignment_conditional method definitions

assignment_conditional::assignment_conditional() : rule(), 
		meta_conditional_base(), clauses() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment_conditional::~assignment_conditional() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(assignment_conditional)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if ALL clauses are empty.  
 */
bool
assignment_conditional::empty(void) const {
	return meta_conditional_type::empty(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
assignment_conditional::dump(ostream& o, const assignment_dump_context& c) const {
	return meta_conditional_type::dump(*this, o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the rules in the body of guard evaluates true.  
 */
good_bool
assignment_conditional::unroll(const unroll_context& c) const {
	return meta_conditional_type::unroll(*this, c, "RTE");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is a static check, so every clause is visited.
 */
void
assignment_conditional::check(void) const {
	typedef	clause_list_type::const_iterator	clause_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	for ( ; ci!=ce; ++ci) {
		for_each(ci->begin(), ci->end(), assignment::checker());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_conditional::append_guarded_clause(const guard_ptr_type& g) {
	meta_conditional_type::append_guarded_clause(*this, g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_conditional::collect_transient_info(persistent_object_manager& m) const {
	meta_conditional_type::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_conditional::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional_type::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_conditional::load_object(const persistent_object_manager& m, istream& i) {
	meta_conditional_type::load_object(*this, m, i);
}
//=============================================================================
// class assignment_loop method definitions

assignment_loop::assignment_loop() : rule(), nested_assignments(), meta_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment_loop::assignment_loop(const ind_var_ptr_type& i, 
		const range_ptr_type& r) :
		nested_assignments(), meta_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
assignment_loop::~assignment_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(assignment_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
assignment_loop::dump(ostream& o, const assignment_dump_context& c) const {
	return meta_loop_type::dump(*this, o, c, ':');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent production rules.  
 */
good_bool
assignment_loop::unroll(const unroll_context& c) const {
	return meta_loop_type::unroll(*this, c, "production rule");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_CHECK_MEMFUN_DEFAULT(assignment_loop)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_loop::collect_transient_info(persistent_object_manager& m) const {
	meta_loop_type::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	meta_loop_type::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	meta_loop_type::load_object(*this, m, i);
}
#endif

//=============================================================================
#if 0
// class expr_loop_base method definitions

inline
expr_loop_base::expr_loop_base() : parent_type(), body_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
expr_loop_base::expr_loop_base(const ind_var_ptr_type& i, 
		const range_ptr_type& r) :
		parent_type(i, r), body_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
expr_loop_base::expr_loop_base(const ind_var_ptr_type& i, 
		const range_ptr_type& r, const rte_expr_ptr_type& e) :
		parent_type(i, r), body_expr(e) {
	NEVER_NULL(body_expr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
expr_loop_base::~expr_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	print_stamp parameter is unused, (just passed down), 
	loop is always parenthesized.  
	TODO: construct entity::expr_dump_context from RTE::expr_dump_context.
 */
ostream&
expr_loop_base::dump(ostream& o, const expr_dump_context& c,
		const char op) const {
	// const bool paren = stamp && (stamp != print_stamp);
	// always parenthesized
	NEVER_NULL(ind_var);
	NEVER_NULL(range);
	o << '(' << op << ':' << ind_var->get_name() << ':';
	range->dump(o, entity::expr_dump_context(c)) << ": ";
	return body_expr->dump(o, c) << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: What to do about 0..0 loops?
	CAST just ignores and skips.  
	\return 0 upon failure.  
	TODO: resolve NULL expressions (CAST Defect Report)
 */
size_t
expr_loop_base::unroll_base(const unroll_context& c, 
		const char type_enum) const {
	// STACKTRACE_VERBOSE;
	RTE::footprint& pfp(c.get_target_footprint().get_rte_footprint());
	// first, resolve bounds of the loop range, using current context
	const_range cr;
	if (!range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression: ";
		range->dump(cerr, entity::expr_dump_context::default_value)
			<< endl;
		return 0;
	}
	static const size_t lim = std::numeric_limits<expr_count_type>::max();
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	if (min > max) {
		cerr << "Sorry, empty expression loops are not yet supported.  "
			"Currently waiting for resolution on language "
			"defect report." << endl;
		return 0;
	} else if (size_t(max -min) > lim) {
		cerr << "ERROR: size of prs expression exceeded limit: " <<
			max -min << " > " << lim << endl;
		return 0;
	}
	// range gives us upper and lower bound of loop
	// in a loop:
	// create context chain of lookup
	//	using unroll_context's template_formal/actual mechanism.  
	// copied from loop_scope::unroll()
	DECLARE_TEMPORARY_FOOTPRINT(f);
	const never_ptr<pint_scalar>
		var(initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& p(var->get_instance().value);  
		// acquire direct reference
	const unroll_context cc(&f, c);
	list<size_t> expr_indices;
	for (p = min; p <= max; ++p) {
		expr_indices.push_back(body_expr->unroll(cc));
	}
	RTE::footprint::expr_node&
		new_expr(pfp.push_back_expr(type_enum, expr_indices.size()));
	copy(expr_indices.begin(), expr_indices.end(), &new_expr[1]);
	// find index of first error (1-indexed)
	const size_t err = new_expr.first_node_error();
	if (err) {
		cerr << "Error resolving production rule expression at:"
			<< endl;
		return 0;	// reserved to signal error
	} else {
		// return the latest index
		return pfp.current_expr_index();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_loop_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	NEVER_NULL(body_expr);
	body_expr->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_loop_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer(o, body_expr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
expr_loop_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
	m.read_pointer(i, body_expr);
}
#endif

//=============================================================================
// class binop_expr method definitions

binop_expr::binop_expr() : rte_expr(), sequence_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
binop_expr::binop_expr(expr_sequence_type::const_reference l, const char o) : 
	rte_expr(), sequence_type(), op(o) {
	sequence_type::push_back(l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
binop_expr::~binop_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(binop_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(binop_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print expression, pre-unrolled.
 */
ostream&
binop_expr::dump(ostream& o, const expr_dump_context& c) const {
	const bool paren = c.expr_stamp && (c.expr_stamp != print_stamp);
	expr_dump_context cc(c);
	cc.expr_stamp = print_stamp;
	const_iterator i(begin());
	const const_iterator e(end());
	NEVER_NULL(*i);
	if (paren) o << '(';
	(*i)->dump(o, cc);
	for (++i; i!=e; ++i) {
		o << ' ' << op << ' ';
		NEVER_NULL(*i);
		(*i)->dump(o, cc);
	}
	if (paren) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	More expensive push_front emulation, hopefully infrequent.
 */
void
binop_expr::push_front(const_reference e) {
	sequence_type temp;
	temp.push_back(e);
	copy(begin(), end(), back_inserter(temp));
	this->swap(temp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	More expensive push_front emulation, hopefully infrequent.
 */
void
binop_expr::push_front(const_reference e) {
	// first expression can't have precharge
	if (!sequence_type::empty()) {
		precharge_array_type temp;
		temp.push_back(precharge_type());	// NULL
		copy(precharge_array.begin(), precharge_array.end(),
			back_inserter(temp));
		precharge_array.swap(temp);
	}
	sequence_type temp;
	temp.push_back(e);
	copy(begin(), end(), back_inserter(temp));
	this->swap(temp);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
void
binop_expr::push_back(const_reference e, const precharge_type& p) {
	sequence_type::push_back(e);
	precharge_array.push_back(p);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr::push_back(const_reference e) {
#if 0
	// first expression can't have precharge
	if (!sequence_type::empty()) {
		precharge_array.push_back(precharge_type());	// NULL
	}
#endif
	sequence_type::push_back(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr::check(void) const {
	STACKTRACE("binop_expr::check()");
	for_each(begin(), end(), rte_expr::checker());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs expressions bottom-up.  
	\return index of newly created expression if successful (1-indexed), 
		else return 0.
 */
size_t
binop_expr::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	list<size_t> expr_indices;
	transform(begin(), end(), back_inserter(expr_indices), 
		rte_expr::unroller(c));
	RTE::footprint& pfp(c.get_target_footprint().get_rte_footprint());
	RTE::footprint::expr_node&
		new_expr(pfp.push_back_expr(
			op == '&' ? PRS::PRS_AND_EXPR_TYPE_ENUM :
			PRS::PRS_OR_EXPR_TYPE_ENUM,
			expr_indices.size()));
	const size_t ret = pfp.current_expr_index();
	copy(expr_indices.begin(), expr_indices.end(), &new_expr[1]);
	// find index of first error (1-indexed)
	const size_t err = new_expr.first_node_error();
	if (err) {
		cerr << "Error resolving production rule expression at:"
			<< endl;
		return 0;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves into an expression with resolved local references.  
	\return copy of self resolved (may be same)
 */
rte_expr_ptr_type
binop_expr::unroll_copy(const unroll_context& c,
		const rte_expr_ptr_type& e) const {
	STACKTRACE_VERBOSE;
	INVARIANT(e == this);
	const count_ptr<this_type> ret(new this_type);
	transform(begin(), end(), back_inserter(*ret), 
		rte_expr::unroll_copier(c));
	// find index of first error (1-indexed), if any
	if (find(ret->begin(), ret->end(), rte_expr_ptr_type(NULL))
			!= ret->end()) {
		cerr << "Error resolving production rule expression: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return rte_expr_ptr_type(NULL);
	}
	if (std::equal(begin(), end(), ret->begin())) {
		return e;
	} else {
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr::collect_transient_info_base(persistent_object_manager& m) const {
	m.collect_pointer_list(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	saves memory on precharge array by writing a sparse map, 
	which is the common case.
 */
void
binop_expr::write_object(const persistent_object_manager& m, ostream& o) const {
	write_value(o, op);
	m.write_pointer_list(o, *this);
#if 0
	// save precharge info sparsely
	typedef	std::map<size_t, precharge_array_type::const_iterator>
						precharge_map_type;
	precharge_map_type temp;
	size_t j = 0;
	precharge_array_type::const_iterator
		i(precharge_array.begin()), e(precharge_array.end());
	for ( ; i!=e; ++i, ++j) {
		if (*i) {
			temp.insert(precharge_map_type::value_type(j, i));
		}
	}
	write_value(o, temp.size());
	precharge_map_type::const_iterator
		mi(temp.begin()), me(temp.end());
	for ( ; mi != me ; ++mi) {
		write_value(o, mi->first);
		mi->second->write_object_base(m, o);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restore precharge list from sparse array.
 */
void
binop_expr::load_object(const persistent_object_manager& m, istream& i) {
	read_value(i, op);
	m.read_pointer_list(i, *this);
#if 0
	size_t j, s;
	INVARIANT(this->size());
	precharge_array.resize(this->size() -1);
	// default construct array first
	read_value(i, s);
	for (j=0; j<s; ++j) {
		size_t ind;
		read_value(i, ind);
		precharge_array[ind].load_object_base(m, i);
	}
#endif
}

//=============================================================================
#if 0
// class binop_expr_loop method definitions

binop_expr_loop::binop_expr_loop() : parent_type(), expr_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
binop_expr_loop::binop_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r) :
		parent_type(), expr_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
binop_expr_loop::binop_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r, const rte_expr_ptr_type& e) :
		parent_type(), expr_loop_base(i, r, e) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
binop_expr_loop::~binop_expr_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(binop_expr_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	print_stamp parameter is unused, loop is always parenthesized.  
 */
ostream&
binop_expr_loop::dump(ostream& o, const expr_dump_context& c) const {
	expr_dump_context cc(c);
	cc.expr_stamp = RTE_LITERAL_TYPE_ENUM;
	return expr_loop_base::dump(o, cc, '&');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr_loop::check(void) const {
	body_expr->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls into compile-time expanded meta expression.  
	\return index of newly created expression if successful (1-indexed), 
		else return 0.
 */
size_t
binop_expr_loop::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return expr_loop_base::unroll_base(c, RTE_AND_EXPR_TYPE_ENUM);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rte_expr_ptr_type
binop_expr_loop::unroll_copy(const unroll_context&, 
		const rte_expr_ptr_type&) const {
	FINISH_ME(Fang);
	return rte_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr_loop::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	expr_loop_base::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	expr_loop_base::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
binop_expr_loop::load_object(const persistent_object_manager& m, istream& i) {
	expr_loop_base::load_object_base(m, i);
}
#endif

//=============================================================================
#if 0
// class or_expr method definitions

or_expr::or_expr() : rte_expr(), sequence_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
or_expr::~or_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(or_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(or_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
or_expr::dump(ostream& o, const expr_dump_context& c) const {
	const bool paren = c.expr_stamp && (c.expr_stamp != print_stamp);
	expr_dump_context cc(c);
	cc.expr_stamp = print_stamp;
	const_iterator i(begin());
	const const_iterator e(end());
	NEVER_NULL(*i);
	if (paren) o << '(';
	(*i)->dump(o, cc);
	for (i++; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->dump(o << " | ", cc);
	}
	if (paren) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr::check(void) const {
	STACKTRACE("or_expr::check()");
	for_each(begin(), end(), rte_expr::checker());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs expressions bottom-up.  
	\return index > 0 if successful, else 0.
 */
size_t
or_expr::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	list<size_t> expr_indices;
	transform(begin(), end(), back_inserter(expr_indices), 
		rte_expr::unroller(c ));
	RTE::footprint& pfp(c.get_target_footprint().get_rte_footprint());
	RTE::footprint::expr_node&
		new_expr(pfp.push_back_expr(
			RTE_OR_EXPR_TYPE_ENUM, expr_indices.size()));
	copy(expr_indices.begin(), expr_indices.end(), &new_expr[1]);
	// find index of first error (1-indexed)
	const size_t err = new_expr.first_node_error();
	if (err) {
		cerr << "Error resolving production rule expression at:"
			<< endl;
		return 0;
	} else {
		return pfp.current_expr_index();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves into an expression with resolved local references.  
	\return copy of self resolved (may be same)
 */
rte_expr_ptr_type
or_expr::unroll_copy(const unroll_context& c,
		const rte_expr_ptr_type& e) const {
	STACKTRACE_VERBOSE;
	INVARIANT(e == this);
	const count_ptr<this_type> ret(new this_type);
	transform(begin(), end(), back_inserter(*ret), 
		rte_expr::unroll_copier(c));
	// find index of first error (1-indexed), if any
	if (find(ret->begin(), ret->end(), rte_expr_ptr_type(NULL))
			!= ret->end()) {
		cerr << "Error resolving production rule expression: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return rte_expr_ptr_type(NULL);
	}
	if (std::equal(begin(), end(), ret->begin())) {
		return e;
	} else {
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr::collect_transient_info_base(persistent_object_manager& m) const {
	m.collect_pointer_list(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer_list(o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer_list(i, *this);
}
#endif

//=============================================================================
#if 0
// class or_expr_loop method definitions

or_expr_loop::or_expr_loop() : parent_type(), expr_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
or_expr_loop::or_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r) :
		parent_type(), expr_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
or_expr_loop::or_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r, const rte_expr_ptr_type& e) :
		parent_type(), expr_loop_base(i, r, e) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
or_expr_loop::~or_expr_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(or_expr_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
or_expr_loop::dump(ostream& o, const expr_dump_context& c) const {
	expr_dump_context cc(c);
	cc.expr_stamp = RTE_LITERAL_TYPE_ENUM;
	return expr_loop_base::dump(o, cc, '|');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr_loop::check(void) const {
	body_expr->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls into compile-time expanded meta expression.  
	\return index of newly created expression if successful (1-indexed), 
		else return 0.
 */
size_t
or_expr_loop::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return expr_loop_base::unroll_base(c, RTE_OR_EXPR_TYPE_ENUM);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rte_expr_ptr_type
or_expr_loop::unroll_copy(const unroll_context&, 
		const rte_expr_ptr_type&) const {
	FINISH_ME(Fang);
	return rte_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr_loop::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	expr_loop_base::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	expr_loop_base::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr_loop::load_object(const persistent_object_manager& m, istream& i) {
	expr_loop_base::load_object_base(m, i);
}
#endif

//=============================================================================
// class not_expr method definitions

not_expr::not_expr() : rte_expr(), var() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
not_expr::not_expr(const rte_expr_ptr_type& g) : rte_expr(), var(g) {
	NEVER_NULL(var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
not_expr::~not_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(not_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(not_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
not_expr::dump(ostream& o, const expr_dump_context& c) const {
	// never needs parentheses
	expr_dump_context cc(c);
	cc.expr_stamp = print_stamp;
	return var->dump(o << "~", cc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
not_expr::check(void) const {
	STACKTRACE("not_expr::check()");
	assert(var);
	var->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a production rule expression.  
 */
size_t
not_expr::unroll(const unroll_context& c
		) const {
	STACKTRACE_VERBOSE;
	const size_t expr_ind = var->unroll(c);
	if (!expr_ind) {
		cerr << "Error unrolling production rule expression." << endl;
		var->dump(cerr << '\t') << endl;
		return 0;
	}
	RTE::footprint& pfp(c.get_target_footprint().get_rte_footprint());
	RTE::footprint::expr_node&
		new_expr(pfp.push_back_expr(PRS::PRS_NOT_EXPR_TYPE_ENUM, 1));
	new_expr[1] = expr_ind;
	return pfp.current_expr_index();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves into an expression with resolved local references.  
	\return copy of self resolved (may be same)
 */
rte_expr_ptr_type
not_expr::unroll_copy(const unroll_context& c,
		const rte_expr_ptr_type& e) const {
	STACKTRACE_VERBOSE;
	INVARIANT(e == this);
	const rte_expr_ptr_type arg(var->unroll_copy(c, var));
	if (!arg) {
		cerr << "Error resolving production rule expression: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return rte_expr_ptr_type(NULL);
	}
	if (arg == var) {
		return e;
	} else {
		return rte_expr_ptr_type(new this_type(arg));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
not_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	var->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
not_expr::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
not_expr::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, var);
}

//=============================================================================
// class literal method definitions

literal::literal() : rte_expr(), base_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
literal::literal(const bool_literal& l) :
		rte_expr(), base_type(l) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
literal::literal(const literal_base_ptr_type& l) :
		rte_expr(), base_type(l) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
literal::literal(const node_literal_ptr_type& l) :
		rte_expr(), base_type(l) { }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
literal::~literal() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(literal)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(literal)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Later, change prototype to pass in pointer to parent definition.  
 */
ostream&
literal::dump(ostream& o, const expr_dump_context& c) const {
	// never needs parentheses
	base_type::dump(o, c);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
literal::check(void) const {
	STACKTRACE("literal::check()");
	assert(var || int_node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper around unroll_base that provide error message.  
	\pre this literal must refer to a bool, not an internal node.
	\return index of the node referenced, local to this definition only, 
		NOT the globally allocated one.  
 */
size_t
literal::unroll_node(const unroll_context& c) const {
	NEVER_NULL(var);
	const size_t ret = unroll_base(c);
	if (!ret) {
		base_type::dump(
			cerr << "Error resolving production rule literal: ", 
			entity::expr_dump_context::default_value)
			<< endl;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has code in common with pull_up/pull_dn...
	\return the index of the new expression represented by this
		literal reference, else 0 if error occurs.  
	TODO: adjust for internal node
 */
size_t
literal::unroll(const unroll_context& c) const {
	RTE::footprint::expr_node* new_expr = NULL;
	RTE::footprint& pfp(c.get_target_footprint().get_rte_footprint());
#if 0
if (is_internal()) {
	const node_literal_ptr_type
		nref(unroll_node_reference(c));
	if (!nref) {
		cerr << "Error resolving internal node reference: ";
		dump(cerr, assignment_dump_context()) << endl;
		return 0;
	}
	std::ostringstream oss;
	nref->dump_local(oss);
	size_t guard_index;
	try {
		guard_index = pfp.lookup_internal_node_expr(
			oss.str(), !is_negated());
	} catch (...) {
		// already have error message
		return 0;
	}
	new_expr = &(pfp.push_back_expr(RTE_NODE_TYPE_ENUM, 1));
	(*new_expr)[1] = guard_index;
} else {
#endif
	const size_t node_index = unroll_node(c);
	if (!node_index) {
		// already have error message
		return 0;
	}
	new_expr = &(pfp.push_back_expr(PRS::PRS_LITERAL_TYPE_ENUM, 1));
	(*new_expr)[1] = node_index;
#if 0
}	// end if int_node
#endif
#if 0
	// TODO: should attributes even apply to internal nodes?
	const size_t perr = bool_directive_source::unroll_params(params, c,
			new_expr->params);
	if (perr) {
		cerr << "Error resolving rule literal parameter " << perr
			<< " in rule." << endl;
		return 0;
	}
	INVARIANT(params.size() <= 2);
	INVARIANT(new_expr->params.size() <= 2);
	if (!unroll_check_attributes(attr, new_expr->attributes, c, 
			cflat_literal_attribute_registry).good) {
		// already have some error message
		cerr << "Error resolving literal attribute." << endl;
		return 0;
	}
#endif
	return pfp.current_expr_index();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rte_expr_ptr_type
literal::unroll_copy(const unroll_context& c,
		const rte_expr_ptr_type& e) const {
	INVARIANT(e == this);
	const bool_literal lref(unroll_reference(c));
	if (!lref.valid()) {
		cerr << "Error resolving prs literal: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return rte_expr_ptr_type(NULL);
	}
#if 0
	// copied from above
	directive_base_params_type crpar;
	literal_params_type rpar;
	const size_t perr =
		bool_directive_source::unroll_params(params, c, crpar);
	if (perr) {
		cerr << "Error resolving rule literal parameter " << perr
			<< " in rule." << endl;
		return rte_expr_ptr_type(NULL);
	}
	copy(crpar.begin(), crpar.end(), back_inserter(rpar));
#endif
#if 0
	resolved_attribute_list_type rat;
	if (!unroll_check_attributes(attr, rat, c, 
			cflat_literal_attribute_registry).good) {
		// already have some error message
		cerr << "Error resolving literal attribute." << endl;
		return rte_expr_ptr_type(NULL);
	}
	if ((lref == *this) &&
		std::equal(params.begin(), params.end(), rpar.begin()) && 
		std::equal(attr.begin(), attr.end(), rat.begin())) {
		return e;
	} else {
		return rte_expr_ptr_type(new literal(lref, rpar, rat));
	}
#else
	return rte_expr_ptr_type(new literal(lref));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers itself as a dynamically allocated object.  
 */
void
literal::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
literal::write_object(const persistent_object_manager& m, ostream& o) const {
//	m.write_pointer(o, var);
	write_object_base(m, o);		// saves var
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
literal::load_object(const persistent_object_manager& m, istream& i) {
//	m.read_pointer(i, var);
	load_object_base(m, i);			// restores var
}

//=============================================================================
}	// end namespace RTE
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

