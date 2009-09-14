/**
	\file "util/optparse.cc"
	Implementation of rudimentary option parsing.
	$Id: optparse.cc,v 1.3 2009/09/14 21:17:16 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include "util/size_t.h"
#include "util/optparse.h"
#include "util/tokenize.h"
#include "util/string.h"	// for strgsub

namespace util {
using std::transform;
using strings::strgsub;
#include "util/using_ostream.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print an single option with its values.
 */
ostream&
operator << (ostream& o, const option_value& v) {
	o << v.key << '=';
	list<string>::const_iterator
		i(v.values.begin()), e(v.values.end());
if (i!=e) {
	list<string>::const_iterator l(e);
	--l;	// point to last element
	std::copy(i, l, std::ostream_iterator<string>(o, ","));
	o << *l;	// no trailing comma
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print a list of options with their values, newline separated. 
 */
ostream&
operator << (ostream& o, const option_value_list& v) {
	std::copy(v.begin(), v.end(),
		std::ostream_iterator<option_value>(o, "\n"));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
option_value
optparse(const string& s, const char c) {
	return optparse(s, '=', c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
option_value
optparse(const string& s) {
	return optparse(s, ',');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Backwards compatibility option parsing.
	Expects format: type name value
	TODO: value cannot contain any spaces... yet
	We ignore type for now.
	TODO: type check later
 */
option_value
optparse_compat(const string& s) {
	option_value ret;
	list<string> opts;
	tokenize(s, opts);
	if (opts.size() != 3) {
		cerr << "Warning: malformed old-style option string: "
			<< s << endl;
	} else {
		const string& k(*++opts.begin());
		ret.key = k;
		string v(opts.back());
		if (opts.front() == "string") {
			// strip quotes
			const size_t subs = strgsub(v, "\"", "");
			if (subs != 2) {
cerr << "Warning: possibly misquoted string value for parameter " << k << endl;
			}
		}
		ret.values.push_back(v);
	}
	return ret;		// NULL option
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For overload resolution.
 */
static
option_value
optparse_default(const string& s) {
	return optparse(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pass a NUL to prevent value separation.
 */
option_value
optparse_no_sep(const string& s) {
	return optparse(s, '\0');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s option string, should have no spaces!
	\param e is the key separator, usually '='
	\param c is the value separator, usually ','
		If separate is NUL, then don't bother separating.  
	If there is no key separator, then the whole string is assumed
		to be the key, or error out, or just silently ignore
		and return blank?
	Value(s) following '=' separator is allowed to be empty.
 */
option_value
optparse(const string& s, const char e, const char c) {
	option_value ret;
	size_t f = s.find(e);
	const size_t end = string::npos;
	ret.key = s.substr(0, f);
if (c) {
if (f != end) {
	++f;
	size_t a = s.find(c, f);
	while (a != end) {
		ret.values.push_back(s.substr(f, a-f));
		f = a+1;
		a = s.find(c, f);
	}
	if (f != end) {
		ret.values.push_back(s.substr(f));
	}
}
} else {
	// treat remaining string as a single value, can be processed later
	ret.values.push_back(s.substr(f+1));
}
	// optional: diagnostic?
//	cerr << "got: " << ret << endl;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
option_value_list
optparse_list(const string& s) {
	option_value_list ret;
	list<string> opts;
	tokenize(s, opts);
	transform(opts.begin(), opts.end(), std::back_inserter(ret), 
		&optparse_default);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
option_value_list
optparse_list(const string& s, const char c) {
	option_value_list ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses options from a stream/file.
	Ignores blank lines and '#' comments.
	TODO: error handling here?
 */
static
option_value_list
optparse_file_generic(istream& i, option_value (*parse)(const string&)) {
	option_value_list ret;
	size_t lineno = 1;
	string line;
while (std::getline(i, line)) {
	// TODO: allow leading whitespace before '#'
	// TODO: eat whitespaces
	if (line.length() && line[0] != '#') {
		list<string> toks;
		tokenize(line, toks);
		const size_t s = toks.size();
		if (s >= 1) {
			if (s > 1) {
	cerr << "Warning: ignoring extra tokens after whitespace on line "
		<< lineno << "." << endl;
			}
			ret.push_back((*parse)(toks.front()));
		}
		// else ignore blank
	}
	++lineno;
}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
option_value_list
optparse_file(istream& i) {
	return optparse_file_generic(i, &optparse);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Backwards compatibility mode.
	Parses options from a stream/file.
	Ignores blank lines and '#' comments.
	TODO: error handling here?
 */
option_value_list
optparse_file_compat(istream& i) {
	option_value_list ret;
	size_t lineno = 1;
	string line;
while (std::getline(i, line)) {
	// TODO: allow leading whitespace before '#'
	// TODO: eat whitespaces
	if (line.length() && line[0] != '#') {
		ret.push_back(optparse_compat(line));
		// else ignore blank
	}
	++lineno;
}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace util

