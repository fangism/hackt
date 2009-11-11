/**
	\file "util/string.cc"
	$Id: string.cc,v 1.6 2009/11/11 00:34:06 fang Exp $
	Additional string utilities.
 */

#include <string>
#include <cctype>
#include "util/string.tcc"
#include "util/macros.h"

namespace util {
namespace strings {
using std::string;

//=============================================================================
bool
string_begins_with(const string& s, const string& pre) {
	return s.find(pre) == 0;
}

//=============================================================================
/**
	Exact match substitution.
 */
size_t
strgsub(string& t, const string& s, const string& r) {
	// substitution is done by copying pieces of strings
	size_t subs = 0;
if (s.length() && (s != r)) {
	string ret;	// temporary
	const size_t slen = s.length();
	INVARIANT(slen);
	size_t tpos = 0;
	size_t mpos = t.find(s, tpos);
	while (mpos != string::npos) {
		++subs;
		// copy non-matching prefix
		ret.append(t.begin()+tpos, t.begin()+mpos);
		ret.append(r);		// substitute match
		tpos = mpos +slen;	// skip over matched part
		mpos = t.find(s, tpos);
	}
	ret.append(t.begin()+tpos, t.end());	// remainder
	if (t != ret)
		t = ret;
	// otherwise, just leave alone
}
	return subs;
}

//=============================================================================
/**
	Repeatedly removes prefix of string.
	\return the number of occurrences removed.  
 */
size_t
strip_prefix(string& t, const string& s) {
	size_t subs = 0;
	while (string_begins_with(t, s)) {
		t = t.substr(s.length());
		++subs;
	}
	return subs;
}

//=============================================================================
string
string_tolower(const string& s) {
	return transform_string(s, &tolower);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
string_toupper(const string& s) {
	return transform_string(s, &toupper);
}

//=============================================================================
}	// end namespace strings
}	// end namespace util

