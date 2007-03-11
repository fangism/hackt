/**
	\file "AST/import_root.cc"
	$Id: import_root.cc,v 1.6 2007/03/11 16:34:15 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_DTORS			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include "AST/import_root.h"
#include "AST/node_list.tcc"
#include "AST/parse_context.h"
#include "AST/token.h"
#include "AST/token_string.h"
#include "util/memory/count_ptr.tcc"
#include "Object/common/namespace.h"
#include "common/TODO.h"
#include "util/what.h"
#include "util/stacktrace.h"

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::imported_root, "(imported-root)")
SPECIALIZE_UTIL_WHAT(HAC::parser::imported_root_list, "(imported-root-list)")

namespace memory {
// explicit template instantiation
template class count_ptr<const HAC::parser::imported_root>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace parser {
#include "util/using_ostream.h"
//=============================================================================
// class imported_root method definitions

imported_root::imported_root(excl_ptr<root_body>& r,
		excl_ptr<const keyword_position>& k,
		excl_ptr<const token_quoted_string>& f, 
		const string& n,
		const bool s) :
		root(r),
		import(k), rel_file(f),
		name(n), seen(s) {
	NEVER_NULL(import);
	NEVER_NULL(rel_file);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
imported_root::~imported_root() {
	STACKTRACE_DTOR_VERBOSE;
#if STACKTRACE_DTORS
	STACKTRACE_INDENT_PRINT("at " << this << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(imported_root)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
imported_root::string_compare(const char*) const {
	FINISH_ME_EXIT(Fang);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
imported_root::leftmost(void) const {
	return import->leftmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
imported_root::rightmost(void) const {
	return rel_file->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Exception error handling is temporary until I have more time.  
 */
never_ptr<const object>
imported_root::check_build(context& c) const {
	if (!seen) {
		// push the file name onto context stack for diagnosis
		NEVER_NULL(root);
		const context::file_stack_frame _fsf(c, name);
		try {
			return root->check_build(c);
		} catch (...) {
			cerr << "From: " << '\"' << name << '\"' << endl;
			throw;
		}
	} else {
		return c.top_namespace();
	}
}

//=============================================================================
// class imported_root_list method definitions

imported_root_list::imported_root_list() :
		root_item(), parent_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
imported_root_list::imported_root_list(const imported_root* i) :
		root_item(), parent_type(i) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
imported_root_list::~imported_root_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(imported_root_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
imported_root_list::leftmost(void) const {
	return parent_type::leftmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
imported_root_list::rightmost(void) const {
	return parent_type::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
imported_root_list::check_build(context& c) const {
	// will have already exited upon error
	// note: this call is obsolete, just quick to write
	return parent_type::check_build(c);
	// return never_ptr<const object>(NULL);
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

