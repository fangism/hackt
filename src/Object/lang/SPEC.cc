/**
	\file "Object/lang/SPEC.cc"
	$Id: SPEC.cc,v 1.5 2008/10/31 02:11:44 fang Exp $
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
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directive, "SPEC::directive")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::invariant, "SPEC::invariant")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directives_loop, "SPEC::directive")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directives_conditional,
		"SPEC::directive")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::directive, SPEC_DIRECTIVE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::invariant, SPEC_INVARIANT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::directives_loop, SPEC_DIRECTIVE_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::directives_conditional, SPEC_DIRECTIVE_CONDITIONAL_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
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
};	// end struct directive::dumper

//=============================================================================
// class directive method definitions

directive::directive() : directive_abstract(), directive_source() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive::directive(const string& n) :
		directive_abstract(), directive_source(n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive::~directive() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off PRS::macro::dump().
 */
ostream&
directive::dump(ostream& o, const PRS::rule_dump_context& c) const {
	return directive_source::dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
 */
good_bool
directive::unroll(const unroll_context& c, const node_pool_type& np, 
		footprint& sfp) const {
	STACKTRACE_VERBOSE;
	// at least check the instance references first...
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
directive::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive::write_object(const persistent_object_manager& m, ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive::load_object(const persistent_object_manager& m, istream& i) {
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
invariant::unroll(const unroll_context& c, const node_pool_type& np, 
		footprint& sfp) const {
	STACKTRACE_VERBOSE;
	// append to PRS footprint of target context
	// this should work inside conditionals and loops
	PRS::footprint& pfp(c.get_target_footprint().get_prs_footprint());
	const size_t n = invar_expr->unroll(c, np, pfp);
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
directives_set::unroll(const unroll_context& c, const node_pool_type& np, 
		footprint& sfp) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		if (!(*i)->unroll(c, np, sfp).good)
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
	return meta_loop::dump(*this, o, c, ':');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent spec directives.  
 */
good_bool
directives_loop::unroll(const unroll_context& c, const node_pool_type& np, 
		footprint& sfp) const {
	return meta_loop::unroll(*this, c, np, sfp, "spec directive");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::collect_transient_info(persistent_object_manager& m) const {
	meta_loop::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	meta_loop::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	meta_loop::load_object(*this, m, i);
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
	return meta_conditional::empty(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directives_conditional::dump(ostream& o, const rule_dump_context& c) const {
	INVARIANT(guards.size());
	INVARIANT(guards.size() == clauses.size());
	return meta_conditional::dump(*this, o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the directives in the body of guard evaluates true.  
 */
good_bool
directives_conditional::unroll(const unroll_context& c,
		const node_pool_type& np, footprint& sfp) const {
	return meta_conditional::unroll(*this, c, np, sfp, "SPEC");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::append_guarded_clause(const guard_ptr_type& g) {
	meta_conditional::append_guarded_clause(*this, g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::collect_transient_info(
		persistent_object_manager& m) const {
	meta_conditional::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::load_object(const persistent_object_manager& m,
		istream& i) {
	meta_conditional::load_object(*this, m, i);
}

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

