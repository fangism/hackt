/**
	\file "pretty_function_test.cc"
	$Id: pretty_function_test.cc,v 1.3 2006/02/26 02:28:05 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>

#include "util/using_ostream.hh"

class bar {
public:
	bar() { cerr << __PRETTY_FUNCTION__ << endl; }

	~bar() { cerr << __PRETTY_FUNCTION__ << endl; }

	void
	hello(void) const { cerr << __PRETTY_FUNCTION__ << endl; }

	static
	void
	greet(void) { cerr << __PRETTY_FUNCTION__ << endl; }

};

int
main(int, char*[]) {
	cerr << __PRETTY_FUNCTION__ << endl;
	bar bee;
	bar::greet();
	bee.hello();
	return 0;
}

