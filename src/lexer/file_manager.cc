/**
	\file "lexer/file_manager.cc"
	$Id: file_manager.cc,v 1.1.2.6 2005/11/09 08:24:00 fang Exp $
 */

#include <iostream>
#include "lexer/file_manager.h"
#include "util/unique_list.tcc"
// #include "util/unique_stack.tcc"

#define	ENABLE_STACKTRACE			0
#include "util/stacktrace.h"

namespace ART {
namespace lexer {
#include "util/using_ostream.h"
//=============================================================================
// class file_position_stack method definitions

file_position_stack::file_position_stack() : _registry(), _files() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
file_position_stack::~file_position_stack() {
	STACKTRACE_VERBOSE;
	reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Anonymous file_position.  
 */
void
file_position_stack::push(const file_position& fp) {
	_files.push_back(fp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pushes file_position onto stack and registers file name with it
	to the list of files seen.  
	\return true if file was already included/opened previously.  
 */
bool
file_position_stack::push(const file_position& fp, const string& fn) {
	STACKTRACE_VERBOSE;
	if (_registry.push(fn)) {
		return true;
	} else {
		_files.push_back(fp);
		return false;
	}
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
	return _files.back().file;
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
	_files.pop_back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears stack.  
 */
void
file_position_stack::reset(void) {
	STACKTRACE_VERBOSE;
	while (!_files.empty()) {
		FILE* f = _files.back().file;
		if (f)	fclose(f);
		_files.pop_back();
	}
}

//=============================================================================
// class file_manager method definitions

file_manager::file_manager() : _paths(), _fstack() {
	_fstack.push(file_position());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
file_manager::~file_manager() {
	STACKTRACE_VERBOSE;
	// reset();
	const size_t fs = _fstack.size();
	if (fs) {
		INVARIANT(!_fstack.top().file);
	}
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
file_manager::return_type
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
			return open_FILE(dfs, ret);
		}
		// else continue searching
	}
}
	// else not found
	return return_type(NULL, false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This this variant, the caller has already opened FILE* f.  
	\param f already opened file pointer.  
	\param fs may be NULL, representing stdin.
	\return the file pointer and whether or not file is already opened.  
 */
file_manager::return_type
file_manager::open_FILE(const char* fs, FILE* f) {
	STACKTRACE_VERBOSE;
	INVARIANT(f);
	bool inc = false;		// whether or not was already included
	// don't register stdin with a file name
	if (fs) {
		inc = _fstack.push(file_position(f), fs);
	} else {
		_fstack.push(file_position(f));
	}
	_names.push_back(fs ? fs : "-stdin-");
	return return_type(&_fstack.top(), inc);
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
void
file_manager::reset(void) {
	STACKTRACE_VERBOSE;
	_fstack.reset();
	_fstack.push(file_position());
	_paths.clear();
	_names.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps the current file and position stack for diagnostics.  
 */
ostream&
file_manager::dump_file_stack(ostream& o) const {
	// cerr << _fstack.size() << endl;
	// cerr << _names.size() << endl;
	INVARIANT(_fstack.size() == _names.size() +1);
	file_position_stack::const_iterator pi(++_fstack.begin());
	const file_position_stack::const_iterator pe(_fstack.end());
	file_names_type::const_iterator ni(_names.begin());
	// const file_names_type::const_iterator ne(_names.end());
	for ( ; pi != pe; pi++, ni++) {
		const token_position& t(pi->pos);
		o << "At: \"" << *ni << "\":" << t.line << ':' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps just the inner-most file.  
 */
ostream&
file_manager::dump_file_stack_top(ostream& o) const {
	// cerr << _fstack.size() << endl;
	// cerr << _names.size() << endl;
	INVARIANT(_fstack.size() == _names.size() +1);
	const file_position_stack::const_iterator pi(--_fstack.end());
	const file_names_type::const_iterator ni(--_names.end());
	const token_position& t(pi->pos);
	o << "From: \"" << *ni << "\":" << t.line << ':' << endl;
	return o;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
file_manager::reset_and_dump_file_stack(ostream& o) {
	dump_file_stack(o);
	reset();
	return o;
}

//=============================================================================
}	// end namespace lexer
}	// end namespace ART

