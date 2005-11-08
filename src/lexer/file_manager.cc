/**
	\file "lexer/file_manager.cc"
	$Id: file_manager.cc,v 1.1.2.3 2005/11/08 05:09:43 fang Exp $
 */

// #include <fstream>
#include "lexer/file_manager.h"
#include "util/unique_list.tcc"
// #include "util/unique_stack.tcc"

#define	ENABLE_STACKTRACE			0
#include "util/stacktrace.h"

namespace ART {
namespace lexer {
// using std::ifstream;
//=============================================================================
// class file_position_stack method definitions

file_position_stack::file_position_stack() : _registry(), _files() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
file_position_stack::~file_position_stack() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Anonymous file_position.  
 */
void
file_position_stack::push(const file_position& fp) {
	_files.push(fp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pushes file_position onto stack and registers file name with it
	to the list of files seen.  
 */
void
file_position_stack::push(const file_position& fp, const string& fn) {
	STACKTRACE_VERBOSE;
	_files.push(fp);
	_registry.push(fn);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
FILE*
file_position_stack::open_FILE(const string& s) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FILE*
file_position_stack::current_FILE(void) const {
	STACKTRACE_VERBOSE;
	return _files.top().file;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
FILE*
file_position_stack::close_FILE(void) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
file_position_stack::pop(void) {
	STACKTRACE_VERBOSE;
	_files.pop();
}

//=============================================================================
// class file_manager method definitions

file_manager::file_manager() : _paths(), _fstack() {
	_fstack.push(file_position());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
file_manager::~file_manager() {
	INVARIANT(_fstack.size() == 1);
	INVARIANT(!_fstack.top().file);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a path to the list of include paths to search.  
 */
void
file_manager::add_path(const string& p) {
	STACKTRACE_VERBOSE;
	_paths.push(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param f the name of the included file (relative path).  
		This method will always try the local directory first.
		TODO: some research about the ordering rules used
		by cpp/gcc, since that's what we're trying to imitate. 
	\return pointer to opened file if successful, else NULL.  
 */
file_position*
file_manager::open_FILE(const char* fs) {
	STACKTRACE_VERBOSE;
{
	FILE* ret = fopen(fs, "r");
	if (ret) {
		return open_FILE(fs, ret);
	}
	// else didn't find, continue searching include paths
}
{
	// else check search paths one-by-one
	search_paths::const_iterator i(_paths.begin());
	const search_paths::const_iterator e(_paths.end());
	for ( ; i!=e; i++) {
		// use some form of weak cat-string?
		// for now, assuming unix style directory separator, 
		// configure later...
		const string df(*i +"/"+fs);	// does strcat
		const char* dfs = df.c_str();
		FILE* ret = fopen(dfs, "r");
		if (ret) {
			return open_FILE(fs, ret);
		}
		// else continue searching
	}
}
	// else not found
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This this variant, the caller has already opened FILE* f.  
	\param f already opened file pointer.  
	\param fs may be NULL, representing stdin.
 */
file_position*
file_manager::open_FILE(const char* fs, FILE* f) {
	STACKTRACE_VERBOSE;
	INVARIANT(f);
	// don't register stdin with a file name
	if (fs)	_fstack.push(file_position(f), fs);
	else	_fstack.push(file_position(f));
	_names.push_back(fs ? fs : "-stdin-");
	return &_fstack.top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const file_position*
file_manager::current_FILE(void) const {
	return &_fstack.top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes the file on the top of the file stack, and 
	returns a pointer to the previous file.  
 */
file_position*
file_manager::close_FILE(void) {
	STACKTRACE_VERBOSE;
	_fstack.pop();
	_names.pop_back();
	return &_fstack.top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\return 0 if there are files remaining on the file stack, 
		else 1 to signal end of input.  
 */
int
file_manager::yywrap(void) const {
	STACKTRACE_VERBOSE;
	// if there's one entry left, it's the null-placeholder entry.
	return (_fstack.size() == 1 ? 1 : 0);
}
#endif

//=============================================================================
}	// end namespace lexer
}	// end namespace ART

