/**
	\file "Object/lang/SPEC.cc"
	$Id: SPEC.cc,v 1.3.108.1 2008/01/17 23:01:52 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include "Object/lang/SPEC.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/PRS_literal_unroller.h"	// for PRS::literal
#include "Object/def/footprint.h"
#include "Object/expr/param_expr.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_range.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/value_scalar.h"
#include "Object/persistent_type_hash.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/pint_traits.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/unroll/unroll_context.h"
#include "Object/common/dump_flags.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/memory/chunk_map_pool.tcc"	// for memory pool
#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/IO_utils.h"

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directive, "SPEC::directive")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directives_loop, "SPEC::directive")
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directives_conditional,
		"SPEC::directive")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::directive, SPEC_DIRECTIVE_TYPE_KEY, 0)
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
/**
	TODO: params
 */
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
	NEVER_NULL(ind_var);
	NEVER_NULL(range);
	o << "(:" << ind_var->get_name() << ':';
	range->dump(o, entity::expr_dump_context(c)) << ':' << endl;
	{
		INDENT_SECTION(o);
		directives_set::dump(o, c);
	}
	return o << auto_indent << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent spec directives.  
 */
good_bool
directives_loop::unroll(const unroll_context& c, const node_pool_type& np, 
		footprint& sfp) const {
	// most of this copied from expr_loop_base::unroll...
	// STACKTRACE_VERBOSE;
	// first, resolve bounds of the loop range, using current context
	const_range cr;
	if (!range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression: ";
		range->dump(cerr, entity::expr_dump_context::default_value)
			<< endl;
		return good_bool(false);
	}
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	// if range is empty or backwards, then ignore
	if (min > max) {
		return good_bool(true);
	}
	// range gives us upper and lower bound of loop
	// in a loop:
	// create context chain of lookup
	//	using unroll_context's template_formal/actual mechanism.  
	// copied from loop_scope::unroll()
	entity::footprint f;
	const never_ptr<pint_scalar>
		var(initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& p(var->get_instance().value);  
		// acquire direct reference
	const unroll_context cc(&f, c);
	for (p = min; p <= max; ++p) {
		if (!directives_set::unroll(cc, np, sfp).good) {
			cerr << "Error resolving spec directive in loop:"
				<< endl;
			ind_var->dump_qualified_name(cerr, dump_flags::verbose)
				<< " = " << p << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	directives_set::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	meta_loop_base::write_object_base(m, o);
	directives_set::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	meta_loop_base::load_object_base(m, i);
	directives_set::load_object_base(m, i);
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
	typedef	clause_list_type::const_iterator	clause_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	for ( ; ci!=ce; ++ci) {
		if (!ci->empty())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directives_conditional::dump(ostream& o, const rule_dump_context& c) const {
	INVARIANT(guards.size());
	INVARIANT(guards.size() == clauses.size());
	typedef	clause_list_type::const_iterator	clause_iterator;
	typedef	meta_conditional_base::const_iterator	guard_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	guard_iterator gi(guards.begin()), ge(guards.end());
	entity::expr_dump_context edc(c);
	NEVER_NULL(*gi);
	(*gi)->dump(o << "[ ", edc) << " ->" << endl;
	{
		INDENT_SECTION(o);
		ci->dump(o, c);
	}
	for (++gi, ++ci; ci!=ce; ++gi, ++ci) {
		o << auto_indent << "[] ";
		if (*gi) {
			(*gi)->dump(o, edc);
		} else {
			o << "else";
		}
		o << " ->" << endl;
		INDENT_SECTION(o);
		ci->dump(o, c);
	}
	return o << auto_indent << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the directives in the body of guard evaluates true.  
 */
good_bool
directives_conditional::unroll(const unroll_context& c,
		const node_pool_type& np, footprint& sfp) const {
	typedef	clause_list_type::const_iterator	clause_iterator;
	typedef	meta_conditional_base::const_iterator	guard_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	guard_iterator gi(guards.begin()), ge(guards.end());
for ( ; ci!=ce; ++ci, ++gi) {
	const guard_ptr_type& guard(*gi);
	// guards may be NULL-terminated with else clause
	if (!guard) {
		if (!ci->unroll(c, np, sfp).good) {
			cerr << "Error encountered in conditional SPEC else-clause." << endl;
			return good_bool(false);
		}
		return good_bool(true);
	}
	const count_ptr<const pbool_const>
		g(guard->__unroll_resolve_rvalue(c, guard));
	if (!g) {
		cerr << "Error evaluating guard expression of conditional SPEC."
			<< endl;
		return good_bool(false);
	}
	// no change in context necessary
	if (g->static_constant_value()) {
		const directives_set& if_specs(*ci);
		if (!if_specs.unroll(c, np, sfp).good) {
			cerr << "Error encountered in conditional SPEC if-clause." << endl;
			return good_bool(false);
		}
		return good_bool(true);
	}
}	// end for
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::append_guarded_clause(const guard_ptr_type& g) {
	guards.push_back(g);
	clauses.push_back(directives_set());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	meta_conditional_base::collect_transient_info_base(m);
	for_each(clauses.begin(), clauses.end(),
		util::persistent_collector_ref(m));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional_base::write_object_base(m, o);
	const size_t s = clauses.size();
	util::write_value(o, s);
	for_each(clauses.begin(), clauses.end(),
		util::persistent_writer<directives_set>(
			&directives_set::write_object_base, m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_conditional::load_object(const persistent_object_manager& m, istream& i) {
	meta_conditional_base::load_object_base(m, i);
	size_t s;
	util::read_value(i, s);
	clauses.resize(s);
	for_each(clauses.begin(), clauses.end(),
		util::persistent_loader<directives_set>(
			&directives_set::load_object_base, m, i));
}

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

