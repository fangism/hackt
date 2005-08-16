/**
 */

#include <iostream>

#include "util/using_ostream.h"

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

