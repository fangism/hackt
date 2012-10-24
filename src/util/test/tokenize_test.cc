/**
	\file "util/test/tokenize_test.cc"
	$Id: string_test.cc,v 1.2 2009/08/28 20:45:33 fang Exp $
 */

#include <iostream>
#include <string>
#include <list>
#include "util/tokenize.hh"

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
tokenize_string_test(const string& t, const string& sep) {
	cout << "\"" << t << "\" / \"" << sep << "\" -> ";
	util::string_list list;
	util::tokenize_string(t, sep, list);
	util::string_list::const_iterator i(list.begin()), e(list.end());
	for ( ; i!=e; ++i) {
		cout << "\"" << *i << "\", ";
	}
	cout << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(int, char*[]) {
	tokenize_string_test("a", "::");
	tokenize_string_test("::", "::");
	tokenize_string_test("::::", "::");
	tokenize_string_test(":::::", "::");
	tokenize_string_test("::", "::");
	tokenize_string_test("::a", "::");
	tokenize_string_test("a::", "::");
	tokenize_string_test("a::b", "::");
	tokenize_string_test("a::b::c", "::");
	tokenize_string_test("a:b::c", "::");
	tokenize_string_test("a::b:c", "::");
	tokenize_string_test("a::b.c.d[e]", "::");
	return 0;
}

