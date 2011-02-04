/**
	\file "util/named_ifstream_manager.cc"
	Adapted from "lexer/ifstream_manager.cc"
	$Id: named_ifstream_manager.cc,v 1.6 2011/02/04 02:23:40 fang Exp $
 */

#include <iostream>
#include <fstream>
#include "config.h"
#include "util/named_ifstream_manager.h"
#include "util/unique_list.tcc"

#define	ENABLE_STACKTRACE			0
#include "util/stacktrace.h"

namespace util {
using std::ifstream;
#include "util/using_ostream.h"

//=============================================================================
// class named_ifstream method definitions

#if !NAMED_IFSTREAM_USE_ISTREAM_PTR
const string
named_ifstream::dev_stdin(DEV_STDIN);
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NAMED_IFSTREAM_USE_ISTREAM_PTR
named_ifstream::operator bool () const {
	return stream && *stream;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	With no file name, open stdin/cin
 */
bool
named_ifstream::open(void) {
	stream = istream_ptr_type(&std::cin);	// ptr type will not delete
	return stream && *stream;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
named_ifstream::open(const string& s) {
#if NAMED_IFSTREAM_USE_ISTREAM_PTR
	stream = istream_ptr_type(new ifstream(s.c_str()));
#else
	parent_type::open(s.c_str());
#endif
	return *this;
}

//=============================================================================
// class named_ifstream_stack method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
named_ifstream_stack::named_ifstream_stack() : _registry(), _files() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
named_ifstream_stack::~named_ifstream_stack() {
	STACKTRACE_VERBOSE;
	reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Opens a file stream.  
	\param fp the path name to a file guaranteed to be openable.  
	\return true if file was seen before, 
		false if this file is open for the first time.  
		NOTE: this does not detect file cycles.  
		Never checks "/dev/stdin", always opens it.  
 */
bool
named_ifstream_stack::push(const string& fp) {
#if !NAMED_IFSTREAM_USE_ISTREAM_PTR
	INVARIANT(fp.length());
if ((fp != named_ifstream::dev_stdin)) {
#endif
	const bool ret = _registry.push(fp);
	const named_ifstream dummy;
	_files.push_back(dummy);	// uses fake copy ctor
	_files.back().open(fp);
	INVARIANT(_files.back());
	return ret;
#if !NAMED_IFSTREAM_USE_ISTREAM_PTR
} else {
	return false;
}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
named_ifstream_stack::pop(void) {
	STACKTRACE_VERBOSE;
	_files.pop_back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Clears stack.  
 */
void
named_ifstream_stack::reset(void) {
	STACKTRACE_VERBOSE;
	_files.clear();
}

//=============================================================================
// class ifstream_manager method definitions

ifstream_manager::ifstream_manager() : _paths(), _fstack() {
#if 0
	const string dummy;
	_fstack.push(dummy);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ifstream_manager::~ifstream_manager() {
#if 0
	STACKTRACE_VERBOSE;
	// reset();
	const size_t fs = _fstack.size();
	if (fs) {
		INVARIANT(!_fstack.top());
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a path to the list of include paths to search.  
 */
void
ifstream_manager::add_path(const string& p) {
	STACKTRACE_VERBOSE;
	_paths.push(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param fs the name of the included file (relative path).  
		This method will always try the local directory first.
		TODO: some research about the ordering rules used
		by cpp/gcc, since that's what we're trying to imitate. 
	\return pointer to opened file if successful, else NULL.  
 */
ifstream_manager::return_type
ifstream_manager::open_ifstream(const string& fs) {
	STACKTRACE_VERBOSE;
{
	ifstream probe(fs.c_str());
	if (probe) {
		probe.close();
		return __open_ifstream(fs);
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
		const string df(*i +"/" +fs);	// does strcat
		ifstream probe(df.c_str());
		if (probe) {
			probe.close();
			return __open_ifstream(df);
		}
		// else continue searching
	}
}
	// else not found
	return return_type(AS_A(named_ifstream*, NULL), file_status::NOT_FOUND);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This this variant, the caller has already verified the 
		open-ability of the file named by fs.  
	\param fs is a full path to file, including the search path
		with which it was found.  fs may be empty, representing stdin.
	\pre we already know the named file stream is openable, 
		from the caller.  
	\return the file stream pointer and 
		whether or not file is already opened.  
 */
ifstream_manager::return_type
ifstream_manager::__open_ifstream(const string& fs) {
	STACKTRACE_VERBOSE;
#if 0 && defined(HAVE_STD_IFSTREAM_DEV_STDIN)
	static const string _stdin_(named_ifstream::dev_stdin);
	const bool cyc = _names.push_back(fs.length() ? fs : _stdin_);
#else
	const bool cyc =
		(
#if NAMED_IFSTREAM_USE_ISTREAM_PTR
		fs.size()
#else
		fs != named_ifstream::dev_stdin
#endif
			? _names.push_back(fs) : false);
#endif
	// don't register stdin with a file name in the named_ifstream_stack
	// but is ok to register it with the cycle-detection _names stack.
	file_status::status s;		// whether or not was already included
	if (cyc) {
		// then must also already have been on _fstack
		const bool seen = _fstack.push(fs);	// sure to be rejected
		INVARIANT(seen);
		_fstack.pop();			// for balance
		s = file_status::CYCLE;
	} else
#if 0 && defined(HAVE_STD_IFSTREAM_DEV_STDIN)
	if (fs.length())
#endif
	{
		// check to see whether or not it is new or visited
		// NOTE: stdin is always never yet visited
		s = (_fstack.push(fs) ?
			file_status::SEEN_FILE : file_status::NEW_FILE);
#if 0
		if (s == file_status::SEEN_FILE) {
			// then we're not going to open the FILE
			_names.pop_back();
		}
#else
		// open the file even if we've seen it
		// let caller decide whether or not to close it
#endif
	}
#if 0 && defined(HAVE_STD_IFSTREAM_DEV_STDIN)
	else {
		// stdin: can only be opened once, so safe to assert it is new
		// ignore the return status of push
		_fstack.push(named_ifstream::dev_stdin);
		s = file_status::NEW_FILE;
	}
#endif
	return return_type(&_fstack.top(), s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
named_ifstream&
ifstream_manager::current_named_ifstream(void) {
	return _fstack.top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes the file on the top of the file stack, and 
	returns a pointer to the previous file.  
	Only do this if you know that the file (open request) was successful.
 */
void
ifstream_manager::close_ifstream(void) {
	STACKTRACE_VERBOSE;
	_fstack.pop();
	_names.pop_back();
	// return &_fstack.top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ifstream_manager::reset(void) {
	STACKTRACE_VERBOSE;
	_fstack.reset();
	// _fstack.push(string());	// NULL file
	_paths.clear();
	_names.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps the current file and position stack for diagnostics.  
 */
ostream&
ifstream_manager::dump_file_stack(ostream& o) const {
	STACKTRACE_VERBOSE;
	typedef	named_ifstream_stack::const_iterator	const_iterator;
	// const_iterator pi(++_fstack.begin());
	const_iterator pi(_fstack.begin());
	const const_iterator pe(_fstack.end());
	// file_names_type::const_iterator ni(_names.begin());
	// const file_names_type::const_iterator ne(_names.end());
	for ( ; pi != pe; pi++) {
		// const token_position& t(pi->pos);
		// o << "At: \"" << pi->name << "\":" << t.line << ':' << endl;
		o << "At: \"" << pi->name() << "\":" << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps just the inner-most file.  
 */
ostream&
ifstream_manager::dump_file_stack_top(ostream& o) const {
	const named_ifstream_stack::const_iterator pi(--_fstack.end());
	// const file_names_type::const_iterator ni(--_names.end());
	// const token_position& t(pi->pos);
	o << "From: \"" << pi->name() << "\":" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ifstream_manager::reset_and_dump_file_stack(ostream& o) {
	dump_file_stack(o);
	reset();
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps out contents of _names.  
 */
ostream&
ifstream_manager::dump_file_names(ostream& o) const {
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
ifstream_manager::dump_paths(ostream& o) const {
	search_paths::const_iterator i(_paths.begin());
	const search_paths::const_iterator e(_paths.end());
	for ( ; i!=e; ++i) {
		o << *i << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ifstream_manager::dump(ostream& o) const {
	// dump include paths
	dump_file_stack(o) << endl;
	dump_file_names(o) << endl;
	return o;
}

//=============================================================================
// class ifstream_manager::placeholder method definitions

ifstream_manager::placeholder::placeholder(
		ifstream_manager& i, const string& s) :
		_ifm(i), _status(_ifm.open_ifstream(s)) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ifstream_manager::placeholder::~placeholder() {
	if (good()) {
		_ifm.close_ifstream();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
named_ifstream&
ifstream_manager::placeholder::get_stream(void) {
	INVARIANT(good());
	return _ifm.current_named_ifstream();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
ifstream_manager::placeholder::good(void) const {
	return (_status.second == file_status::NEW_FILE ||
			_status.second == file_status::SEEN_FILE);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
ifstream_manager::placeholder::error_msg(ostream& o) const {
	switch(_status.second) {
	case file_status::NEW_FILE:
		// fall-through
	case file_status::SEEN_FILE:
		break;
	case file_status::NOT_FOUND:
		o << "Not found.";
		break;
	case file_status::CYCLE:
		o << "Cyclic file reference.";
		break;
	default:
		o << "Unknown.";
	}
	return o;
}

//=============================================================================
}	// end namespace util

