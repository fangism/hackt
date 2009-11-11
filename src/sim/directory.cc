/**
	\file "sim/directory.cc"
	$Id: directory.cc,v 1.1 2009/11/11 00:34:03 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iterator>
#include <algorithm>
#include <list>
#include "sim/directory.h"
#include "util/tokenize.h"
#include "util/string.h"
#include "util/macros.h"
#include "util/stacktrace.h"

namespace HAC {
namespace SIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
using util::string_list;
using util::tokenize;
using util::strings::strip_prefix;

//=============================================================================
// class directory_stack method definitions

directory_stack::directory_stack() : dir_stack(), 
		absolute_prefix("::"), 
		separator("."),
		this_dir_string("."),
		parent_dir_string(".."),
		parent_separator("/") {
	// start with an empty string
	dir_stack.push_back("");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directory_stack::~directory_stack() { }

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
	// empty string, go home
	cur.clear();
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
{
	string_list rtoks;
	tokenize(rel, rtoks, parent_separator.c_str());
	string_list::const_iterator i(rtoks.begin()), e(rtoks.end());
	for ( ;i!=e; ++i) {
	if (*i == parent_dir_string) {
		if (toks.size()) {
			toks.pop_back();
		}
	} else if (*i != this_dir_string) {
		string_list t;
		tokenize(*i, t, separator.c_str());
		remove_copy_if(t.begin(), t.end(), back_inserter(toks), 
			mem_fun_ref(&string::empty));
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
}	// end namespace SIM
}	// end namespace HAC

