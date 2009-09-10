/**
	\file "util/named_ifstream_manager.h"
	Common file management facilities for including, search paths...
	Consider making this a general util for the library.  
	$Id: named_ifstream_manager.h,v 1.5.8.1 2009/09/10 18:38:33 fang Exp $
 */

#ifndef	__UTIL_NAMED_IFSTREAM_MANAGER_H__
#define	__UTIL_NAMED_IFSTREAM_MANAGER_H__

#include <iosfwd>
#include <fstream>
#include <string>
#include <list>
#include <stack>
#include <utility>			// for std::pair
#include "config.h"
#if 0
#include "util/STL/construct_fwd.h"
#include "util/STL/allocator_fwd.h"
#endif
#include "util/unique_list.h"
#include "util/macros.h"
#include "util/attributes.h"
#include "util/file_status.h"

#if !defined(HAVE_STD_IFSTREAM_DEV_STDIN)
#warn "Support without std::ifstream("/dev/stdin") is yet untested."
#endif

namespace util {
using std::ostream;
using std::ifstream;
using std::string;
// using util::unique_list;
// using util::file_status;
class named_ifstream_track;

//=============================================================================
/**
	Manages search path for files (like includes).  
	Keeps track of search paths using a unique_list.  
 */
typedef	unique_list<string>		search_paths;

//=============================================================================
/**
	Record of file and position in the file.  
	Consider deriving from ifstream and including the name.  
	nifstream -- named ifstream.  
 */
class named_ifstream : public std::ifstream {
	typedef	std::ifstream			parent_type;
	typedef	named_ifstream			this_type;
public:
	static const string			dev_stdin;
private:
	/**
		store the (full-path) name of the file here 
		redundantly for convenience
	 */
	string				_name;

public:
	named_ifstream() : _name() { }

	explicit
	named_ifstream(const string&);

	// default copy-constructor
	// default destructor
	// default assignment -- should not be assignable...

	const string&
	name(void) const { return _name; }

	bool
	open(const string& s) {
		parent_type::open(s.c_str());
		return *this;
	}

#if 0
private:
	FRIEND_STD_CONSTRUCT2
#if	defined(HAVE_EXT_NEW_ALLOCATOR_H)
	template <typename>
	friend class NEW_ALLOCATOR_NAMESPACE::new_allocator;
#endif
#endif

	// non-copyable: note: doesn't actually copy
	// needed to be able to extend list (push_back)
	explicit
	named_ifstream(const this_type&) :
		std::ios(), std::ifstream(), _name() { }

private:
	// not assignable
	this_type&
	operator = (const this_type&);

};	// end struct named_ifstream

//=============================================================================
/**
	Tracks the stack of nested, included files.  
	Can also guarantee that files are only ever opened once, 
	like implicit #pragma once.  
	Has a pure stack interface.  
	Strings are expanded file path names.  
	Keep track of token_position?
	KNOWN PROBLEM: on solaris-2.9, gcc-3.3, 
		list<named_ifstream> triggers a cast-align warning.  :(
 */
class named_ifstream_stack {
public:
	typedef	std::list<named_ifstream>		record_stack_type;
	typedef	record_stack_type::const_iterator	const_iterator;
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
	record_stack_type		_files;
public:
	named_ifstream_stack();
	~named_ifstream_stack();

	size_t
	size(void) const { return _files.size(); }

	const_iterator
	begin(void) const { return _files.begin(); }

	const_iterator
	end(void) const { return _files.end(); }

	bool
	push(const string&);

	const named_ifstream&
	top(void) const { return _files.back(); }

	named_ifstream&
	top(void) { return _files.back(); }

	void
	pop(void);

	void
	reset(void);

};	// end class named_ifstream_stack

//=============================================================================
/**
	Combined manager to handle include paths and 
	opened file stack.  
	Consider adding some sort of vector and reverse_map
	to pair-up file-index and file-name (full path).  
 */
class ifstream_manager {
	typedef	unique_list<string>	file_names_type;
private:
	search_paths			_paths;
	/**
		This tracks a stack of opened file positions, 
		and also keeps a registry of all files ever seen
		(not just those on stack) for the purpose
		of implementing implicit #pragma once.  
	 */
	named_ifstream_stack		_fstack;
	/**
		Use as a stack for diagnostics.  
		Also maintains uniqueness for detecting cyclic
		dependencies.  
	 */
	file_names_type			_names;
public:
	/**
		The first field is the resulting named_ifstream at
		the top of the stack.  
		The second value indicates whether or not the 
		last requested opened file was ignored 
		(because it is already open).  
	 */
	typedef	std::pair<named_ifstream*, file_status::status>
					return_type;
	/**
		Use this struct as a proxy to manage the state of the
		ifstream manager.  
		This automatically closes file streams upon
		end-of-scope, and balances the file stacks.  
	 */
	class placeholder {
		ifstream_manager&		_ifm;
		return_type			_status;
	public:
		explicit
		placeholder(ifstream_manager&, const string&);

		~placeholder();

		return_type
		status(void) const { return _status; }

		bool
		good(void) const;

		operator bool () const { return good(); }

		named_ifstream&
		get_stream(void);

		ostream&
		error_msg(ostream&) const;

	private:
		// non-copyable
		explicit
		placeholder(const placeholder&);

	} __ATTRIBUTE_UNUSED__ ;
public:
	ifstream_manager();
	~ifstream_manager();

	void
	add_path(const string&);

private:
	return_type
	open_ifstream(const string&);

	return_type
	__open_ifstream(const string&);

	void
	close_ifstream(void);
public:

	named_ifstream&
	current_named_ifstream(void);

	const string&
	top_named_ifstream_name(void) const {
		return _names.back();
	}

	/**
		\return 0 if empty, else the depth of file inclusion.  
	 */
	size_t
	file_depth(void) const {
		return _fstack.size();
	}

private:
	void
	reset(void);

	ostream&
	reset_and_dump_file_stack(ostream&);
public:
	ostream&
	dump_paths(ostream&) const;

	ostream&
	dump_file_names(ostream&) const;

	ostream&
	dump_file_stack(ostream&) const;

	ostream&
	dump_file_stack_top(ostream&) const;

	/// dump everything, for debugging
	ostream&
	dump(ostream&) const;

};	// end class ifstream_manager

//=============================================================================
}	// end namespace util

#endif	// __UTIL_NAMED_IFSTREAM_MANAGER_H__

