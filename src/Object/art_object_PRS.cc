/**
	\file "Object/art_object_PRS.cc"
	Implementation of PRS objects.
	$Id: art_object_PRS.cc,v 1.1.2.4 2005/05/16 21:43:42 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_CC__
#define	__OBJECT_ART_OBJECT_PRS_CC__

#include "Object/art_object_PRS.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_classification_details.h"
#include "Object/art_object_type_hash.h"

#include "util/persistent_object_manager.tcc"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/memory/chunk_map_pool.tcc"

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

//=============================================================================
// class prs_expr::negation_normalizer method definitions

#if 0
excl_ptr<prs_expr>
prs_expr::negation_normalizer::operator () (
		const sticky_ptr<prs_expr>& e) const {
	return e->negation_normalize();
}
#endif

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
pull_up::pull_up(guard_arg_type& g, const literal& o, const bool c) :
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
excl_ptr<rule>
pull_up::complement(void) const {
	return excl_ptr<rule>(NULL);
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
pull_dn::pull_dn(guard_arg_type& g, const literal& o, const bool c) :
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
excl_ptr<rule>
pull_dn::complement(void) const {
	return excl_ptr<rule>(NULL);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(pass)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<rule>
pass::complement(void) const {
	return excl_ptr<rule>(NULL);
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
and_expr::dump(ostream& o) const {
	const_iterator i = begin();
	const const_iterator e = end();
	NEVER_NULL(*i);
	(*i)->dump(o << '(');
	for (i++; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->dump(o << " & ");
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<prs_expr>
and_expr::negation_normalize(void) const {
#if 0
	excl_ptr<or_expr> ret(new or_expr);
	transform(begin(), end(), back_inserter(*ret), 
		prs_expr::negation_normalizer()
	);
	return ret.as_a_xfer<prs_expr>;
#else
	return excl_ptr<prs_expr>(NULL);
#endif
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
or_expr::dump(ostream& o) const {
	const_iterator i = begin();
	const const_iterator e = end();
	NEVER_NULL(*i);
	(*i)->dump(o << '(');
	for (i++; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->dump(o << " | ");
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<prs_expr>
or_expr::negation_normalize(void) const {
	return excl_ptr<prs_expr>(NULL);
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
not_expr::not_expr(guard_arg_type& g) : prs_expr(), var(g) {
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
not_expr::dump(ostream& o) const {
	return var->dump(o << "~");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<prs_expr>
not_expr::negation_normalize(void) const {
	return excl_ptr<prs_expr>(NULL);
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
literal::dump(ostream& o) const {
	return var->dump_briefer(o, never_ptr<const scopespace>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<prs_expr>
literal::negation_normalize(void) const {
	return excl_ptr<prs_expr>(NULL);
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

#endif	// __OBJECT_ART_OBJECT_PRS_CC__

