/**
	\file "AST/import_root.cc"
	$Id: import_root.cc,v 1.2.2.1 2005/12/11 00:45:12 fang Exp $
 */

#include <iostream>
#include "AST/import_root.h"
#include "AST/art_parser_node_list.tcc"
#include "AST/parse_context.h"
#include "util/memory/count_ptr.tcc"
#include "Object/common/namespace.h"
#include "common/TODO.h"
#include "util/what.h"

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::imported_root, "(imported-root)")
SPECIALIZE_UTIL_WHAT(HAC::parser::imported_root_list, "(imported-root-list)")
}

namespace HAC {
namespace parser {
#include "util/using_ostream.h"
//=============================================================================
// class imported_root method definitions

imported_root::imported_root(excl_ptr<root_body>& r, const string& n,
		const bool s) :
		root(r), name(n), seen(s) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
imported_root::~imported_root() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(imported_root)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
imported_root::string_compare(const char*) const {
	FINISH_ME_EXIT(Fang);
	return 0;
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

