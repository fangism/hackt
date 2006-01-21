/**
	\file "lexer/file_manager.h"
	Common file management facilities for including, search paths...
	Consider making this a general util for the library.  
	$Id: file_manager.h,v 1.4.2.2 2006/01/21 21:56:21 fang Exp $
 */

#ifndef	__LEXER_FILE_MANAGER_H__
#define	__LEXER_FILE_MANAGER_H__

#include <iosfwd>
#include <string>
#include <list>
#include <stack>
#include <utility>			// for std::pair
#include "util/unique_list.h"
// #include "util/unique_stack.h"
#include "lexer/hac_lex.h"
#include "util/macros.h"
#include "util/file_status.h"

namespace HAC {
namespace lexer {
using std::ostream;
using std::string;
using util::unique_list;
// using util::unique_stack;
using util::file_status;

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
	/**
		store the (full-path) name of the file here 
		redundantly for convenience
	 */
	string				name;

	file_position() : file(NULL), pos(1, 0, 1), name() { }

#if 0
	/**
		For anonymous file streams like stdin.
	 */
	explicit
	file_position(FILE* const f) : file(f), pos(1, 0, 1), name() { }
#endif

	file_position(FILE* const f, const string& n) :
		file(f), pos(1, 0, 1), name(n) { }

	// default copy-constructor
	// default destructor
	// default assignment -- should not be assignable...

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
public:
	typedef	std::default_list<file_position>::type	position_stack_type;
	typedef	position_stack_type::const_iterator	const_iterator;
private:
	/**
		This accumulates a set of all files seen.  
		A set is insufficient, because we want to keep order
		to establish a map between files and indices.  
	 */
	unique_list<string>		_registry;
	/**
		Stack of file positions, implemented as a list
		to be able to walk the stack from front to back.
		Could use a deque.  
	 */
	position_stack_type		_files;
public:
	file_position_stack();
	~file_position_stack();

	size_t
	size(void) const { return _files.size(); }

	const_iterator
	begin(void) const { return _files.begin(); }

	const_iterator
	end(void) const { return _files.end(); }

	bool
	push(const file_position&);

#if 0
	bool
	push(const file_position&, const string&);
#endif

	const file_position&
	top(void) const { return _files.back(); }

	file_position&
	top(void) { return _files.back(); }

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

	void
	reset(void);

};	// end class file_position_stack

//=============================================================================
/**
	Combined manager to handle include paths and 
	opened file stack.  
	Consider adding some sort of vector and reverse_map
	to pair-up file-index and file-name (full path).  
 */
class file_manager {
	typedef	unique_list<string>	file_names_type;
private:
	search_paths			_paths;
	/**
		This tracks a stack of opened file positions, 
		and also keeps a registry of all files ever seen
		(not just those on stack) for the purpose
		of implementing implicit #pragma once.  
	 */
	file_position_stack		_fstack;
	/**
		Use as a stack for diagnostics.  
		Also maintains uniqueness for detecting cyclic
		dependencies.  
	 */
	file_names_type			_names;
public:
	/**
		The first field is the resulting file_position at
		the top of the stack.  
		The second value indicates whether or not the 
		last requested opened file was ignored 
		(because it is already open).  
	 */
	typedef	std::pair<file_position*, file_status::status>
					return_type;
public:
	file_manager();
	~file_manager();

	void
	add_path(const string&);

	return_type
	open_FILE(const char*);

	return_type
	open_FILE(const char*, FILE*);

	file_position*
	close_FILE(void);

	const file_position*
	current_FILE(void) const;

	const string&
	top_FILE_name(void) const {
		return _names.back();
	}

	/**
		For the lexer to update the current file position.  
		\pre File stack must not be empty.  
	 */
	token_position&
	current_position(void) {
		return _fstack.top().pos;
	}

	const token_position&
	current_position(void) const {
		return _fstack.top().pos;
	}

	/**
		Subtracting 1 from size because there is always
		at least one entry (from initialization).  
	 */
	size_t
	file_depth(void) const {
		return _fstack.size() -1;
	}

	void
	reset(void);

	ostream&
	dump_file_stack(ostream&) const;

	ostream&
	dump_file_stack_top(ostream&) const;

	ostream&
	reset_and_dump_file_stack(ostream&);

};	// end class file_manager

//=============================================================================
}	// end namespace lexer
}	// end namespace HAC

#endif	// __LEXER_FILE_MANAGER_H__

