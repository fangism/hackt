/**
	\file "test_case.cc"
	This file is just a scratch pad for creating test cases
	when debugging small examples of code.
 */

#define	ENABLE_STACKTRACE			1

#include <cstdio>
#include <iostream>
#include <vector>
#include "sim/prsim/Event-prsim.h"
#include "util/stacktrace.h"

using std::vector;
using std::endl;
using namespace HAC::SIM::PRSIM;

// dummy empty class
template <size_t S>
struct foo {
	char		fill[S];
	foo() { }
};

template <class T>
static
void
vector_push_crash(void) {
	STACKTRACE_VERBOSE;
	vector<T> ev;
	STACKTRACE_INDENT_PRINT("element size: " << sizeof(T) << endl);
	STACKTRACE_INDENT_PRINT("vector size: " << ev.size() << endl);
	STACKTRACE_INDENT_PRINT("vector capacity: " << ev.capacity() << endl);
	STACKTRACE_INDENT_PRINT("vector start @0x" << &ev.front() << endl);
	const T e;
	ev.push_back(e);
	// clang-build crashes in vector::push_back()!?
	STACKTRACE_INDENT_PRINT("vector size: " << ev.size() << endl);
	STACKTRACE_INDENT_PRINT("vector capacity: " << ev.capacity() << endl);
	STACKTRACE_INDENT_PRINT("vector start @0x" << &ev.front() << endl);
	ev.push_back(e);
	// clang-build crashes in vector::push_back()!?
	STACKTRACE_INDENT_PRINT("vector size: " << ev.size() << endl);
	STACKTRACE_INDENT_PRINT("vector capacity: " << ev.capacity() << endl);
	STACKTRACE_INDENT_PRINT("vector start @0x" << &ev.front() << endl);
}

static
void
vector_push_crash_2(void) {
	STACKTRACE_VERBOSE;
	vector<Event> ev;
	STACKTRACE_INDENT_PRINT("element size: " << sizeof(Event) << endl);
	STACKTRACE_INDENT_PRINT("vector size: " << ev.size() << endl);
	STACKTRACE_INDENT_PRINT("vector capacity: " << ev.capacity() << endl);
	STACKTRACE_INDENT_PRINT("vector start @0x" << &ev.front() << endl);
	const Event e;
	ev.push_back(e);
	// clang-build crashes in vector::push_back()!?
	STACKTRACE_INDENT_PRINT("vector size: " << ev.size() << endl);
	STACKTRACE_INDENT_PRINT("vector capacity: " << ev.capacity() << endl);
	STACKTRACE_INDENT_PRINT("vector start @0x" << &ev.front() << endl);
	ev.push_back(e);
	// clang-build crashes in vector::push_back()!?
	STACKTRACE_INDENT_PRINT("vector size: " << ev.size() << endl);
	STACKTRACE_INDENT_PRINT("vector capacity: " << ev.capacity() << endl);
	STACKTRACE_INDENT_PRINT("vector start @0x" << &ev.front() << endl);
}

static
void
event_pool_crash(void) {
	STACKTRACE_VERBOSE;
	EventPool ep;
	// clang-build crashes in index_pool::allocate on first push_back()!?
}

static
void
event_construct_test(void) {
	STACKTRACE_VERBOSE;
	Event e;
	const Event e2(e);
	Event e3, e4;
	std::_Construct(&e3, e);
	std::_Construct(&e4, e2);
	Event e5;
	::new(static_cast<void*>(&e5)) Event(e);
{
	Event ea[4];
	::new(static_cast<void*>(&ea[0])) Event(e);
	::new(static_cast<void*>(&ea[1])) Event(e2);
	std::_Construct(&ea[2], e);
	std::_Construct(&ea[3], e2);
}{
	vector<Event> ea;
	ea.resize(4);
	::new(static_cast<void*>(&ea[0])) Event(e);
	::new(static_cast<void*>(&ea[1])) Event(e2);
	std::_Construct(&ea[2], e);
	std::_Construct(&ea[3], e2);
}
}

/**
	FIXED: by explicitly writing Event's copy-constructor
	instead of letting compiler (clang) synthesize one.
	Need to report this bug!
 */
int
main(int, char*[]) {
	STACKTRACE_VERBOSE;
	event_construct_test();
#if 1
	// somehow, un/commenting the following tests changes crash behavior!?
	vector_push_crash<foo<1> >();
//	vector_push_crash<foo<8> >();	// works with/out this
	vector_push_crash<foo<32> >();	// uncommenting causes crash!
	vector_push_crash<Event>();
	vector_push_crash_2();		// works with/out this
#endif
	event_pool_crash();		// goal: get this to work
	return 0;
}


