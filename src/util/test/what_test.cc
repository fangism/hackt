/**
	\file "what_test.cc"
	$Id: what_test.cc,v 1.1 2005/01/12 03:20:13 fang Exp $
 */

#include <iostream>
#include "what.tcc"

#define	DEBUG_LIST_VECTOR_POOL		1	// pre-define to enable debug
#include "memory/list_vector_pool.h"

#include "using_ostream.h"
using util::what;
using util::memory::list_vector_pool;

class Foo {
};

class Ick {
};

namespace util {
SPECIALIZE_UTIL_WHAT(Ick, "ick");
}

template <class T>
class Bar {
public:
	static
	void
	print_type(void) {
		cerr << what<T>::name << endl;
	}
};

class Yum {
};

namespace util {
SPECIALIZE_UTIL_WHAT(Yum, "yum");
SPECIALIZE_UTIL_WHAT(Bar<Ick>, "Bar<Ick>");
}

int
main(int argc, char* argv) {
	cerr << what<Foo>::name << endl;
	cerr << what<Ick>::name << endl;
	cerr << what<Yum>::name << endl;
	cerr << what<Bar<Foo> >::name << endl;
	cerr << what<Bar<Ick> >::name << endl;
	cerr << what<Bar<Yum> >::name << endl;
	Bar<Foo>::print_type();
	Bar<Ick>::print_type();
	Bar<Yum>::print_type();

	list_vector_pool<Foo>		Foo_pool(8);
	list_vector_pool<Ick>		Ick_pool(8);
	list_vector_pool<Yum>		Yum_pool(8);
	list_vector_pool<Bar<Foo> >	BarFoo_pool(8);
	list_vector_pool<Bar<Ick> >	BarIck_pool(8);
	list_vector_pool<Bar<Yum> >	BarYum_pool(8);

	return 0;
}

