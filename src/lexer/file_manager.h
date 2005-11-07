/**
	\file "lexer/file_manager.h"
	Common file management facilities for including, search paths...
	Consider making this a general util for the library.  
	$Id: file_manager.h,v 1.1.2.2 2005/11/07 08:55:08 fang Exp $
 */

#ifndef	__LEXER_FILE_MANAGER_H__
#define	__LEXER_FILE_MANAGER_H__

#include <cstdio>
#include <string>
#include <stack>
#include "util/unique_list.h"
// #include "util/unique_stack.h"
#include "lexer/hac_lex.h"
#include "util/macros.h"

namespace ART {
namespace lexer {
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
	 */
	unique_list<string>		_stack;
	std::stack<FILE*>		_files;
public:
	file_position_stack();
	~file_position_stack();

	/**
		\return valid opened read-only FILE* if file
			wasn't already opened (not already on stack).  
			Returns null if already on stack.  
	 */
	FILE*
	open_FILE(const string&);

	FILE*
	current_FILE(void) const;

	/**
		Pops last file off stack.
		\returns the previous file on stack (already opened).  
	 */
	FILE*
	close_FILE(void);

};	// end class file_position_stack

//=============================================================================
/**
	Combined manager to handle include paths and 
	opened file stack.  
	Consider adding some sort of vector and reverse_map
	to pair-up file-index and file-name (full path).  
 */
class file_manager {
private:
	search_paths			_paths;
	file_position_stack		_fstack;
public:
	file_manager();
	~file_manager();

	void
	add_path(const string&);

	FILE*
	open_FILE(const string&);

	FILE*
	close_FILE(void);

};	// end class file_manager

//=============================================================================
}	// end namespace lexer
}	// end namespace ART

#endif	// __LEXER_FILE_MANAGER_H__

