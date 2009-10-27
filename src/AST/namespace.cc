/**
	\file "AST/namespace.cc"
	Class method definitions for namespace and other root item classes.
	$Id: namespace.cc,v 1.6 2009/10/27 18:21:43 fang Exp $
 */

#ifndef	__HAC_AST_NAMESPACE_CC__
#define	__HAC_AST_NAMESPACE_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>

#include "AST/token.h"
#include "AST/namespace.h"
#include "AST/identifier.h"
#include "AST/node_list.tcc"
#include "AST/parse_context.h"

#include "Object/common/namespace.h"

#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::root_item, "(root_item)")
SPECIALIZE_UTIL_WHAT(HAC::parser::namespace_body, "namespace-body")
SPECIALIZE_UTIL_WHAT(HAC::parser::namespace_id, "(namespace-id)")
SPECIALIZE_UTIL_WHAT(HAC::parser::using_namespace, "(using-namespace)")

namespace memory {
// explicit template instantiations
using namespace HAC::parser;
template class count_ptr<const root_item>;
template class count_ptr<root_body>;
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;

//=============================================================================
// class root_item method definitions

//-----------------------------------------------------------------------------
// class root_body method definitions

root_body::root_body() : parent() { }

root_body::root_body(const root_item* r) : parent(r) { }

root_body::~root_body() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
root_body::check_build(context& c) const {
	const never_ptr<const object> ret(parent::check_build(c));
	const error_policy p(c.parse_opts.case_collision_policy);
	// check for case collisions at the end instead of on-the-fly
if (p != OPTION_IGNORE) {
	const bool e =
		c.get_current_named_scope()->check_case_collisions(cerr);
	if (e) {
		cerr << "Warnings found in top-level namespace "
			<< where(*this) << endl;
	if (p == OPTION_ERROR) {
		cerr << "Promoting warning to error." << endl;
		THROW_EXIT;
	}
	}
}
	return ret;
}

//=============================================================================
// class namespace_body method definitions

/**
	Namespace body constructor.  
	\param s the "namespace" keyword. 
	\param n the identifier for the name of the namespace.  
	\param l the left brace.  
	\param b the body (contents), may be NULL.  
	\param r the right brace.  
 */
CONSTRUCTOR_INLINE
namespace_body::namespace_body(
		const generic_keyword_type* s, const token_identifier* n, 
		const root_body* b) :
		root_item(),       
		ns(s), name(n), body(b) {
	NEVER_NULL(ns); NEVER_NULL(name);
	// body may be NULL
//	NEVER_NULL(semi);	// don't really care about syntax sugar
}

/// destructor
DESTRUCTOR_INLINE
namespace_body::~namespace_body() {
}

/// what eeeez it, man?
ostream&
namespace_body::what(ostream& o) const {
	return o << '(' << util::what<namespace_body>::name() <<
		": " << *name << ')';
}

line_position
namespace_body::leftmost(void) const {
	if (ns)		return ns->leftmost();
	else		return name->rightmost();
}

line_position
namespace_body::rightmost(void) const {
	if (body)	return body->rightmost();
	else		return name->rightmost();
}

// recursive type-checker
never_ptr<const object>
namespace_body::check_build(context& c) const {
	STACKTRACE("namespace_body::check_build()");
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c.lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context
{
	const context::namespace_frame _nf(c, *name);
	// if there was error, would've exited...
	if (body)			// may be NULL, which means empty
		body->check_build(c);
	const error_policy p(c.parse_opts.case_collision_policy);
	// check for case collisions at the end instead of on-the-fly
if (p != OPTION_IGNORE) {
	const bool e =
		c.get_current_named_scope()->check_case_collisions(cerr);
	if (e) {
		cerr << "Warnings found in namespace `" <<
			*name << "\' in " << where(*body) << endl;
	if (p == OPTION_ERROR) {
		cerr << "Promoting warning to error." << endl;
		THROW_EXIT;
	}
	}
}
}
	// if no errors, return pointer to the namespace just processed
	return c.top_namespace();
}

//=============================================================================
// class namespace_id method definitions

namespace_id::namespace_id(qualified_id* i) : qid(i) {
	NEVER_NULL(qid);
}

namespace_id::~namespace_id() {
}

ostream&
namespace_id::what(ostream& o) const {
	return o << util::what<namespace_id>::name() << ": " << *qid;
}

line_position
namespace_id::leftmost(void) const {
	return qid->leftmost();
}

line_position
namespace_id::rightmost(void) const {
	return qid->rightmost();
}

qualified_id*
namespace_id::force_absolute(const string_punctuation_type* s) {
	return qid->force_absolute(s);
}

bool
namespace_id::is_absolute(void) const {
	return qid->is_absolute();
}

/*** NOT USED... yet
never_ptr<const object>
namespace_id::check_build(context& c) const {
}
***/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions

ostream&
operator << (ostream& o, const namespace_id& id) {
	return o << *id.qid;
}

//=============================================================================
// class using_namespace method definitions

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
 */
CONSTRUCTOR_INLINE
using_namespace::using_namespace(
		const generic_keyword_type* o, const namespace_id* i) :
		root_item(), open(o), id(i), alias(NULL) {
	NEVER_NULL(open); NEVER_NULL(id);
}

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
	\param a the "as" keyword.  
	\param n the alias name.  
 */
CONSTRUCTOR_INLINE
using_namespace::using_namespace(
		const generic_keyword_type* o, const namespace_id* i, 
		const token_identifier* n) :
		root_item(), open(o), id(i), alias(n) {
	NEVER_NULL(open); NEVER_NULL(id); NEVER_NULL(alias);
}

/// default destructor
DESTRUCTOR_INLINE
using_namespace::~using_namespace() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(using_namespace)

line_position
using_namespace::leftmost(void) const {
	return open->leftmost();
}

line_position
using_namespace::rightmost(void) const {
	if (alias)	return alias->rightmost();
	else		return id->rightmost();
}

/// returns a pointer to a valid namespace that's now mapped in this scope
never_ptr<const object>
using_namespace::check_build(context& c) const {
	STACKTRACE("using_namespace::check_build()");
if (alias) {
	// (alias)
	c.alias_namespace(*id->get_id(), *alias);
} else {
	// (using)
	// if aliased... print all, report as error (done inside)
	c.using_namespace(*id->get_id());
}
	return c.top_namespace();
}

//=============================================================================
template class node_list<const root_item>;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_NAMESPACE_CC__

