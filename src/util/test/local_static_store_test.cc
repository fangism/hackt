/**
	\file "local_static_store_test.cc"
	Testing for expected lifetime and behavior of 
	function-local static objects.  
	$Id: local_static_store_test.cc,v 1.1.2.1 2005/01/22 06:38:29 fang Exp $
 */

#define ENABLE_STACKTRACE		1
#define	DEBUG_LIST_VECTOR_POOL		1

#include <iostream>
#include <string>
#include "using_ostream.h"
#include "memory/list_vector_pool.h"
#include "memory/pointer_classes.h"
#include "what.h"
#include "stacktrace.h"

USING_STACKTRACE;
using util::what;
using std::string;
using namespace util::memory;

#if 1
class named_thing {
	typedef	named_thing		this_type;
public:
	named_thing() {
		STACKTRACE("empty ctor");
		cout << this << endl;
	}
	named_thing(const string& s) {
		STACKTRACE("dummy ctor");
		cout << this << endl;
	}
	~named_thing() {
		STACKTRACE("dtor");
		cout << this << endl;
	}

//	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
};
#elif 0
class named_thing {
	typedef	named_thing		this_type;
private:
	mutable int*		_int;
public:
	named_thing() : _int(new int) {
		STACKTRACE("empty ctor");
		cout << this << endl;
	}
	named_thing(const string& s) : _int(new int) {
		STACKTRACE("dummy ctor");
		cout << this << endl;
	}
	named_thing(const named_thing& n) : _int(new int) {
		STACKTRACE("copy ctor");
		cout << this << endl;
		n._int = NULL;
	}
	~named_thing() {
		STACKTRACE("dtor");
		cout << this << endl;
		if (_int) delete _int; _int = NULL;
	}
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
};

#else
class named_thing {
	typedef	named_thing		this_type;
private:
	string		name;
	mutable int*	_int;
public:
	named_thing() : name("???"), _int(new int) {
		STACKTRACE("empty ctor");
		cout << "Hello, " << name << " @ " << this << endl;
	}
	named_thing(const string& s) : name(s), _int(new int) {
		STACKTRACE("real ctor");
		cout << "Hello, " << name << "! @ " << this << endl;
	}

	named_thing(const named_thing& n) : name(n.name), _int(n._int) {
		STACKTRACE("copy ctor");
		n._int = NULL;		// destructive transfer
	}

	~named_thing() {
		STACKTRACE("dtor");
		cout << "Goodbye, name @ " << this << '.' << endl;
		if (_int) delete _int; _int = NULL;
	}

	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
};	// end class named_thing
#endif

namespace util {
	SPECIALIZE_UTIL_WHAT(named_thing, "named_thing");
}

// placing here or below makes no difference in program execution?
// LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(named_thing, 16)


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
	static const excl_ptr<named_thing> anon1(new named_thing("Bob"));
	static const count_ptr<named_thing> anon2(new named_thing("Anna"));
}

// intentionally defining this last as a test...
// LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(named_thing, 16)

int
main(int argc, char* argv[]) {
	STACKTRACE("main()");
	actually_called_one();
	actually_called_two();
	actually_called_one();
	actually_called_two();
	return 0;
}

