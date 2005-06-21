/**
	\file "local_static_store_test.cc"
	Testing for expected lifetime and behavior of 
	function-local static objects.  

	One thing to note:
	The function-local static pool for the class named_thing
	doesn't seem to be destroyed.  
	Don't know what the standard says about this behoavior...

	$Id: local_static_store_test.cc,v 1.3 2005/06/21 21:26:42 fang Exp $
 */

#define ENABLE_STACKTRACE		1

#include <iostream>
// #include <string>
#include "using_ostream.h"
#include "named_pooled_thing.h"
#include "memory/list_vector_pool.h"
#include "stacktrace.h"

// DEBUG memory pool using stacktrace, seeing double destruction!

USING_STACKTRACE
// using std::string;

// declare this before any dynamic allocation of this type in this module
REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(named_thing)

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
	static named_thing anon2("Carol");
}

static
void
actually_called_two(void) {
	STACKTRACE("actually_called_two()");
//	static const never_ptr<named_thing> anon0(new named_thing("Bob"));
		// will cause a leak of course!
	static const excl_ptr<const named_thing>
		anon1(new named_thing("Bob"));
	static const count_ptr<const named_thing>
		anon2(new named_thing("Anna"));
}

static const named_thing anon_static("Dot");
static const excl_ptr<const named_thing>
anon_dynamic(new named_thing("Eve"));

int
main(int argc, char* argv[]) {
	STACKTRACE("MAIN()");	// magic expression used to filter for test
	cerr << "anon_static sitting at " << &anon_static << endl;
	cerr << "anon_dynamic sitting at " << &*anon_dynamic << endl;

	actually_called_one();
	actually_called_one();

	actually_called_two();
	actually_called_two();

	return 0;
}

