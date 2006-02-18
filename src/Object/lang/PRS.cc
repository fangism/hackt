/**
	\file "Object/lang/PRS.cc"
	Implementation of PRS objects.
	$Id: PRS.cc,v 1.12.4.1 2006/02/18 21:47:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_CC__
#define	__HAC_OBJECT_LANG_PRS_CC__

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE		0

#include "Object/lang/PRS.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/PRS_macro_registry.h"
#include "Object/lang/PRS_literal_unroller.h"

#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/traits/bool_traits.h"
// #include "Object/inst/alias_empty.h"
// #include "Object/inst/instance_alias_info.h"

#include "Object/expr/bool_expr.h"
#include "Object/expr/const_param.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/def/template_formals_manager.h"
#include "Object/type/template_actuals.h"
#include "Object/unroll/unroll_context.h"
#include "Object/persistent_type_hash.h"

#include "common/TODO.h"
#include "util/persistent_functor.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"
#include "util/packed_array.h"	// for bool_alias_collection_type
#include "util/likely.h"
#include "util/stacktrace.h"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::pull_up, "PRS-up")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::pull_dn, "PRS-dn")
SPECIALIZE_UTIL_WHAT(HAC::entity::PRS::rule_loop, "PRS-loop")
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
};	// end struct negation_normalizer

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

//=============================================================================
// class rule_set method definitions

rule_set::rule_set() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rule_set::~rule_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
rule_set::dump(ostream& o, const rule_dump_context& c) const {
	for_each(begin(), end(), rule::dumper(o, c));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::compact_references(void) {
	cerr << "Fang, write PRS::rule_set::compact_references()!" << endl;
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
rule_set::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_set::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, *this);
}

//=============================================================================
// class attribute method definitions

attribute::attribute() : key(), values() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
attribute::attribute(const string& k) : key(k), values() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
attribute::~attribute() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
attribute::operator bool () const {
	return key.length();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
attribute::push_back(const value_type& e) {
	if (!values) {
		values = count_ptr<values_type>(new values_type);
		NEVER_NULL(values);
	}
	values->push_back(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
attribute::dump(ostream& o, const rule_dump_context& c) const {
	o << key << '=';
	NEVER_NULL(values);
	return values->dump(o, entity::expr_dump_context(c));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param_expr_list>
attribute::unroll_values(const unroll_context& c) const {
	NEVER_NULL(values);
	const count_ptr<const const_param_expr_list>
		ret(values->unroll_resolve(c));
	if (!ret) {
		cerr << "Error resolving attribute values!" << endl;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does NOT register itself, because called by reference, not pointer.  
 */
void
attribute::collect_transient_info_base(persistent_object_manager& m) const {
	if (values)
		values->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
attribute::write_object(const persistent_object_manager& m, ostream& o) const {
	INVARIANT(key.length());
	write_value(o, key);
	m.write_pointer(o, values);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
attribute::load_object(const persistent_object_manager& m, istream& i) {
	read_value(i, key);
	INVARIANT(key.length());
	m.read_pointer(i, values);
}

//=============================================================================
// class pull_base method definitions

pull_base::pull_base() : rule(), guard(), output(), cmpl(false), 
	attributes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_base::pull_base(const prs_expr_ptr_type& g, 
		const bool_literal& o, const bool c) :
		rule(), guard(g), output(o), cmpl(c), attributes() {
	NEVER_NULL(guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_base::pull_base(const prs_expr_ptr_type& g, 
		const bool_literal& o, const attribute_list_type& l) :
		rule(), guard(g), output(o), cmpl(false), attributes(l) {
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
	output.dump(
		guard->dump(o, c) <<
			((cmpl) ? comp_arrow : norm_arrow), c) << dir;
	if (!attributes.empty()) {
		o << " [";
		typedef	attribute_list_type::const_iterator	const_iterator;
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
	TODO: process attributes
 */
good_bool
pull_base::unroll_base(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp, const bool dir) const {
	STACKTRACE_VERBOSE;
	size_t guard_expr_index = guard->unroll(c, np, pfp);
	if (!guard_expr_index) {
		this->dump(cerr << "Error unrolling production rule: "
			<< endl << '\t', rule_dump_context()) << endl;
		// dump context too?
		return good_bool(false);
	}
	const size_t output_node_index = output.unroll_base(c);
	if (!output_node_index) {
		output.dump(cerr <<
			"Error resolving output node of production rule: ", 
			rule_dump_context())
			<< endl;
		return good_bool(false);
	}
	// check for auto-complement, and unroll it?
	footprint_rule&
		r(pfp.push_back_rule(guard_expr_index, output_node_index, dir));
{
	typedef	attribute_list_type::const_iterator	const_iterator;
	const_iterator i(attributes.begin());
	const const_iterator e(attributes.end());
	for ( ; i!=e; ++i) {
		const string& key(i->get_key());
		// check whether or not named attribute is registered
		const attribute_definition_entry att(attribute_registry[key]);
		if (!att) {
			cerr << "Error: unrecognized attribute \'" << key <<
				"\'." << endl;
			return good_bool(false);
		}
		const count_ptr<const const_param_expr_list>
			att_vals(i->unroll_values(c));
		if (!att_vals) {
			// already have minimal error message
			return good_bool(false);
		}
		if (!att.check_values(*att_vals).good) {
			// already have error message
			return good_bool(false);
		}
		r.push_back(footprint_rule_attribute(key, att_vals));
	}
}
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
	write_value(o, cmpl);
	util::write_persistent_sequence(m, o, attributes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_base::load_object_base(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, guard);
	output.load_object_base(m, i);
	read_value(i, cmpl);
	util::read_persistent_sequence_resize(m, i, attributes);
}

//=============================================================================
// class pull_up method definitions

pull_up::pull_up() : pull_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::pull_up(const prs_expr_ptr_type& g, 
		const bool_literal& o, const bool c) :
		pull_base(g, o, c) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::pull_up(const prs_expr_ptr_type& g, const bool_literal& o,
		const attribute_list_type& l) :
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
	Expands this rule into its complement if the cmpl bit is set.  
	The call to negate will result in negation-normal form.  
	\return complement rule if the cmple bit is set, else NULL.  
 */
excl_ptr<rule>
pull_up::expand_complement(void) {
	if (cmpl) {
		cmpl = false;
		return excl_ptr<rule>(
			new pull_dn(guard->negate(), output, attributes));
	} else	return excl_ptr<rule>(NULL);
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
		const bool_literal& o, const bool c) :
		pull_base(g, o, c) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::pull_dn(const prs_expr_ptr_type& g, const bool_literal& o,
		const attribute_list_type& l) :
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
	if (cmpl) {
		cmpl = false;
		return excl_ptr<rule>(
			new pull_up(guard->negate(), output, attributes));
	} else	return excl_ptr<rule>(NULL);
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
// class rule_loop method definitions

rule_loop::rule_loop() : rule(), meta_loop_base(), rules() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rule_loop::rule_loop(const ind_var_ptr_type& i, 
		const range_ptr_type& r) :
		rule(), meta_loop_base(i, r), rules() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rule_loop::~rule_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(rule_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
rule_loop::dump(ostream& o, const rule_dump_context& c) const {
	NEVER_NULL(ind_var);
	NEVER_NULL(range);
	o << "(:" << ind_var->get_name() << ':';
	range->dump(o, entity::expr_dump_context(c)) << ':' << endl;
	{
		INDENT_SECTION(o);
		rules.dump(o, c);
	}
	return o << auto_indent << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::check(void) const {
	for_each(rules.begin(), rules.end(), rule::checker());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<rule>
rule_loop::expand_complement(void) {
	rules.expand_complements();
	return excl_ptr<rule>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::push_back(excl_ptr<rule>& r) {
	NEVER_NULL(r);
	rules.push_back(value_type());
	rules.back() = r;
	MUST_BE_NULL(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent production rules.  
 */
good_bool
rule_loop::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
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
	template_formals_manager tfm;
	const never_ptr<const pint_scalar> pvc(&*ind_var);
	tfm.add_strict_template_formal(pvc);

	const count_ptr<pint_const> ind(new pint_const(min));
	const count_ptr<const_param_expr_list> al(new const_param_expr_list);
	NEVER_NULL(al);
	al->push_back(ind);
	const template_actuals::const_arg_list_ptr_type sl(NULL);
	const template_actuals ta(al, sl);
	unroll_context cc(ta, tfm);
	cc.chain_context(c);
	pint_value_type ind_val = min;
	for ( ; ind_val <= max; ind_val++) {
		*ind = ind_val;
		if (!rules.unroll(cc, np, pfp).good) {
			cerr << "Error resolving production rule in loop:"
				<< endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	rules.collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	meta_loop_base::write_object_base(m, o);
	rules.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
rule_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	meta_loop_base::load_object_base(m, i);
	rules.load_object_base(m, i);
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
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
#if 0
	INVARIANT(min <= max);
#else
	if (min > max) {
		cerr << "Sorry, empty expression loops are not yet supported.  "
			"Currently waiting for resolution on language "
			"defect report." << endl;
		return 0;
	}
#endif
	// range gives us upper and lower bound of loop
	// in a loop:
	// create context chain of lookup
	//	using unroll_context's template_formal/actual mechanism.  
	template_formals_manager tfm;
	const never_ptr<const pint_scalar> pvc(&*ind_var);
	tfm.add_strict_template_formal(pvc);

	const count_ptr<pint_const> ind(new pint_const(min));
	const count_ptr<const_param_expr_list> al(new const_param_expr_list);
	NEVER_NULL(al);
	al->push_back(ind);
	const template_actuals::const_arg_list_ptr_type sl(NULL);
	const template_actuals ta(al, sl);
	unroll_context cc(ta, tfm);
	cc.chain_context(c);
	pint_value_type ind_val = min;
	list<size_t> expr_indices;
	for ( ; ind_val <= max; ind_val++) {
		*ind = ind_val;
		expr_indices.push_back(body_expr->unroll(cc, np, pfp));
		// check for errors after loop
	}
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(type_enum, expr_indices.size()));
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
// class and_expr method definitions

and_expr::and_expr() : prs_expr(), sequence_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
and_expr::~and_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(and_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(and_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
and_expr::dump(ostream& o, const expr_dump_context& c) const {
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
		(*i)->dump(o << " & ", cc);
	}
	if (paren) o << ')';
	return o;
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
void
and_expr::collect_transient_info_base(persistent_object_manager& m) const {
	m.collect_pointer_list(*this);
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
void
and_expr::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer_list(o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
and_expr::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer_list(i, *this);
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
prs_expr_ptr_type
or_expr::negate(void) const {
	STACKTRACE("or_expr::negate()");
	const count_ptr<and_expr> ret(new and_expr);
	transform(begin(), end(), back_inserter(*ret), prs_expr::negater());
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
literal::literal(const literal_base_ptr_type& l) :
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
	assert(var);
	// var->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Could be more efficient...
 */
prs_expr_ptr_type
literal::negate(void) const {
	STACKTRACE("literal::negate()");
	return prs_expr_ptr_type(new not_expr(
		prs_expr_ptr_type(new literal(*this))));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return NULL, signaling that it is already in normal form.  
 */
prs_expr_ptr_type
literal::negation_normalize(void) {
	STACKTRACE("literal::negation_normalize()");
	return prs_expr_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper around unroll_base that provide error message.  
	\return index of the node referenced, local to this definition only, 
		NOT the globally allocated one.  
 */
size_t
literal::unroll_node(const unroll_context& c) const {
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
	TODO: check parameters!
	Has code in common with pull_up/pull_dn...
	\return the index of the new expression represented by this
		literal reference, else 0 if error occurs.  
 */
size_t
literal::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	const size_t node_index = unroll_node(c);
	if (!node_index) {
		// already have error message
		return 0;
	}
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(PRS_LITERAL_TYPE_ENUM, 1));
	new_expr[1] = node_index;
	const size_t perr = directive_source::unroll_params(params, c,
			new_expr.get_params());
	if (perr) {
		cerr << "Error resolving rule literal parameter " << perr
			<< " in rule." << endl;
		return 0;
	}
	INVARIANT(new_expr.get_params().size() <= 2);
	return pfp.current_expr_index();
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
	write_object_base(m, o);
	m.write_pointer_list(o, params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
literal::load_object(const persistent_object_manager& m, istream& i) {
//	m.read_pointer(i, var);
	load_object_base(m, i);
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
	const macro_definition_entry m(macro_registry[name]);
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

