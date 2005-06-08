/**
	\file "Object/art_object_PRS.cc"
	Implementation of PRS objects.
	$Id: art_object_PRS.cc,v 1.6.2.1 2005/06/08 19:13:18 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_CC__
#define	__OBJECT_ART_OBJECT_PRS_CC__

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE		0

#include "Object/art_object_PRS.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_classification_details.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_inst_ref_subtypes.h"

#include "util/persistent_object_manager.tcc"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"
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
using util::persistent_traits;
using util::auto_indent;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

//=============================================================================
// class rule method definitions

//=============================================================================
// class rule::dumper method definitions

template <class P>
void
rule::dumper::operator () (const P& r) {
	NEVER_NULL(r);
	r->dump(os) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
void
rule::checker::operator () (const P& r) const {
	STACKTRACE("rule::checker::operator()");
	NEVER_NULL(r);
	r->check();
}

//=============================================================================
// class prs_expr::negation_normalizer method definitions

void
prs_expr::checker::operator () (const const_prs_expr_ptr_type& e) const {
	STACKTRACE("prs_expr::checker::operator ()");
	assert(e);
	e->check();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
prs_expr_ptr_type
prs_expr::negater::operator () (const const_prs_expr_ptr_type& e) const {
	STACKTRACE("prs_expr::negater::operator ()");
	NEVER_NULL(e);
	return e->negate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return modified (normalized) expression if changed, else
		pointer to the original expression.  
 */
prs_expr_ptr_type
prs_expr::negation_normalizer::operator () (const prs_expr_ptr_type& e) const {
	STACKTRACE("prs_expr::negation_normalizer::operator ()");
	NEVER_NULL(e);
	if (nb) {
		return e->negate();
	} else {
		const prs_expr_ptr_type ret(e->negation_normalize());
		return (ret ? ret : e);
	}
}

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
	iterator i = begin();
	const iterator e = end();
	for ( ; i!=e; i++) {
		excl_ptr<rule> cmpl = (*i)->expand_complement();
		if (cmpl) {
			*insert(i, value_type()) = cmpl;
			INVARIANT(!cmpl);	// transferred ownership
		}
	}
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
	static const char* norm_arrow = " -> ";
	static const char* comp_arrow = " => ";
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
	const_iterator i = begin();
	const const_iterator e = end();
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
	const_iterator i = begin();
	const const_iterator e = end();
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
	count_ptr<and_expr> ret(new and_expr);
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

#endif	// __OBJECT_ART_OBJECT_PRS_CC__

