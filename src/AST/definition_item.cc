/**
	\file "AST/namespace.cc"
	Class method definitions for namespace and other root item classes.
	$Id: definition_item.cc,v 1.7 2007/11/26 08:27:23 fang Exp $
 */

#ifndef	__HAC_AST_DEFINITION_ITEM_CC__
#define	__HAC_AST_DEFINITION_ITEM_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>

#include "AST/definition_item.hh"
#include "AST/lang.hh"
#include "AST/node_list.tcc"
#include "AST/parse_context.hh"

#include "util/indent.hh"
#include "util/what.hh"
#include "util/stacktrace.hh"
#include "util/memory/count_ptr.tcc"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT_DEFINITION(HAC::parser::def_body_item, "(def-body-item)")

namespace memory {
// explicit template instantiations
template class count_ptr<const HAC::parser::def_body_item>;
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.hh"
using util::indent;
using util::auto_indent;

//=============================================================================
// class def_body_item method definitions

//=============================================================================
// class language_body methd definitions

CONSTRUCTOR_INLINE
language_body::language_body(const generic_keyword_type* t) :
#if INSTANCE_MANAGEMENT_LANG
		instance_management(), 
#else
		def_body_item(), 
#endif
		tag(t) {
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

