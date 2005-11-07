/**
	\file "lexer/file_manager.cc"
	$Id: file_manager.cc,v 1.1.2.2 2005/11/07 08:55:08 fang Exp $
 */

#include "lexer/file_manager.h"
#include "util/unique_list.tcc"
// #include "util/unique_stack.tcc"

namespace ART {
namespace lexer {
//=============================================================================
// class file_position_stack method definitions

file_position_stack::file_position_stack() : _stack(), _files() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
file_position_stack::~file_position_stack() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
FILE*
file_position_stack::open_FILE(const string& s) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FILE*
file_position_stack::current_FILE(void) const {
	return _files.top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FILE*
file_position_stack::close_FILE(void) {
}
#endif

//=============================================================================
// class file_manager method definitions

file_manager::file_manager() : _paths(), _fstack() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
file_manager::~file_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a path to the list of include paths to search.  
 */
void
file_manager::add_path(const string& p) {
	_paths.push(p);
}

//=============================================================================
}	// end namespace lexer
}	// end namespace ART

