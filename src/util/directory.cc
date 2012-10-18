/**
	\file "util/directory.cc"
	$Id: directory.cc,v 1.3 2011/05/23 01:10:51 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iterator>
#include <algorithm>
#include <list>
#include "util/directory.hh"
#include "util/tokenize.hh"
#include "util/string.hh"
#include "util/macros.h"
#include "util/stacktrace.hh"

namespace util {
#include "util/using_ostream.hh"
using std::ostream_iterator;
using strings::strip_prefix;

//=============================================================================
// class directory_stack method definitions

directory_stack::directory_stack() : dir_stack(), 
		absolute_prefix("::"), 
		separator("."),
		this_dir_string("."),
		parent_dir_string(".."),
		parent_separator("/") {
	// start with an empty string
	reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directory_stack::~directory_stack() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directory_stack::reset(void) {
	// start with an empty string
	dir_stack.clear();
	dir_stack.push_back("");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directory_stack::set_separator(const char c) {
	separator.clear();
	separator.push_back(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
directory_stack::current_working_directory(void) const {
	INVARIANT(!dir_stack.empty());
	return dir_stack.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the length of the string that would be prefixed
		using the current working directory, including the length
		of the delimiter.
 */
size_t
directory_stack::common_prefix_length(void) const {
	const string& wd(current_working_directory());
	if (wd.length()) {
		return wd.length() + separator.length();
	} else	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Changes top-directory of stack, relatively, unless _rel begins with 
	the absolute prefix.  
	\return true if there is error.
 */
bool
directory_stack::__change_directory(string& cur, string rel) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("current: " << cur << ", rel: " << rel << endl);
	INVARIANT(!dir_stack.empty());
if (!rel.length()) {
#if 0
	// empty string, do nothing (could go home to ::, but require explicit)
	cur.clear();
#endif
} else if (strip_prefix(rel, absolute_prefix)) {
	// have absolute path, just replace it (stripping absolute-prefix)
	cur = rel;
	// TODO: handle parent directory references in rel
} else if (rel == this_dir_string) {
	// leave cur as-is
} else {
	// subtract and add paths according to relative spec
	string_list toks;
	tokenize(cur, toks, separator.c_str());
#if 0 && ENABLE_STACKTRACE
	cout << "toks: ";
	copy(toks.begin(), toks.end(), ostream_iterator<string>(cout, " "));
	cout << endl;
#endif
{
	string_list rtoks;
	tokenize(rel, rtoks, parent_separator.c_str());
	string_list::const_iterator i(rtoks.begin()), e(rtoks.end());
#if 0 && ENABLE_STACKTRACE
	cout << "rtoks: ";
	copy(i, e, ostream_iterator<string>(cout, " "));
	cout << endl;
#endif
	for ( ;i!=e; ++i) {
	if (*i == parent_dir_string) {
		if (toks.size()) {
			toks.pop_back();
		}
	} else if (*i != this_dir_string) {
		string_list t;
		// caution: *i may contain ".." range operators
		// but separator may be "."!
		// this should be PARSED
		if (separator.length() > 1) {
			tokenize(*i, t, separator.c_str());
		} else {
			// FIXME: HACK ALERT!
			tokenize_single_char_only(*i, t, separator[0]);
		}
#if 0 && ENABLE_STACKTRACE
		cout << "t: ";
		copy(t.begin(), t.end(), ostream_iterator<string>(cout, "+"));
		cout << endl;
#endif
#if 1
		// for tab-completion, want to keep trailing '.'
		// so don't remove empty strings
		copy(t.begin(), t.end(), back_inserter(toks));
#else
		remove_copy_if(t.begin(), t.end(), back_inserter(toks), 
			mem_fun_ref(&string::empty));
#endif
	}
	// else ignore this_dir_string
	}
}
	string_list::const_iterator i(toks.begin()), e(toks.end());
if (!toks.empty()) {
	// glue tokens back together, using separator
	cur = *i;
	for (++i; i!=e; ++i) {
		cur += separator +*i;
	}
} else {
	cur.clear();
}
	STACKTRACE_INDENT_PRINT("result: " << cur << endl);
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
directory_stack::change_directory(const string& _rel) {
	INVARIANT(!dir_stack.empty());
	return __change_directory(dir_stack.back(), _rel);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
directory_stack::push_directory(const string& _rel) {
	INVARIANT(!dir_stack.empty());
	dir_stack.push_back(dir_stack.back());
	return change_directory(_rel);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if popping stack would result in an empty stack.
	The stack must contain at least one working directory at all times.
 */
bool
directory_stack::pop_directory(void) {
	if (dir_stack.size() > 1) {
		dir_stack.pop_back();
		return false;
	} else {
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Transforms relative/absolute references to expanded name, 
	stripping absolute prefix where applicable.  
 */
string
directory_stack::transform(string d) const {
	string cwd(current_working_directory());
	// can actually reuse __change_directory() for a leaf name
	__change_directory(cwd, d);
	return cwd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	So that the root directory doesn't look blank.
 */
static
string
show_dir(const string& s) {
	if (!s.length()) {
		return string("~");
	} else	return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directory_stack::dump_working_directory(ostream& o) const {
	return o << show_dir(current_working_directory());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directory_stack::dump_stack(ostream& o) const {
	std::transform(dir_stack.rbegin(), dir_stack.rend(), 
		ostream_iterator<string>(o, " "), &show_dir);
	return o;
}

//=============================================================================
}	// end namespace util

