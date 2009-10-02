/**
	\file "util/string.cc"
	$Id: string.cc,v 1.4 2009/10/02 01:57:46 fang Exp $
	Additional string utilities.
 */

#include <string>
#include "util/string.h"
#include "util/macros.h"

namespace util {
namespace strings {
using std::string;

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
}	// end namespace strings
}	// end namespace util

