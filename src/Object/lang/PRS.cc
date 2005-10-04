/**
	\file "Object/lang/PRS.cc"
	Implementation of PRS objects.
	$Id: PRS.cc,v 1.2.16.1 2005/10/04 21:24:24 fang Exp $
 */

#ifndef	__OBJECT_LANG_PRS_CC__
#define	__OBJECT_LANG_PRS_CC__

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE		0

#include "Object/lang/PRS.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/traits/bool_traits.h"
#include "Object/expr/bool_expr.h"
#include "Object/persistent_type_hash.h"

#include "common/TODO.h"
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
SPECIALIZE_UTIL_WHAT(ART::entity::PRS::pull_up, "PRS-up")
SPECIALIZE_UTIL_WHAT(ART::entity::PRS::pull_dn, "PRS-dn")
SPECIALIZE_UTIL_WHAT(ART::entity::PRS::and_expr, "PRS-and")
SPECIALIZE_UTIL_WHAT(ART::entity::PRS::or_expr, "PRS-or")
SPECIALIZE_UTIL_WHAT(ART::entity::PRS::not_expr, "PRS-not")
SPECIALIZE_UTIL_WHAT(ART::entity::PRS::literal, "PRS-var")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::PRS::pull_up, PRS_PULLUP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::PRS::pull_dn, PRS_PULLDN_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::PRS::and_expr, PRS_AND_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::PRS::or_expr, PRS_OR_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::PRS::not_expr, PRS_NOT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::PRS::literal, PRS_LITERAL_TYPE_KEY, 0)
}	// end namespace util

namespace ART {
namespace entity {
namespace PRS {
using std::copy;
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
	ostream& os;
	dumper(ostream& o) : os(o) { }

	template <class P>
	void
	operator () (const P& r) {
		NEVER_NULL(r);
		r->dump(os) << endl;
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
rule_set::dump(ostream& o) const {
	for_each(begin(), end(), rule::dumper(o));
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
// class pull_up method definitions

pull_up::pull_up() : rule(), guard(), output(), cmpl(false) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::pull_up(const prs_expr_ptr_type& g, const literal& o, const bool c) :
		rule(), guard(g), output(o), cmpl(c) {
	NEVER_NULL(guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_up::~pull_up() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pull_up)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pull_up)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pull_up::dump(ostream& o) const {
	static const char* const norm_arrow = " -> ";
	static const char* const comp_arrow = " => ";
	return output.dump(
		guard->dump(o << auto_indent) <<
			((cmpl) ? comp_arrow : norm_arrow)) << "+";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_up::check(void) const {
	STACKTRACE("pull_up::check()");
	assert(guard);
	output.check();
	guard->check();
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
			new pull_dn(guard->negate(), output, false));
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
	size_t guard_expr_index = guard->unroll(c, np, pfp);
	if (!guard_expr_index) {
		this->dump(cerr << "Error unrolling production rule: "
			<< endl << '\t') << endl;
		// dump context too?
		return good_bool(false);
	}
	typedef literal_base_ptr_type::element_type::alias_collection_type
			bool_instance_alias_collection_type;
	bool_instance_alias_collection_type bc;
	if (output.get_bool_var()->unroll_references(c, bc).bad) {
		output.dump(cerr <<
			"Error resolving output node of production rule: ")
			<< endl;
		return good_bool(false);
	}
	INVARIANT(!bc.dimensions());		// must be scalar
	const instance_alias_info<bool_tag>& bi(*bc.front());
	const size_t output_node_index = bi.instance_index;
	INVARIANT(output_node_index);
	pfp.push_back_rule(guard_expr_index, output_node_index, true);	// up
	// check auto-complement, and unroll it
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_up::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	guard->collect_transient_info(m);
	output.collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_up::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, guard);
//	m.write_pointer(o, output);
	output.write_object(m, o);
	write_value(o, cmpl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_up::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, guard);
//	m.read_pointer(i, output);
	output.load_object(m, i);
	read_value(i, cmpl);
}

//=============================================================================
// class pull_dn method definitions

pull_dn::pull_dn() : rule(), guard(), output(), cmpl(false) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::pull_dn(const prs_expr_ptr_type& g, const literal& o, const bool c) :
		rule(), guard(g), output(o), cmpl(c) {
	NEVER_NULL(guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pull_dn::~pull_dn() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pull_dn)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pull_dn)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pull_dn::dump(ostream& o) const {
	static const char* norm_arrow = " -> ";
	static const char* comp_arrow = " => ";
	return output.dump(
		guard->dump(o << auto_indent) <<
			((cmpl) ? comp_arrow : norm_arrow)) << "-";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_dn::check(void) const {
	STACKTRACE("pull_dn::check()");
	assert(guard);
	output.check();
	guard->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<rule>
pull_dn::expand_complement(void) {
	if (cmpl) {
		cmpl = false;
		return excl_ptr<rule>(
			new pull_up(guard->negate(), output, false));
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
	size_t guard_expr_index = guard->unroll(c, np, pfp);
	if (!guard_expr_index) {
		this->dump(cerr << "Error unrolling production rule: "
			<< endl << '\t') << endl;
		// dump context too?
		return good_bool(false);
	}
	typedef literal_base_ptr_type::element_type::alias_collection_type
			bool_instance_alias_collection_type;
	bool_instance_alias_collection_type bc;
	if (output.get_bool_var()->unroll_references(c, bc).bad) {
		output.dump(cerr <<
			"Error resolving output node of production rule: ")
			<< endl;
		return good_bool(false);
	}
	INVARIANT(!bc.dimensions());		// must be scalar
	const instance_alias_info<bool_tag>& bi(*bc.front());
	const size_t output_node_index = bi.instance_index;
	INVARIANT(output_node_index);
	pfp.push_back_rule(guard_expr_index, output_node_index, false);	// down
	// check auto-complement, and unroll it
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_dn::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	guard->collect_transient_info(m);
	output.collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_dn::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, guard);
	output.write_object(m, o);
//	m.write_pointer(o, output);
	write_value(o, cmpl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pull_dn::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, guard);
	output.load_object(m, i);
//	m.read_pointer(i, output);
	read_value(i, cmpl);
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
pass::dump(ostream& o) const {
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
and_expr::dump(ostream& o, const int stamp) const {
	const bool paren = stamp && (stamp != print_stamp);
	const_iterator i(begin());
	const const_iterator e(end());
	NEVER_NULL(*i);
	if (paren) o << '(';
	(*i)->dump(o, print_stamp);
	for (i++; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->dump(o << " & ", print_stamp);
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
	\return index > 0 if successful, else 0.
 */
size_t
and_expr::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	list<size_t> expr_indices;
	transform(begin(), end(), back_inserter(expr_indices), 
		prs_expr::unroller(c, np, pfp));
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(
			PRS_AND_EXPR_TYPE_ENUM, expr_indices.size()));
	copy(expr_indices.begin(), expr_indices.end(), &new_expr[1]);
	// find index of first error
	const size_t err = new_expr.first_error();
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
and_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(*this);
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
or_expr::dump(ostream& o, const int stamp) const {
	const bool paren = stamp && (stamp != print_stamp);
	const_iterator i(begin());
	const const_iterator e(end());
	NEVER_NULL(*i);
	if (paren) o << '(';
	(*i)->dump(o, print_stamp);
	for (i++; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->dump(o << " | ", print_stamp);
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
	list<size_t> expr_indices;
	transform(begin(), end(), back_inserter(expr_indices), 
		prs_expr::unroller(c, np, pfp));
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(
			PRS_OR_EXPR_TYPE_ENUM, expr_indices.size()));
	copy(expr_indices.begin(), expr_indices.end(), &new_expr[1]);
	// find index of first error
	const size_t err = new_expr.first_error();
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
or_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(*this);
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
not_expr::dump(ostream& o, const int) const {
	// never needs parentheses
	return var->dump(o << "~", print_stamp);
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

literal::literal() : prs_expr(), var() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
literal::literal(const literal_base_ptr_type& l) : prs_expr(), var(l) {
	NEVER_NULL(var);
}

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
literal::dump(ostream& o, const int) const {
	// never needs parentheses
	return var->dump_briefer(o, never_ptr<const scopespace>());
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
		prs_expr_ptr_type(new literal(var))));
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
	TODO: factor out common code.
	Has code in common with pull_up/pull_dn...
 */
size_t
literal::unroll(const unroll_context& c, const node_pool_type& np, 
		PRS::footprint& pfp) const {
	typedef literal_base_ptr_type::element_type::alias_collection_type
			bool_instance_alias_collection_type;
	bool_instance_alias_collection_type bc;
	if (var->unroll_references(c, bc).bad) {
		var->dump(cerr << "Error resolving production rule literal: ")
			<< endl;
		return 0;
	}
	INVARIANT(!bc.dimensions());		// must be scalar
	const instance_alias_info<bool_tag>& bi(*bc.front());
	const size_t node_index = bi.instance_index;
	INVARIANT(node_index);
	PRS::footprint::expr_node&
		new_expr(pfp.push_back_expr(PRS_LITERAL_TYPE_ENUM, 1));
	new_expr[1] = node_index;
	return pfp.current_expr_index();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Doesn't register itself as a dynamically allocated object.  
 */
void
literal::collect_transient_info_base(persistent_object_manager& m) const {
	var->collect_transient_info(m);
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
	m.write_pointer(o, var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
literal::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, var);
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_LANG_PRS_CC__

