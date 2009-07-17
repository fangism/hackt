/**
	\file "Object/lang/PRS.cc"
	Implementation of PRS objects.
	$Id: PRS.cc,v 1.34.2.1 2009/07/17 01:10:51 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_CC__
#define	__HAC_OBJECT_LANG_PRS_CC__

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE		0

#include <sstream>
#include <limits>
#include "Object/lang/PRS.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/PRS_macro_registry.h"
#include "Object/lang/PRS_literal_unroller.h"
#include "Object/unroll/meta_conditional.tcc"
#include "Object/unroll/meta_loop.tcc"
#include "Object/inst/connection_policy.h"
#if BOOL_PRS_CONNECTIVITY_CHECKING
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#endif

#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/node_traits.h"
#include "Object/ref/simple_meta_dummy_reference.h"

#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/def/template_formals_manager.h"
#include "Object/type/template_actuals.h"
#include "Object/persistent_type_hash.h"

#include "common/TODO.h"
#include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"
#include "util/qmap.tcc"	// for const_assoc_query::operator[]
#include "util/packed_array.h"	// for bool_alias_collection_type
#include "util/likely.h"
#include "util/stacktrace.h"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::pull_up, "PRS-up")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::pull_dn, "PRS-dn")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::nested_rules, "PRS-nested")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::rule_loop, "PRS-loop")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::rule_conditional, "PRS-cond")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::and_expr, "PRS-and")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::or_expr, "PRS-or")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::and_expr_loop, "PRS-and-loop")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::or_expr_loop, "PRS-or-loop")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::not_expr, "PRS-not")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::literal, "PRS-var")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::pull_up, PRS_PULLUP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::pull_dn, PRS_PULLDN_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::rule_loop, PRS_RULE_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::nested_rules, PRS_NESTED_RULES_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::subcircuit, PRS_SUBCKT_RULES_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::rule_conditional, PRS_RULE_COND_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::and_expr, PRS_AND_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::or_expr, PRS_OR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::and_expr_loop, PRS_AND_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::or_expr_loop, PRS_OR_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::not_expr, PRS_NOT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::literal, PRS_LITERAL_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::PRS::macro, PRS_MACRO_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
namespace entity {
namespace PRS {
using std::copy;
using std::transform;
using std::back_inserter;
using util::persistent_traits;
using util::auto_indent;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

//=============================================================================
// class rule method definitions

//=============================================================================
// class rule::dumper method definitions


struct rule::checker {
	template <class P>
	void
	operator () (const P& r) const {
		STACKTRACE("rule::checker::operator()");
		NEVER_NULL(r);
		r->check();
	}
};	// end struct rule::checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct rule::dumper {
	ostream&		os;
	rule_dump_context	rdc;
	dumper(ostream& o, const rule_dump_context& c) : os(o), rdc(c) { }

	template <class P>
	void
	operator () (const P& r) {
		NEVER_NULL(r);
		r->dump(os << auto_indent, rdc) << endl;
	}
};      // end struct rule::dumper

//=============================================================================
// class prs_expr::negation_normalizer method definitions

struct prs_expr::checker {
	void
	operator () (const const_prs_expr_ptr_type& e) const {
		STACKTRACE("prs_expr::checker::operator ()");
		assert(e);
		e->check();
	}
};	// end struct checker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct prs_expr::negater {
	prs_expr_ptr_type
	operator () (const const_prs_expr_ptr_type& e) const {
		STACKTRACE("prs_expr::negater::operator ()");
		NEVER_NULL(e);
		return e->negate();
	}
};	// end struct negater

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct prs_expr::literal_flipper {
	prs_expr_ptr_type
	operator () (const const_prs_expr_ptr_type& e) const {
		STACKTRACE("prs_expr::literal_flipper::operator ()");
		NEVER_NULL(e);
		return e->flip_literals();
	}
};	// end struct literal_flipper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct prs_expr::negation_normalizer {
	const bool nb;
	negation_normalizer(const bool b) : nb(b) { }

/**
	\return modified (normalized) expression if changed, else
		pointer to the original expression.  
 */
	prs_expr_ptr_type
	operator () (const prs_expr_ptr_type& e) const {
		STACKTRACE("prs_expr::negation_normalizer::operator ()");
		NEVER_NULL(e);
		if (nb) {
			return e->negate();
		} else {
			const prs_expr_ptr_type ret(e->negation_normalize());
			return (ret ? ret : e);
		}
	}
};	// end struct negation_normalizer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct prs_expr::unroller {
	const unroll_context& _context;
	const node_pool_type& _node_pool;
	PRS::footprint& _fpf;

	unroller(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& fpf) : _context(c), _node_pool(np), _fpf(fpf) {
	}

	size_t
	operator () (const prs_expr_ptr_type& e) const {
		NEVER_NULL(e);
		return e->unroll(_context, _node_pool, _fpf);
	}

};	// end struct unroller

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct prs_expr::unroll_copier {
	const unroll_context& _context;

	unroll_copier(const unroll_context& c) : 
		_context(c) {
	}

	prs_expr_ptr_type
	operator () (const prs_expr_ptr_type& e) const {
		NEVER_NULL(e);
		return e->unroll_copy(_context, e);
	}

};	// end struct unroller

//=============================================================================
// class rule_set method definitions

rule_set::rule_set() : rule(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rule_set::~rule_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
rule_set::what(ostream& o) const {
	return o << "PRS::rule_set";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
rule_set::dump(ostream& o, const rule_dump_context& c) const {
	for_each(begin(), end(), rule::dumper(o, c));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Appends rule by transfer of ownership, 
	also auto-expanding complements.  
 */
void
rule_set::append_rule(excl_ptr<rule>& r) {
	NEVER_NULL(r);
	r->check();             // paranoia
	excl_ptr<rule> cmpl = r->expand_complement();
	parent_type::push_back(value_type());
	back() = r;
	INVARIANT(!r);
	if (cmpl) {
		parent_type::push_back(value_type());
		back() = cmpl;
		INVARIANT(!cmpl);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::compact_references(void) {
	cerr << "Fang, write PRS::rule_set::compact_references()!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::check(void) const {
	for_each(begin(), end(), rule::checker());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<rule>
rule_set::expand_complement(void) {
	expand_complements();
	return excl_ptr<rule>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits each rule, and if can be expanded, will insert its
	complement before the original rule.  The original rule
	will then unset its complement flag.  
 */
void
rule_set::expand_complements(void) {
	iterator i(begin());
	const iterator e(end());
	for ( ; i!=e; i++) {
		excl_ptr<rule> cmpl = (*i)->expand_complement();
		if (cmpl) {
			*insert(i, value_type()) = cmpl;
			INVARIANT(!cmpl);	// transferred ownership
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the collection of un-resolved production rules
	into production rule footprint.  
 */
good_bool
rule_set::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		if (!(*i)->unroll(c, np, pfp).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::collect_transient_info_base(persistent_object_manager& m) const {
	m.collect_pointer_list(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, AS_A(parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::load_object(const persistent_object_manager& m, 
		istream& i) {
	load_object_base(m, i);
}

//=============================================================================
// class attribute method definitions

attribute::attribute() : generic_attribute() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
attribute::attribute(const string& k) : generic_attribute(k) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
attribute::~attribute() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
attribute::dump(ostream& o, const rule_dump_context& c) const {
	return generic_attribute::dump(o, entity::expr_dump_context(c));
}

//=============================================================================
// class pull_base method definitions

pull_base::pull_base() : rule(), guard(), output(), 
		arrow_type(ARROW_NORMAL), 
		attributes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_base::pull_base(const prs_expr_ptr_type& g, 
		const bool_literal& o, const char c) :
		rule(), guard(g), output(o), 
		arrow_type(c), attributes() {
	NEVER_NULL(guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_base::pull_base(const prs_expr_ptr_type& g, 
		const bool_literal& o, const rule_attribute_list_type& l) :
		rule(), guard(g), output(o), 
		arrow_type(ARROW_NORMAL), attributes(l) {
	NEVER_NULL(guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_base::~pull_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pull_base::dump_base(ostream& o, const rule_dump_context& c, 
		const char dir) const {
	static const char* const norm_arrow = " -> ";
	static const char* const comp_arrow = " => ";
	static const char* const flip_arrow = " #> ";
	guard->dump(o, c);
	switch (arrow_type) {
	case ARROW_NORMAL: o << norm_arrow; break;
	case ARROW_COMPLEMENT: o << comp_arrow; break;
	case ARROW_FLIP: o << flip_arrow; break;
	default: o << " ?> ";
	}
	output.dump(o, c);
	o << dir;
	if (!attributes.empty()) {
		o << " [";
		typedef	rule_attribute_list_type::const_iterator
							const_iterator;
		const_iterator i(attributes.begin());
		const const_iterator e(attributes.end());
		for ( ; i!=e; ++i) {
			i->dump(o, c) << "; ";
		}
		o << ']';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_base::check(void) const {
	STACKTRACE("pull_base::check()");
	// check attributes?
	assert(guard);
//	output.check();
	guard->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a production rule into a footprint template form.  
	\param c the context in which this production rule is unrolled.
	\param np the node pool from which to lookup unique local nodes.  
	\param pfp the production rule footprint in which to add
		newly resolved production rules.  
	\param dir is the direction, true for up, false for down.
 */
good_bool
pull_base::unroll_base(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp, const bool dir) const {
	STACKTRACE_VERBOSE;
	// resolve guard expression
	const size_t guard_expr_index = guard->unroll(c, np, pfp);
	if (!guard_expr_index) {
		this->dump(cerr << "Error unrolling production rule guard: "
			<< endl << '\t', rule_dump_context()) << endl;
		// dump context too?
		return good_bool(false);
	}
if (output.is_internal()) {
	// we have an internal-node definition
	// rule-attributes are ignored for internal-node definitions
	if (attributes.size()) {
		cerr <<
	"Warning: internal node definitions ignore rule attributes.  "
			<< endl;
	}
	// resolve indices (if any) to constant.
	const node_literal_ptr_type
		nref(output.unroll_node_reference(c));
	if (!nref) {
		cerr << "Error resolving internal node reference: ";
		output.dump(cerr, rule_dump_context()) << endl;
		return good_bool(false);
	}
	// register guard expression
	std::ostringstream oss;
	nref->dump_local(oss);
	return pfp.register_internal_node_expr(
		oss.str(), guard_expr_index, dir);
} else {
	// rule is a standard pull-up/dn
	const size_t output_node_index = output.unroll_base(c);
	if (!output_node_index) {
		output.dump(cerr <<
			"Error resolving output node of production rule: ", 
			rule_dump_context())
			<< endl;
		return good_bool(false);
	}
	// check for auto-complement, and unroll it?
#if BOOL_PRS_CONNECTIVITY_CHECKING
	// doing this at unroll-time, but we could do it in a later pass...
{
	entity::footprint& tfp(c.get_target_footprint());
	state_instance<bool_tag>::pool_type&
		bp(tfp.get_instance_pool<bool_tag>());
	// kludge: get_back_ref only returns const ptr ...
	const_cast<instance_alias_info<bool_tag>&>(
		*bp[output_node_index].get_back_ref()).find()->prs_fanin(dir);
	std::set<size_t> f;	// node_index_type
	pfp.collect_literal_indices(f, guard_expr_index);
	std::set<size_t>::const_iterator
		i(f.begin()), e(f.end());
	for ( ; i!=e; ++i) {
		const_cast<instance_alias_info<bool_tag>&>(
			*bp[*i].get_back_ref()).find()->prs_fanout(dir);
	}
}
#endif
	footprint_rule&
		r(pfp.push_back_rule(guard_expr_index, output_node_index, dir));
{
	typedef	rule_attribute_list_type::const_iterator	const_iterator;
	const_iterator i(attributes.begin());
	const const_iterator e(attributes.end());
	for ( ; i!=e; ++i) {
		const string& key(i->get_key());
		// check whether or not named attribute is registered
		// NOTE: every directive should at least be registered
		// as a cflat directive, the master set of all directives.  
		const cflat_attribute_definition_entry
			att(cflat_attribute_registry[key]);
		if (!att) {
			cerr << "Error: unrecognized attribute \'" << key <<
				"\'." << endl;
			return good_bool(false);
		}
		const count_ptr<const const_param_expr_list>
			att_vals(i->unroll_values(c));
		if (!att_vals) {
			// allow value-less attributes
			const const_param_expr_list empty;
			if (!att.check_values(empty).good)
				return good_bool(false);
		} else
		if (!att.check_values(*att_vals).good) {
			// already have error message
			return good_bool(false);
		}
		r.push_back(footprint_rule_attribute(key, att_vals));
	}
}
}	// end if output.is_internal()
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_base::collect_transient_info_base(persistent_object_manager& m) const {
	guard->collect_transient_info(m);
	output.collect_transient_info_base(m);
	for_each(attributes.begin(), attributes.end(),
		util::persistent_collector_ref(m)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_base::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer(o, guard);
	output.write_object_base(m, o);
	write_value(o, arrow_type);
	util::write_persistent_sequence(m, o, attributes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_base::load_object_base(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, guard);
	output.load_object_base(m, i);
	read_value(i, arrow_type);
	util::read_persistent_sequence_resize(m, i, attributes);
}

//=============================================================================
// class pull_up method definitions

pull_up::pull_up() : pull_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::pull_up(const prs_expr_ptr_type& g, 
		const bool_literal& o, const char c) :
		pull_base(g, o, c) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::pull_up(const prs_expr_ptr_type& g, const bool_literal& o,
		const rule_attribute_list_type& l) :
		pull_base(g, o, l) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::~pull_up() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pull_up)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pull_up)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pull_up::dump(ostream& o, const rule_dump_context& c) const {
	return dump_base(o, c, '+');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands this rule into its complement arrow_type is COMPLEMENT.
	The call to negate will result in negation-normal form.  
	\return expanded rule if appropriate, else NULL.  
 */
excl_ptr<rule>
pull_up::expand_complement(void) {
	switch (arrow_type) {
	case ARROW_COMPLEMENT:
		arrow_type = ARROW_NORMAL;	// un-flag this rule
		return excl_ptr<rule>(
			new pull_dn(guard->negate(), output, attributes));
		// also do the same for internal_nodes?
	case ARROW_FLIP:
		arrow_type = ARROW_NORMAL;	// un-flag this rule
		return excl_ptr<rule>(
			new pull_dn(guard->flip_literals(), output, attributes));
		break;
	default: {}
	}
	return excl_ptr<rule>(NULL);
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
pull_up::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	return unroll_base(c, np, pfp, true);
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

pull_dn::pull_dn() : pull_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::pull_dn(const prs_expr_ptr_type& g, 
		const bool_literal& o, const char c) :
		pull_base(g, o, c) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::pull_dn(const prs_expr_ptr_type& g, const bool_literal& o,
		const rule_attribute_list_type& l) :
		pull_base(g, o, l) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::~pull_dn() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pull_dn)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pull_dn)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pull_dn::dump(ostream& o, const rule_dump_context& c) const {
	return dump_base(o, c, '-');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<rule>
pull_dn::expand_complement(void) {
	switch (arrow_type) {
	case ARROW_COMPLEMENT:
		arrow_type = ARROW_NORMAL;	// un-flag this rule
		return excl_ptr<rule>(
			new pull_up(guard->negate(), output, attributes));
		// also do the same for internal_nodes?
	case ARROW_FLIP:
		arrow_type = ARROW_NORMAL;	// un-flag this rule
		return excl_ptr<rule>(
			new pull_up(guard->flip_literals(), output, attributes));
		break;
	default: {}
	}
	return excl_ptr<rule>(NULL);
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
pull_dn::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	return unroll_base(c, np, pfp, false);
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

//=============================================================================
// class pass method definitions

pass::pass() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pass::~pass() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pass)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pass)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pass::dump(ostream& o, const rule_dump_context& c) const {
	return o << "PRS::pass::dump() unimplemented.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pass::check(void) const {
	STACKTRACE("pass::check()");
	// nothing to check yet
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pass-gate currently does not have a complement.  
 */
excl_ptr<rule>
pass::expand_complement(void) {
	return excl_ptr<rule>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pass::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	FINISH_ME(Fang);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pass::collect_transient_info(persistent_object_manager& m) const {
	// FINISH ME!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pass::write_object(const persistent_object_manager& m, ostream& o) const {
	// FINISH ME!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pass::load_object(const persistent_object_manager& m, istream& i) {
	// FINISH ME!
}

//=============================================================================
// class prs_expr method definitions

//=============================================================================
// class rule_conditional method definitions

rule_conditional::rule_conditional() : rule(), 
		meta_conditional_base(), clauses() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rule_conditional::~rule_conditional() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(rule_conditional)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if ALL clauses are empty.  
 */
bool
rule_conditional::empty(void) const {
	return meta_conditional_type::empty(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
rule_conditional::dump(ostream& o, const rule_dump_context& c) const {
	return meta_conditional_type::dump(*this, o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the rules in the body of guard evaluates true.  
 */
good_bool
rule_conditional::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	return meta_conditional_type::unroll(*this, c, np, pfp, "PRS");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is a static check, so every clause is visited.
 */
void
rule_conditional::check(void) const {
	typedef	clause_list_type::const_iterator	clause_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	for ( ; ci!=ce; ++ci) {
		for_each(ci->begin(), ci->end(), rule::checker());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<rule>
rule_conditional::expand_complement(void) {
	for_each(clauses.begin(), clauses.end(), 
		mem_fun_ref(&rule_set::expand_complements));
	return excl_ptr<rule>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_conditional::append_guarded_clause(const guard_ptr_type& g) {
	meta_conditional_type::append_guarded_clause(*this, g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_conditional::collect_transient_info(persistent_object_manager& m) const {
	meta_conditional_type::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_conditional::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional_type::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_conditional::load_object(const persistent_object_manager& m, istream& i) {
	meta_conditional_type::load_object(*this, m, i);
}

//=============================================================================
// class subcircuit method definitions

subcircuit::subcircuit() : nested_rules(), name() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
subcircuit::subcircuit(const string& n) : nested_rules(), name(n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
subcircuit::~subcircuit() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(subcircuit)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
subcircuit::dump(ostream& o, const rule_dump_context& c) const {
	o << "subckt <\"" << name << "\"> {" << endl;
{
	INDENT_SECTION(o);
	nested_rules::dump(o, c);
}
	return o << auto_indent << '}';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent production rules.  
	TODO: add structure
 */
good_bool
subcircuit::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	return nested_rules::unroll(c, np, pfp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subcircuit::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	nested_rules::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subcircuit::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	write_value(o, name);
	nested_rules::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subcircuit::load_object(const persistent_object_manager& m, 
		istream& i) {
	read_value(i, name);
	nested_rules::load_object_base(m, i);
}

//=============================================================================
// class rule_loop method definitions

rule_loop::rule_loop() : nested_rules(), meta_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rule_loop::rule_loop(const ind_var_ptr_type& i, 
		const range_ptr_type& r) :
		nested_rules(), meta_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rule_loop::~rule_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(rule_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
rule_loop::dump(ostream& o, const rule_dump_context& c) const {
	return meta_loop_type::dump(*this, o, c, ':');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent production rules.  
 */
good_bool
rule_loop::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	return meta_loop_type::unroll(*this, c, np, pfp, "production rule");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::collect_transient_info(persistent_object_manager& m) const {
	meta_loop_type::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	meta_loop_type::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	meta_loop_type::load_object(*this, m, i);
}

//=============================================================================
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
		const range_ptr_type& r, const prs_expr_ptr_type& e) :
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
	TODO: construct entity::expr_dump_context from PRS::expr_dump_context.
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
expr_loop_base::unroll_base(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp, const char type_enum) const {
	// STACKTRACE_VERBOSE;
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
		expr_indices.push_back(body_expr->unroll(cc, np, pfp));
	}
	PRS::footprint::expr_node&
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

//=============================================================================
// class precharge_expr method definitions

precharge_expr::~precharge_expr() { }

ostream&
precharge_expr::dump(ostream& o, const expr_dump_context& c) const {
if (expr) {
	return expr->dump(o << '{' << (dir ? '+' : '-'), c) << '}';
}
	return o;
}

void
precharge_expr::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (expr) {
		expr->collect_transient_info(m);
	}
}

void
precharge_expr::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, expr);
	write_value(o, dir);
}

void
precharge_expr::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, expr);
	read_value(i, dir);
}

//=============================================================================
// class and_expr method definitions

and_expr::and_expr() : prs_expr(), sequence_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
and_expr::and_expr(expr_sequence_type::const_reference l) : 
	prs_expr(), sequence_type() {
	sequence_type::push_back(l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
and_expr::~and_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(and_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(and_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Dump precharge expressions!
 */
ostream&
and_expr::dump(ostream& o, const expr_dump_context& c) const {
	INVARIANT(this->size() == precharge_array.size() +1 || this->empty());
	const bool paren = c.expr_stamp && (c.expr_stamp != print_stamp);
	expr_dump_context cc(c);
	cc.expr_stamp = print_stamp;
	const_iterator i(begin());
	const const_iterator e(end());
	precharge_array_type::const_iterator j(precharge_array.begin());
	NEVER_NULL(*i);
	if (paren) o << '(';
	(*i)->dump(o, cc);
	for (++i; i!=e; ++i, ++j) {
		o << " &";
		if (*j) {
			j->dump(o, c);
		}
		NEVER_NULL(*i);
		(*i)->dump(o << ' ', cc);
	}
	if (paren) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	More expensive push_front emulation, hopefully infrequent.
 */
void
and_expr::push_front(const_reference e, const precharge_type& p) {
{
	sequence_type temp;
	temp.push_back(e);
	copy(begin(), end(), back_inserter(temp));
	this->swap(temp);
}{
	precharge_array_type temp;
	temp.push_back(p);
	copy(precharge_array.begin(), precharge_array.end(),
		back_inserter(temp));
	precharge_array.swap(temp);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	More expensive push_front emulation, hopefully infrequent.
 */
void
and_expr::push_front(const_reference e) {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr::push_back(const_reference e, const precharge_type& p) {
	sequence_type::push_back(e);
	precharge_array.push_back(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr::push_back(const_reference e) {
	// first expression can't have precharge
	if (!sequence_type::empty()) {
		precharge_array.push_back(precharge_type());	// NULL
	}
	sequence_type::push_back(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr::check(void) const {
	STACKTRACE("and_expr::check()");
	for_each(begin(), end(), prs_expr::checker());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
and_expr::negate(void) const {
	STACKTRACE("and_expr::negate()");
	count_ptr<or_expr> ret(new or_expr);
	transform(begin(), end(), back_inserter(*ret), prs_expr::negater());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
and_expr::flip_literals(void) const {
	STACKTRACE("and_expr::flip_literals()");
	count_ptr<this_type> ret(new this_type);
	transform(begin(), end(), back_inserter(*ret),
		prs_expr::literal_flipper());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifies itself in-place!
	NOTE: this is also usable for and_expr_loop, and hence, 
		need not be virtual.  
 */
prs_expr_ptr_type
and_expr::negation_normalize(void) {
	STACKTRACE("and_expr::negation_normalize()");
	transform(begin(), end(), begin(),
		prs_expr::negation_normalizer(false));
	return prs_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs expressions bottom-up.  
	\return index of newly created expression if successful (1-indexed), 
		else return 0.
 */
size_t
and_expr::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	list<size_t> expr_indices;
	transform(begin(), end(), back_inserter(expr_indices), 
		prs_expr::unroller(c, np, pfp));
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(
			PRS_AND_EXPR_TYPE_ENUM, expr_indices.size()));
	copy(expr_indices.begin(), expr_indices.end(), &new_expr[1]);
	// find index of first error (1-indexed)
	const size_t err = new_expr.first_node_error();
	if (err) {
		cerr << "Error resolving production rule expression at:"
			<< endl;
#if 0
		// WTF, not working?
		const sequence_type::const_iterator
			b(sequence_type::begin());
		(b +err -1)->dump(cerr << '\t') << endl;
#endif
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
prs_expr_ptr_type
and_expr::unroll_copy(const unroll_context& c,
		const prs_expr_ptr_type& e) const {
	STACKTRACE_VERBOSE;
	INVARIANT(e == this);
	const count_ptr<this_type> ret(new this_type);
	transform(begin(), end(), back_inserter(*ret), 
		prs_expr::unroll_copier(c));
	// find index of first error (1-indexed), if any
	if (find(ret->begin(), ret->end(), prs_expr_ptr_type(NULL))
			!= ret->end()) {
		cerr << "Error resolving production rule expression: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return prs_expr_ptr_type(NULL);
	}
	if (std::equal(begin(), end(), ret->begin())) {
		return e;
	} else {
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr::collect_transient_info_base(persistent_object_manager& m) const {
	m.collect_pointer_list(*this);
	for_each(precharge_array.begin(), precharge_array.end(), 
		util::persistent_collector_ref(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr::collect_transient_info(persistent_object_manager& m) const {
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
and_expr::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer_list(o, *this);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restore precharge list from sparse array.
 */
void
and_expr::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer_list(i, *this);
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
}

//=============================================================================
// class and_expr_loop method definitions

and_expr_loop::and_expr_loop() : parent_type(), expr_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
and_expr_loop::and_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r) :
		parent_type(), expr_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
and_expr_loop::and_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r, const prs_expr_ptr_type& e) :
		parent_type(), expr_loop_base(i, r, e) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
and_expr_loop::~and_expr_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(and_expr_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	print_stamp parameter is unused, loop is always parenthesized.  
 */
ostream&
and_expr_loop::dump(ostream& o, const expr_dump_context& c) const {
	expr_dump_context cc(c);
	cc.expr_stamp = PRS_LITERAL_TYPE_ENUM;
	return expr_loop_base::dump(o, cc, '&');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr_loop::check(void) const {
	body_expr->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: update comments: we are using pointer to same induction var/expr
	What to do about induction variable and range expression?
	Need to make private copy of variable (placeholder), and 
	update references in the dependent experssions.  
	OR... can we re-use the same variable and range_expr, 
	knowing that only one expression can be evaluated at a time?
	If so, then we need to share the count_ptr.  
 */
prs_expr_ptr_type
and_expr_loop::negate(void) const {
	STACKTRACE("and_expr_loop::negate()");
	return count_ptr<or_expr_loop>(
		new or_expr_loop(ind_var, range, body_expr->negate()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
and_expr_loop::flip_literals(void) const {
	STACKTRACE("and_expr_loop::flip_literals()");
	return count_ptr<this_type>(
		new this_type(ind_var, range, body_expr->flip_literals()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifies itself in-place!
	NOTE: this is also usable for and_expr_loop, and hence, 
		need not be virtual.  
 */
prs_expr_ptr_type
and_expr_loop::negation_normalize(void) {
	STACKTRACE("and_expr_loop::negation_normalize()");
	body_expr->negation_normalize();
	return prs_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls into compile-time expanded meta expression.  
	\return index of newly created expression if successful (1-indexed), 
		else return 0.
 */
size_t
and_expr_loop::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	return expr_loop_base::unroll_base(c, np, pfp, PRS_AND_EXPR_TYPE_ENUM);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
and_expr_loop::unroll_copy(const unroll_context&, 
		const prs_expr_ptr_type&) const {
	FINISH_ME(Fang);
	return prs_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr_loop::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	expr_loop_base::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	expr_loop_base::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr_loop::load_object(const persistent_object_manager& m, istream& i) {
	expr_loop_base::load_object_base(m, i);
}

//=============================================================================
// class or_expr method definitions

or_expr::or_expr() : prs_expr(), sequence_type() { }

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
	for_each(begin(), end(), prs_expr::checker());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Detail: or-expressions don't have precharges, so the negation
	won't have any either.
 */
prs_expr_ptr_type
or_expr::negate(void) const {
	STACKTRACE("or_expr::negate()");
	const count_ptr<and_expr> ret(new and_expr);
	transform(begin(), end(), back_inserter(*ret), prs_expr::negater());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
or_expr::flip_literals(void) const {
	STACKTRACE("or_expr::flip_literals()");
	const count_ptr<this_type> ret(new this_type);
	transform(begin(), end(), back_inserter(*ret),
		prs_expr::literal_flipper());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
or_expr::negation_normalize(void) {
	STACKTRACE("or_expr::negation_normalize()");
	transform(begin(), end(), begin(), 
		prs_expr::negation_normalizer(false));
	return prs_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs expressions bottom-up.  
	\return index > 0 if successful, else 0.
 */
size_t
or_expr::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	list<size_t> expr_indices;
	transform(begin(), end(), back_inserter(expr_indices), 
		prs_expr::unroller(c, np, pfp));
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(
			PRS_OR_EXPR_TYPE_ENUM, expr_indices.size()));
	copy(expr_indices.begin(), expr_indices.end(), &new_expr[1]);
	// find index of first error (1-indexed)
	const size_t err = new_expr.first_node_error();
	if (err) {
		cerr << "Error resolving production rule expression at:"
			<< endl;
#if 0
		// WTF, not working?
		const sequence_type::const_iterator
			b(sequence_type::begin());
		(b +err -1)->dump(cerr << '\t') << endl;
#endif
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
prs_expr_ptr_type
or_expr::unroll_copy(const unroll_context& c,
		const prs_expr_ptr_type& e) const {
	STACKTRACE_VERBOSE;
	INVARIANT(e == this);
	const count_ptr<this_type> ret(new this_type);
	transform(begin(), end(), back_inserter(*ret), 
		prs_expr::unroll_copier(c));
	// find index of first error (1-indexed), if any
	if (find(ret->begin(), ret->end(), prs_expr_ptr_type(NULL))
			!= ret->end()) {
		cerr << "Error resolving production rule expression: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return prs_expr_ptr_type(NULL);
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

//=============================================================================
// class or_expr_loop method definitions

or_expr_loop::or_expr_loop() : parent_type(), expr_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
or_expr_loop::or_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r) :
		parent_type(), expr_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
or_expr_loop::or_expr_loop(const ind_var_ptr_type& i,
		const range_ptr_type& r, const prs_expr_ptr_type& e) :
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
	cc.expr_stamp = PRS_LITERAL_TYPE_ENUM;
	return expr_loop_base::dump(o, cc, '|');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
or_expr_loop::check(void) const {
	body_expr->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: update comments: we are using pointer to same induction var/expr
	What to do about induction variable and range expression?
	Need to make private copy of variable (placeholder), and 
	update references in the dependent experssions.  
	OR... can we re-use the same variable and range_expr, 
	knowing that only one expression can be evaluated at a time?
	If so, then we need to share the count_ptr.  
 */
prs_expr_ptr_type
or_expr_loop::negate(void) const {
	STACKTRACE("or_expr_loop::negate()");
	return count_ptr<and_expr_loop>(
		new and_expr_loop(ind_var, range, body_expr->negate()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
or_expr_loop::flip_literals(void) const {
	STACKTRACE("or_expr_loop::flip_literals()");
	return count_ptr<this_type>(
		new this_type(ind_var, range, body_expr->flip_literals()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifies itself in-place!
	NOTE: this is also usable for or_expr_loop, and hence, 
		need not be virtual.  
 */
prs_expr_ptr_type
or_expr_loop::negation_normalize(void) {
	STACKTRACE("or_expr_loop::negation_normalize()");
	body_expr->negation_normalize();
	return prs_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls into compile-time expanded meta expression.  
	\return index of newly created expression if successful (1-indexed), 
		else return 0.
 */
size_t
or_expr_loop::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	return expr_loop_base::unroll_base(c, np, pfp, PRS_OR_EXPR_TYPE_ENUM);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
or_expr_loop::unroll_copy(const unroll_context&, 
		const prs_expr_ptr_type&) const {
	FINISH_ME(Fang);
	return prs_expr_ptr_type(NULL);
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

//=============================================================================
// class not_expr method definitions

not_expr::not_expr() : prs_expr(), var() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
not_expr::not_expr(const prs_expr_ptr_type& g) : prs_expr(), var(g) {
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
prs_expr_ptr_type
not_expr::negate(void) const {
	STACKTRACE("not_expr::negate()");
	const prs_expr_ptr_type temp(var->negation_normalize());
	return (temp ? temp : var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
not_expr::flip_literals(void) const {
	STACKTRACE("not_expr::flip_literals()");
	const prs_expr_ptr_type temp(var->flip_literals());
	count_ptr<not_expr> nt(temp.is_a<not_expr>());
	// cancel out not-not bottom-up
	return (nt ? nt->var : prs_expr_ptr_type(new this_type(temp)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not-not reduction.  
 */
prs_expr_ptr_type
not_expr::negation_normalize(void) {
	STACKTRACE("not_expr::negation_normalize()");
	const count_ptr<not_expr> nn(var.is_a<not_expr>());
	if (UNLIKELY(nn)) {
		// not-not reduction (rare)
		return nn->negate();
	} else {
		const prs_expr_ptr_type temp(var->negation_normalize());
		if (temp)		// if changed, replace in-place
			var = temp;
		return prs_expr_ptr_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a production rule expression.  
 */
size_t
not_expr::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	const size_t expr_ind = var->unroll(c, np, pfp);
	if (!expr_ind) {
		cerr << "Error unrolling production rule expression." << endl;
		var->dump(cerr << '\t') << endl;
		return 0;
	}
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(PRS_NOT_EXPR_TYPE_ENUM, 1));
	new_expr[1] = expr_ind;
	return pfp.current_expr_index();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves into an expression with resolved local references.  
	\return copy of self resolved (may be same)
 */
prs_expr_ptr_type
not_expr::unroll_copy(const unroll_context& c,
		const prs_expr_ptr_type& e) const {
	STACKTRACE_VERBOSE;
	INVARIANT(e == this);
	const prs_expr_ptr_type arg(var->unroll_copy(c, var));
	if (!arg) {
		cerr << "Error resolving production rule expression: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return prs_expr_ptr_type(NULL);
	}
	if (arg == var) {
		return e;
	} else {
		return prs_expr_ptr_type(new this_type(arg));
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

literal::literal() : prs_expr(), base_type(), params() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
literal::literal(const bool_literal& l, const params_type& p) :
		prs_expr(), base_type(l), params(p)
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
literal::literal(const literal_base_ptr_type& l) :
		prs_expr(), base_type(l), params()
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
literal::literal(const node_literal_ptr_type& l) :
		prs_expr(), base_type(l), params() { }

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
	directive_source::dump_params(params, o, c);
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
	Could be more efficient...
 */
prs_expr_ptr_type
literal::negate(void) const {
	STACKTRACE("literal::negate()");
	const count_ptr<this_type> copy(new this_type(*this));
if (int_node) {
	copy->toggle_negate_node();	// retain everything else
	return copy;
} else {
	// is this acceptable for internal_nodes?
	return prs_expr_ptr_type(new not_expr(copy));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
literal::flip_literals(void) const {
	return negate();	// well THAT was easy
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return NULL, signaling that it is already in normal form.  
 */
prs_expr_ptr_type
literal::negation_normalize(void) {
	STACKTRACE("literal::negation_normalize()");
if (int_node) {
//	FINISH_ME_EXIT(Fang);
	return prs_expr_ptr_type(NULL);
} else {
	return prs_expr_ptr_type(NULL);
}
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
literal::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	PRS::footprint::expr_node* new_expr = NULL;
if (is_internal()) {
	const node_literal_ptr_type
		nref(unroll_node_reference(c));
	if (!nref) {
		cerr << "Error resolving internal node reference: ";
		dump(cerr, rule_dump_context()) << endl;
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
	new_expr = &(pfp.push_back_expr(PRS_NODE_TYPE_ENUM, 1));
	(*new_expr)[1] = guard_index;
} else {
	const size_t node_index = unroll_node(c);
	if (!node_index) {
		// already have error message
		return 0;
	}
	new_expr = &(pfp.push_back_expr(PRS_LITERAL_TYPE_ENUM, 1));
	(*new_expr)[1] = node_index;
}	// end if int_node
	// should attributes even apply to internal nodes?
	const size_t perr = directive_source::unroll_params(params, c,
			new_expr->get_params());
	if (perr) {
		cerr << "Error resolving rule literal parameter " << perr
			<< " in rule." << endl;
		return 0;
	}
	INVARIANT(new_expr->get_params().size() <= 3);
	// NEW [ACT]: optional 3rd parameter is transistor type
	return pfp.current_expr_index();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
literal::unroll_copy(const unroll_context& c,
		const prs_expr_ptr_type& e) const {
	INVARIANT(e == this);
	const bool_literal lref(unroll_reference(c));
	if (!lref.valid()) {
		cerr << "Error resolving prs literal: ";
		this->dump(cerr, expr_dump_context()) << endl;
		return prs_expr_ptr_type(NULL);
	}
	// copied from above
	directive_base_params_type crpar;
	literal_params_type rpar;
	const size_t perr = directive_source::unroll_params(params, c, crpar);
	if (perr) {
		cerr << "Error resolving rule literal parameter " << perr
			<< " in rule." << endl;
		return prs_expr_ptr_type(NULL);
	}
	copy(crpar.begin(), crpar.end(), back_inserter(rpar));
	if ((lref == *this) &&
		std::equal(params.begin(), params.end(), rpar.begin())) {
		return e;
	} else {
		return prs_expr_ptr_type(new literal(lref, rpar));
	}
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
	m.collect_pointer_list(params);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
literal::write_object(const persistent_object_manager& m, ostream& o) const {
//	m.write_pointer(o, var);
	write_object_base(m, o);		// saves var
	m.write_pointer_list(o, params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
literal::load_object(const persistent_object_manager& m, istream& i) {
//	m.read_pointer(i, var);
	load_object_base(m, i);			// restores var
	m.read_pointer_list(i, params);
}

//=============================================================================
// class macro method definitions

macro::macro() : rule(), directive_source() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
macro::macro(const string& n) : rule(), directive_source(n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
macro::~macro() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(macro)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
macro::dump(ostream& o, const rule_dump_context& c) const {
	return directive_source::dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Macros have no complement bit.  
 */
excl_ptr<rule>
macro::expand_complement(void) {
	return excl_ptr<rule>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolling produces an instance of the macro with the resolved nodes.  
	The behavior of the macro is defined by the programmer.  
	The behavior is customized!
	The name is used to lookup a function.  
	Future: may need an additional context/options argument.  
	TODO: can insert diagnostic macros too!  meta-programmable.
	NOTE: don't need node_pool_type.
 */
good_bool
macro::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	STACKTRACE_VERBOSE;
	// at least check the instance references first...
	PRS::footprint::macro& new_macro_call(pfp.push_back_macro(name));
	const size_t perr = unroll_params(c, new_macro_call.params);
	if (perr) {
		cerr << "Error resolving expression at position " << perr
			<< " of macro call \'" << name << "\'." << endl;
		// dump the literal?
		return good_bool(false);
	}
	const size_t nerr = unroll_nodes(c, new_macro_call.nodes);
	if (nerr) {
		cerr << "Error resolving literal node at position " << nerr
			<< " of macro call \'" << name << "\'." << endl;
		// dump the literal?
		return good_bool(false);
	}
#if BOOL_PRS_CONNECTIVITY_CHECKING
	// HACK-ish: need to treat special macros like rules
	// for connectivity checking, namely passn/passp
if (name == "passn" || name == "passp") {
	entity::footprint& tfp(c.get_target_footprint());
	state_instance<bool_tag>::pool_type&
		bp(tfp.get_instance_pool<bool_tag>());
	// see "Object/lang/PRS_macro_registry.cc" for node interpretation
	const size_t g = *new_macro_call.nodes[0].begin();
	const size_t s = *new_macro_call.nodes[1].begin();
	const size_t d = *new_macro_call.nodes[2].begin();
	const bool dir = (name == "passn") ? false : true;	// direction
	const_cast<instance_alias_info<bool_tag>&>(
		*bp[g].get_back_ref()).find()->prs_fanout(dir);
	const_cast<instance_alias_info<bool_tag>&>(
		*bp[s].get_back_ref()).find()->prs_fanout(dir);
	const_cast<instance_alias_info<bool_tag>&>(
		*bp[d].get_back_ref()).find()->prs_fanin(dir);
}
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This checking function is also user defined.  
	Make sure entry exists for this macro.  
	NOTE: no use in checking now before unroll, too limited.  
	TODO: better error handling than throw().
	NOTE: size checking can be done in parser too
 */
void
macro::check(void) const {
	assert(name.length());
	assert(nodes.size());
	// probe existence of macro
	const cflat_macro_definition_entry m(cflat_macro_registry[name]);
	// should've already been checked in the parser
	if (!m) {
		cerr << "Error: unknown PRS macro \'" << name << "\'." << endl;
		THROW_EXIT;
	}
	if (!m.check_num_params(params.size()).good) {
		// make sure passed in correct number of arguments
		// custom-defined, may be variable
		// already have error message
		THROW_EXIT;
	}
	if (!m.check_num_nodes(nodes.size()).good) {
		// make sure passed in correct number of arguments
		// custom-defined, may be variable
		// already have error message
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
macro::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
macro::write_object(const persistent_object_manager& m, ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
macro::load_object(const persistent_object_manager& m, istream& i) {
	load_object_base(m, i);
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_OBJECT_LANG_PRS_CC__

