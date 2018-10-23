/**
	\file "lexer/file_manager.hh"
	Common file management facilities for including, search paths...
	Consider making this a general util for the library.  
	$Id: file_manager.hh,v 1.10 2009/10/02 01:57:24 fang Exp $
 */

#ifndef	__LEXER_FILE_MANAGER_HH__
#define	__LEXER_FILE_MANAGER_HH__

#include <iosfwd>
#include <string>
#include <list>
#include <stack>
#include <utility>			// for std::pair
#include "util/unique_list.hh"
// #include "util/unique_stack.hh"
#include "lexer/hac_lex.hh"
#include "util/macros.h"
#include "util/attributes.h"
#include "util/file_status.hh"
#include "util/memory/pointer_classes_fwd.hh"

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
	typedef	unique_list<string>	registry_type;
public:
	typedef	std::list<file_position>	position_stack_type;
	typedef	position_stack_type::const_iterator	const_iterator;
private:
	/**
		This accumulates a set of all files seen.  
		A set is insufficient, because we want to keep order
		to establish a map between files and indices.  
	 */
	registry_type		_registry;
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

	const registry_type&
	get_registry(void) const { return _registry; }


	const file_position&
	top(void) const { return _files.back(); }

	file_position&
	top(void) { return _files.back(); }

	void
	dupe(void);

	void
	pop(void);


	FILE*
	current_FILE(void) const;

	void
	reset(void);

};	// end class file_position_stack

//=============================================================================
/**
	Combined manager to handle include paths and 
	opened file stack.  
	Consider adding some sort of vector and reverse_map
	to pair-up file-index and file-name (full path).  
	NOTE: this is not the same as the similar class
		util::file_manager.
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

	bool
	empty(void) const { return _names.empty(); }

	/**
		For the lexer to update the current file position.  
		\pre File stack must not be empty.  
	 */
	token_position&
	current_position(void) {
		INVARIANT(_fstack.size());
		return _fstack.top().pos;
	}

	const token_position&
	current_position(void) const {
		INVARIANT(_fstack.size());
		return _fstack.top().pos;
	}

	/// Egregious hack to half-ass support #line directives.
	void
	coerce_line_directive(const string&, const int);

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
	dump_file_names(ostream&) const;

	ostream&
	dump_file_stack(ostream&) const;

	ostream&
	dump_file_stack_top(ostream&) const;

	ostream&
	reset_and_dump_file_stack(ostream&);

	/// dump everything, for debugging
	ostream&
	dump(ostream&) const;

	ostream&
	make_depend(ostream&, const string&) const;

	/**
		This struct manages the file_manager in a way to 
		accommodate 'faking' pushing a file onto the stack
		without actually opening a file, just manipulating
		only the file name and token position stack.
	 */
	class embed_manager {
	private:
		file_manager& 			fm;
	public:
		embed_manager(file_manager&, const string&);
		~embed_manager();
		// heap-allocate OK
		// do not copy, do not assign
	};	// end struct embed_manager

};	// end class file_manager

typedef	std::stack<util::memory::count_ptr<file_manager::embed_manager> >
			embedded_file_stack_type;

//=============================================================================
class file_wrap_directive_printer {
	ostream&				os;
	string					filename;
	bool					enable;
	bool					comment_out;
	/**
		When called from lexer, want to suppress this b/c
		the import statement's newline is already preserved.
	 */
	bool					trailing_newline;

public:
	file_wrap_directive_printer(ostream&, const string&, 
		const bool e = true, const bool c = false,
		const bool t = false);
	~file_wrap_directive_printer();

	void
	disable(void) { enable = false; }

} __ATTRIBUTE_UNUSED__ ;	// end class file_wrap_directive_printer

//=============================================================================
}	// end namespace lexer
}	// end namespace HAC

#endif	// __LEXER_FILE_MANAGER_HH__

