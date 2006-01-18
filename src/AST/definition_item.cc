/**
	\file "AST/namespace.cc"
	Class method definitions for namespace and other root item classes.
	$Id: definition_item.cc,v 1.2.2.1 2006/01/18 06:24:48 fang Exp $
 */

#ifndef	__HAC_AST_DEFINITION_ITEM_CC__
#define	__HAC_AST_DEFINITION_ITEM_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>

#include "AST/definition_item.h"
#include "AST/node_list.tcc"
#include "AST/parse_context.h"

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
SPECIALIZE_UTIL_WHAT(HAC::parser::def_body_item, "(def-body-item)")
}

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;

//=============================================================================
// class def_body_item method definitions

//=============================================================================
// class definition_body method definitions

definition_body::definition_body() : parent() { }

definition_body::definition_body(const def_body_item* d) : parent(d) { }

definition_body::~definition_body() { }

//=============================================================================
// class language_body methd definitions

CONSTRUCTOR_INLINE
language_body::language_body(const generic_keyword_type* t) :
		def_body_item(), tag(t) {
}

DESTRUCTOR_INLINE
language_body::~language_body() { }

language_body*
language_body::attach_tag(generic_keyword_type* t) {
	// need to safe-delete first?  nah...
	tag = excl_ptr<const generic_keyword_type>(t);
	NEVER_NULL(tag);
	return this;
}

line_position
language_body::leftmost(void) const {
	// what if untagged?
	NEVER_NULL(tag);
	return tag->leftmost();
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_DEFINITION_ITEM_CC__

