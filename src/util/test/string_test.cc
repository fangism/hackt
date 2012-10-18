/**
	\file "util/test/string_test.cc"
	$Id: string_test.cc,v 1.2 2009/08/28 20:45:33 fang Exp $
 */

#include <iostream>
#include <string>
#include "util/string.hh"

using std::string;
using std::cout;
using std::endl;

/**
	\param t target string
	\param s search substring
	\param r replacement string
	\param e expected result
 */
static
void
strgsub_test(const string& t, const string& s,
		const string& r, const string& e) {
	cout << "\"" << t << "\" : (\"" << s << "\", \"" << r << "\") -> \""
		<< e << "\" : ";
	string _t(t);
	util::strings::strgsub(_t, s, r);
	if (_t != e) {
		cout << "FAIL, got \"" << _t << "\"";
	} else {
		cout << "PASS";
	}
	cout << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(int, char*[]) {
	strgsub_test("abcabcabcabc", "xyz", "", "abcabcabcabc");
	strgsub_test("abcabcabcabc", "a", "", "bcbcbcbc");
	strgsub_test("abcabcabcabc", "a", "x", "xbcxbcxbcxbc");
	strgsub_test("abcabcabcabc", "a", "xy", "xybcxybcxybcxybc");
	strgsub_test("abcabcabcabc", "ab", "", "cccc");
	strgsub_test("abcabcabcabc", "ca", "", "abbbbc");
	strgsub_test("abcabcabcabc", "a", "a", "abcabcabcabc");
	strgsub_test("abcabcabcabc", "a", "aa", "aabcaabcaabcaabc");
	strgsub_test("abcabcabcabc", "a", "bc", "bcbcbcbcbcbcbcbc");
	strgsub_test("abcabcabcabc", "abcabc", "abc", "abcabc");
	return 0;
}

