/**
	\file "named_pooled_ting.cc"
	Definitions for another testing class.
	$Id: named_pooled_thing.cc,v 1.1.4.1 2005/01/23 01:34:03 fang Exp $
 */

#define ENABLE_STACKTRACE				1
#define	DEBUG_LIST_VECTOR_POOL				1
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		1

#include "named_pooled_thing.h"

#include <iostream>
#include <string>
#include "using_ostream.h"
#include "what.h"
#include "stacktrace.h"

#include "memory/list_vector_pool.h"
#include "memory/pointer_classes.h"


USING_STACKTRACE;
using std::cin;

#if 1
named_thing::named_thing() {
	STACKTRACE("empty ctor");
	cout << this << endl;
}
named_thing::named_thing(const string& s) {
	STACKTRACE("dummy ctor");
	cout << this << endl;
}
named_thing::~named_thing() {
	STACKTRACE("dtor");
	cout << this << endl;
//	char c; cin >> c;
}


#elif 0
named_thing::named_thing() : _int(new int) {
	STACKTRACE("empty ctor");
	cout << this << endl;
}
named_thing::named_thing(const string& s) : _int(new int) {
	STACKTRACE("dummy ctor");
	cout << this << endl;
}
named_thing::named_thing(const named_thing& n) : _int(new int) {
	STACKTRACE("copy ctor");
	cout << this << endl;
	n._int = NULL;
}
named_thing::~named_thing() {
	STACKTRACE("dtor");
	cout << this << endl;
	if (_int) delete _int; _int = NULL;
}

#else
named_thing::named_thing() : name("???"), _int(new int) {
	STACKTRACE("empty ctor");
	cout << "Hello, " << name << " @ " << this << endl;
}
named_thing::named_thing(const string& s) : name(s), _int(new int) {
	STACKTRACE("real ctor");
	cout << "Hello, " << name << "! @ " << this << endl;
}

named_thing::named_thing(const named_thing& n) : name(n.name), _int(n._int) {
	STACKTRACE("copy ctor");
	n._int = NULL;		// destructive transfer
}

named_thing::~named_thing() {
	STACKTRACE("dtor");
	cout << "Goodbye, name @ " << this << '.' << endl;
	if (_int) delete _int; _int = NULL;
}
#endif

namespace util {
	SPECIALIZE_UTIL_WHAT_DEFINITION(named_thing, "named_thing");
}

#if ENABLE_POOL
// intentionally defining this last as a test...
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(named_thing, 16)
#endif

