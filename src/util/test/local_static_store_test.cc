/**
	\file "local_static_store_test.cc"
	Testing for expected lifetime and behavior of 
	function-local static objects.  
	$Id: local_static_store_test.cc,v 1.1.2.2 2005/01/22 20:53:27 fang Exp $
 */

#define ENABLE_STACKTRACE		1

#include <iostream>
// #include <string>
#include "using_ostream.h"
#include "named_pooled_thing.h"
#include "stacktrace.h"

// DEBUG memory pool using stacktrace, seeing double destruction!

USING_STACKTRACE;
// using std::string;


static
void
never_called_one(void) {
	STACKTRACE("never_called_one()");
	static const named_thing anon;
}

static
void
never_called_two(void) {
	STACKTRACE("never_called_two()");
	static const excl_ptr<named_thing> anon;
}

static
void
actually_called_one(void) {
	STACKTRACE("actually_called_one()");
	static const named_thing anon1("Joe");
//	static named_thing anon2("Carol");
}

static
void
actually_called_two(void) {
	STACKTRACE("actually_called_two()");
//	static const never_ptr<named_thing> anon0(new named_thing("Bob"));
		// will cause a leak of course!
	static const excl_ptr<named_thing> anon1(new named_thing("Bob"));
//	static const count_ptr<named_thing> anon2(new named_thing("Anna"));
}

// causes death upon ostream!
#if 1
static const named_thing anon_static("Dot");
#endif
#if 1
static const excl_ptr<const named_thing>
anon_dynamic(new named_thing("Eve"));
#endif

int
main(int argc, char* argv[]) {
	STACKTRACE("main()");
	cerr << "anon_static sitting at " << &anon_static << endl;
	cerr << "anon_dynamic sitting at " << &*anon_dynamic << endl;
#if 0
	actually_called_one();
	actually_called_one();
#endif
#if 1
	actually_called_two();
	actually_called_two();
#endif
	return 0;
}

