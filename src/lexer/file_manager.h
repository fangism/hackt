/**
	\file "lexer/file_manager.h"
	Common file management facilities for including, search paths...
	Consider making this a general util for the library.  
	$Id: file_manager.h,v 1.1.2.3 2005/11/08 05:09:44 fang Exp $
 */

#ifndef	__LEXER_FILE_MANAGER_H__
#define	__LEXER_FILE_MANAGER_H__

#include <cstdio>
#include <iosfwd>
#include <string>
#include <stack>
#include "util/unique_list.h"
// #include "util/unique_stack.h"
#include "lexer/hac_lex.h"
#include "util/macros.h"

namespace ART {
namespace lexer {
using std::ostream;
using std::string;
using util::unique_list;
// using util::unique_stack;
//=============================================================================
/**
	Manages search path for files (like includes).  
	Keeps track of search paths using a unique_list.  
 */
#if 0
class search_paths : public unique_list<string> {
};	// end class search_paths
#else
typedef	unique_list<string>		search_paths;
#endif

//=============================================================================
/**
	Record of file and position in the file.  
 */
struct file_position {
	FILE*				file;
	token_position			pos;

	file_position() : file(NULL), pos(0, 0, 0) { }

	explicit
	file_position(FILE* const f) : file(f), pos(0, 0, 0) { }

	// default copy-constructor
	// default destructor
	// default assignment

};	// end struct file_position

//=============================================================================
/**
	Tracks the stack of nested, included files.  
	Can also guarantee that files are only ever opened once, 
	like implicit #pragma once.  
	Has a pure stack interface.  
	Strings are expanded file path names.  
	Keep track of token_position?
 */
class file_position_stack {
private:
	/**
		This accumulates a set of all files seen.  
		A set is insufficient, because we want to keep order
		to establish a map between files and indices.  
	 */
	unique_list<string>		_registry;
	std::stack<file_position>	_files;
public:
	file_position_stack();
	~file_position_stack();

	size_t
	size(void) const { return _files.size(); }

	void
	push(const file_position&);

	void
	push(const file_position&, const string&);

	const file_position&
	top(void) const { return _files.top(); }

	file_position&
	top(void) { return _files.top(); }

	void
	pop(void);

#if 0
	/**
		\return valid opened read-only FILE* if file
			wasn't already opened (not already on stack).  
			Returns null if already on stack.  
	 */
	FILE*
	open_FILE(const string&);
#endif

	FILE*
	current_FILE(void) const;

#if 0
	/**
		Pops last file off stack.
		\returns the previous file on stack (already opened).  
	 */
	FILE*
	close_FILE(void);
#endif

};	// end class file_position_stack

//=============================================================================
/**
	Combined manager to handle include paths and 
	opened file stack.  
	Consider adding some sort of vector and reverse_map
	to pair-up file-index and file-name (full path).  
 */
class file_manager {
	typedef	std::list<const char*>	file_names_type;
private:
	search_paths			_paths;
	file_position_stack		_fstack;
	/**
		Use as a stack for diagnostics.  
	 */
	file_names_type			_names;
public:
	file_manager();
	~file_manager();

	void
	add_path(const string&);

	file_position*
	open_FILE(const char*);

	file_position*
	open_FILE(const char*, FILE*);

	file_position*
	close_FILE(void);

	const file_position*
	current_FILE(void) const;

	/**
		Subtracting 1 from size because there is always
		at least one entry (from initialization).  
	 */
	size_t
	file_depth(void) const {
		return _fstack.size() -1;
	}

	ostream&
	dump_error_stack(ostream&);

};	// end class file_manager

//=============================================================================
}	// end namespace lexer
}	// end namespace ART

#endif	// __LEXER_FILE_MANAGER_H__

