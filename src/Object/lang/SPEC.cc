/**
	\file "Object/lang/SPEC.cc"
	$Id: SPEC.cc,v 1.10.2.1 2010/08/23 18:38:44 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <algorithm>
#include <iterator>
#include "Object/lang/SPEC.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/PRS_literal_unroller.h"	// for PRS::literal
#include "Object/lang/PRS_footprint.h"	// for PRS::literal
#include "Object/persistent_type_hash.h"
#include "Object/def/footprint.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/unroll/meta_conditional.tcc"
#include "Object/unroll/meta_loop.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"	// for memory pool
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/IO_utils.h"

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::bool_directive, "SPEC::bool_directive")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::invariant, "SPEC::invariant")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directives_loop, "SPEC::directives_loop")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directives_conditional,
		"SPEC::directives_cond")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::bool_directive, SPEC_DIRECTIVE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::proc_directive, SPEC_PROC_DIRECTIVE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::invariant, SPEC_INVARIANT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::directives_loop, SPEC_DIRECTIVE_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::directives_conditional, SPEC_DIRECTIVE_CONDITIONAL_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
//=============================================================================
// global variables
bool
allow_private_member_references = false;

//=============================================================================
namespace SPEC {
using std::for_each;
using std::back_inserter;
using std::transform;
using util::auto_indent;
#include "util/using_ostream.h"
using PRS::rule_dump_context;
using util::read_value;
using util::write_value;

//=============================================================================
// helper class definitions

/**
	ostream binding functor.  
 */
struct directive_abstract::dumper {
	ostream&				os;
	const rule_dump_context&		rdc;

	dumper(ostream& o, const rule_dump_context& r) : os(o), rdc(r) { }

	template <class P>
	void
	operator () (const P& p) {
		NEVER_NULL(p);
		p->dump(os << auto_indent, rdc) << endl;
	}
};	// end struct directive_abstract::dumper

//=============================================================================
// class bool_directive method definitions

bool_directive::bool_directive() :
		directive_abstract(), bool_directive_source() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_directive::bool_directive(const string& n) :
		directive_abstract(), bool_directive_source(n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_directive::~bool_directive() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(bool_directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(bool_directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off PRS::macro::dump().
 */
ostream&
bool_directive::dump(ostream& o, const PRS::rule_dump_context& c) const {
	return bool_directive_source::dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
 */
good_bool
bool_directive::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// at least check the instance references first...
	footprint& sfp(c.get_target_footprint().get_spec_footprint());
	footprint_directive& new_directive(sfp.push_back_directive(name));
	const size_t perr = unroll_params(c, new_directive.params);
	if (perr) {
		cerr << "Error resolving expression at position " << perr
			<< " of spec directive \'" << name << "\'." << endl;
		// dump the literal?
		return good_bool(false);
	}
	const size_t nerr = unroll_nodes(c, new_directive.nodes);
	if (nerr) {
		cerr << "Error resolving literal node at position " << nerr
			<< " of spec directive \'" << name << "\'." << endl;
		// dump the literal?
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_directive::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_directive::write_object(const persistent_object_manager& m, ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_directive::load_object(const persistent_object_manager& m, istream& i) {
	load_object_base(m, i);
}

//-----------------------------------------------------------------------------
// class proc_directive method definitions
// cloned from bool_directive

proc_directive::proc_directive() :
		directive_abstract(), proc_directive_source() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_directive::proc_directive(const string& n) :
		directive_abstract(), proc_directive_source(n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_directive::~proc_directive() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(proc_directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off PRS::macro::dump().
 */
ostream&
proc_directive::dump(ostream& o, const PRS::rule_dump_context& c) const {
	return proc_directive_source::dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
 */
good_bool
proc_directive::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// at least check the instance references first...
	footprint& sfp(c.get_target_footprint().get_spec_footprint());
	footprint_directive& new_directive(sfp.push_back_directive(name));
	const size_t perr = unroll_params(c, new_directive.params);
	if (perr) {
		cerr << "Error resolving expression at position " << perr
			<< " of spec directive \'" << name << "\'." << endl;
		// dump the literal?
		return good_bool(false);
	}
	const size_t nerr = unroll_nodes(c, new_directive.nodes);
	if (nerr) {
		cerr << "Error resolving literal node at position " << nerr
			<< " of spec directive \'" << name << "\'." << endl;
		// dump the literal?
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_directive::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_directive::write_object(const persistent_object_manager& m, ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_directive::load_object(const persistent_object_manager& m, istream& i) {
	load_object_base(m, i);
}

//=============================================================================
// class invariant method definitions

invariant::invariant() : directive_abstract(), invar_expr() { }

invariant::invariant(const count_ptr<const prs_expr>& e) :
		directive_abstract(), invar_expr(e) {
	NEVER_NULL(invar_expr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
invariant::~invariant() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(invariant)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
invariant::dump(ostream& o, const PRS::rule_dump_context& c) const {
	return invar_expr->dump(o << "$(", c) << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Kludge: SPEC directive crossing over into PRS body...
 */
good_bool
invariant::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// append to PRS footprint of target context
	// this should work inside conditionals and loops
	entity::footprint& fp(c.get_target_footprint());
	PRS::footprint& pfp(fp.get_prs_footprint());
	const size_t n = invar_expr->unroll(c
		// np, pfp
		);
	// returns 1-indexed!!! (but expr_pool is also offset-by-1)
	if (!n) {
		cerr << "Error resolving invariant expression." << endl;
		return good_bool(false);
	}
	pfp.push_back_invariant(n);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
invariant::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	invar_expr->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
invariant::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, invar_expr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
invariant::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, invar_expr);
}

//=============================================================================
// class directives_set method definitions

directives_set::directives_set() : directives_set_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directives_set::~directives_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directives_set::dump(ostream& o, const rule_dump_context& rdc) const {
	for_each(begin(), end(), directive_abstract::dumper(o, rdc));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves directives, binding node arguments to local IDs, 
	suitable for complete types.  
 */
good_bool
directives_set::unroll(const unroll_context& c) const {
	// in SPEC context, allow private member references
	const util::value_saver<bool>
		_tmp_(allow_private_member_references, true);
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		if (!(*i)->unroll(c).good)
			return good_bool(false);
			// will already have error message
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_set::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(AS_A(const parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_set::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer_list(o, AS_A(const parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_set::load_object_base(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer_list(i, AS_A(parent_type&, *this));
}

//=============================================================================
// class directives_loop method definitions
// mostly ripped from "Object/lang/PRS.cc":rule_loop

directives_loop::directives_loop() : directives_set(), meta_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directives_loop::directives_loop(const ind_var_ptr_type& i, 
		const range_ptr_type& r) :
		directives_set(), meta_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directives_loop::~directives_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(directives_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directives_loop::dump(ostream& o, const rule_dump_context& c) const {
	return meta_loop_type::dump(*this, o, c, ':');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent spec directives.  
 */
good_bool
directives_loop::unroll(const unroll_context& c) const {
	return meta_loop_type::unroll(*this, c, "spec bool-directive");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::collect_transient_info(persistent_object_manager& m) const {
	meta_loop_type::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	meta_loop_type::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	meta_loop_type::load_object(*this, m, i);
}

//=============================================================================
// class directives_conditional method definitions
// mostly ripped from "Object/lang/PRS.cc":rule_conditional

directives_conditional::directives_conditional() : directive_abstract(), 
		meta_conditional_base(), clauses() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directives_conditional::~directives_conditional() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(directives_conditional)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if ALL clauses are empty.  
 */
bool
directives_conditional::empty(void) const {
	return meta_conditional_type::empty(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directives_conditional::dump(ostream& o, const rule_dump_context& c) const {
	INVARIANT(guards.size());
	INVARIANT(guards.size() == clauses.size());
	return meta_conditional_type::dump(*this, o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the directives in the body of guard evaluates true.  
 */
good_bool
directives_conditional::unroll(const unroll_context& c) const {
	return meta_conditional_type::unroll(*this, c, "SPEC");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::append_guarded_clause(const guard_ptr_type& g) {
	meta_conditional_type::append_guarded_clause(*this, g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::collect_transient_info(
		persistent_object_manager& m) const {
	meta_conditional_type::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional_type::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::load_object(const persistent_object_manager& m,
		istream& i) {
	meta_conditional_type::load_object(*this, m, i);
}

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

