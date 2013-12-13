/**
	\file "lexer/file_manager.cc"
	$Id: file_manager.cc,v 1.11 2010/03/11 18:39:23 fang Exp $
 */

#include <iostream>
#include <set>
#include <cstdio>
#include "lexer/file_manager.hh"
#include "util/unique_list.tcc"
// #include "util/unique_stack.tcc"
#include "util/memory/count_ptr.tcc"

#define	ENABLE_STACKTRACE			0
#include "util/stacktrace.hh"

namespace util {
namespace memory {
using HAC::lexer::file_manager;
// explicit template instantiation
template class count_ptr<file_manager::embed_manager>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace lexer {
#include "util/using_ostream.hh"
using std::set;

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
	\return true if file was already included/opened previously.  
 */
bool
file_position_stack::push(const file_position& fp) {
	if (fp.name.length()) {
		if (!_registry.push(fp.name).second) {
			return true;
		} else {
			_files.push_back(fp);
			return false;
		}
	} else {
		// file is anonymous (like stdin)
		_files.push_back(fp);
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	push a copy of the top.  
	Intended for use with special-case file-stack management, 
	like file embedding.  
	This should be balanced with pop().
 */
void
file_position_stack::dupe(void) {
	_files.push_back(_files.back());
	_files.back().pos.line = 0;
	_files.back().pos.col = 1;
	// column should be 1, but let row increment to 1 with newline
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FILE*
file_position_stack::current_FILE(void) const {
	STACKTRACE_VERBOSE;
	return _files.back().file;
}

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
	\param fs the name of the included file (relative path).  
	NB: traditional cpp will always try the local directory first, 
		we use the local directory as the final fallback path.
	Rationale: the user can always specify where '.' should be placed
		in the search order, but not if it is always searched first.
	\return pointer to opened file if successful, else NULL.  
 */
file_manager::return_type
file_manager::open_FILE(const char* fs) {
	STACKTRACE_VERBOSE;
	// check search paths first!
{
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
	// then check 'here' as a fallback path
{
	FILE* ret = fopen(fs, "r");
	if (ret) {
		return open_FILE(fs, ret);
	}
	// else didn't find, continue searching include paths
}
	// else not found
	return return_type(AS_A(file_position*, NULL),
		file_status::NOT_FOUND);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: this file names stack business should be much cleaner.  
	This this variant, the caller has already opened FILE* f.  
	\param f already opened file pointer.  
	\param fs is a full path to file, including the search path
		with which it was found.  fs may be NULL, representing stdin.
	\return the file pointer and whether or not file is already opened.  
 */
file_manager::return_type
file_manager::open_FILE(const char* fs, FILE* f) {
	STACKTRACE_VERBOSE;
	static const char _stdin_[] = "-stdin-";
	INVARIANT(f);
	// don't register stdin with a file name in the file_position_stack
	// but is ok to register it with the cycle-detection _names stack.
	const bool cyc = !_names.push_back(fs ? fs : _stdin_).second;
	file_status::status s;		// whether or not was already included
	if (cyc) {
		// then must also already have been on _fstack
		const bool seen = _fstack.push(file_position(f, fs));
		INVARIANT(seen);
		s = file_status::CYCLE;
	} else if (fs) {
		// check to see whether or not it is new or visited
		s = _fstack.push(file_position(f, fs)) ?
			file_status::SEEN_FILE : file_status::NEW_FILE;
		if (s == file_status::SEEN_FILE) {
			// then we're not going to open the FILE
			_names.pop_back();
		}
	} else {
		// stdin: can only be opened once, so safe to assert it is new
		_fstack.push(file_position(f, _stdin_));
		s = file_status::NEW_FILE;
	}
	return return_type(&_fstack.top(), s);
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
/**
	Coercively change the current file and line position.
	USE WITH CAUTION, may leave file manager incoherent if
	directives are imbalanced!!!
	Consult physician if irritation occurs.  
	\param fn the file name to change to (path expanded).
	\param fp the line number to update the current position.
 */
void
file_manager::coerce_line_directive(const string& fn, const int fp) {
	_names.pop();
	_names.push(fn);
	_fstack.top().name = fn;
	token_position& t(current_position());
	t.line = fp -1;		// -1 because following newline will increment
	t.col = 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
file_manager::reset(void) {
	STACKTRACE_VERBOSE;
	_fstack.reset();
	_fstack.push(file_position());	// NULL file
	_paths.clear();
	_names.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps the current file and position stack for diagnostics.  
 */
ostream&
file_manager::dump_file_stack(ostream& o) const {
	STACKTRACE_VERBOSE;
	typedef	file_position_stack::const_iterator	const_iterator;
#if 0
	cerr << _fstack.size() << endl;
	cerr << _names.size() << endl;
	// INVARIANT(_fstack.size() == _names.size() +1);	// not true
#endif
	const_iterator pi(++_fstack.begin());
	const const_iterator pe(_fstack.end());
	// file_names_type::const_iterator ni(_names.begin());
	// const file_names_type::const_iterator ne(_names.end());
	for ( ; pi != pe; pi++) {
		const token_position& t(pi->pos);
		o << "At: \"" << pi->name << "\":" << t.line << ':' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps just the inner-most file.  
 */
ostream&
file_manager::dump_file_stack_top(ostream& o) const {
#if 0
	cerr << _fstack.size() << endl;
	cerr << _names.size() << endl;
	// INVARIANT(_fstack.size() == _names.size() +1);	// not true
#endif
	const file_position_stack::const_iterator pi(--_fstack.end());
	// const file_names_type::const_iterator ni(--_names.end());
	const token_position& t(pi->pos);
	o << "From: \"" << pi->name << "\":" << t.line << ':' << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
file_manager::reset_and_dump_file_stack(ostream& o) {
	dump_file_stack(o);
	reset();
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps out contents of _names.  
 */
ostream&
file_manager::dump_file_names(ostream& o) const {
	typedef	file_names_type::const_iterator	const_iterator;
	const_iterator i(_names.begin());
	const const_iterator e(_names.end());
	for ( ; i!=e; ++i) {
		o << *i << ", ";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
file_manager::dump(ostream& o) const {
	// dump include paths
	dump_file_stack(o) << endl;
	dump_file_names(o) << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	List all seen files' full paths.  
	Also prints phony targets.  
	\param s the name of the primary source file (to exclude).
	\param o the output stream.
 */
ostream&
file_manager::make_depend(ostream& o, const string& s) const {
	INVARIANT(o);
	typedef	set<string>			temp_set_type;
	typedef	temp_set_type::const_iterator	const_iterator;
	temp_set_type deps(_fstack.get_registry().begin(),
		_fstack.get_registry().end());
	deps.erase(s);
if (!deps.empty()) {
	const const_iterator b(deps.begin()), e(deps.end());
{
	const_iterator i(b);
	for ( ; i!=e; ++i) {
		o << " \\\n\t" << *i;
	}
}
	o << endl << endl;
{
	// phony targets
	const_iterator i(b);
	for ( ; i!=e; ++i) {
		o << *i << ":\n";
	}
}
}
	return o << endl << endl;
}

//=============================================================================
// class file_manager::embed_manager method definitions

/**
	Register a fake opened file (embedded).
 */
file_manager::embed_manager::embed_manager(file_manager& m, const string& n) :
		fm(m) {
	fm._names.push(n);
	fm._fstack.dupe();	// same file pointer, adjusted name
	fm._fstack.top().name = n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Close/unregister an embedded file.  
 */
file_manager::embed_manager::~embed_manager() {
	fm._names.pop();
	fm._fstack.pop();
	fm._fstack.top().pos.line--;
	// back-track one line count
}

//=============================================================================
}	// end namespace lexer
}	// end namespace HAC

