/**
	\file "stacktrace_test.cc"
	Test for stacktrace dumping utility.
 */

#include <iostream>
#include "using_ostream.h"

#define	ENABLE_STACKTRACE	1

#include "stacktrace.h"

using util::stacktrace;

// only needed for static, dynamically-allocated uses
// REQUIRES_STACKTRACE_STATIC_INIT

static
int
f_a(const int x) {
	STACKTRACE("f_a");
	return x * 2;
}

static
int
f_b(const int x) {
	STACKTRACE("f_b");
	return x + 3;
}

static
int
f_c(const int x, const int y) {
	STACKTRACE("f_c");
	return f_a(x) + f_b(y);
}

int
f_g(void) {
	STACKTRACE("f_g");
	return f_c(2, 3);
}

float
dude_wheres_my_prototype(const char* a, int b[4]) {
	STACKTRACE_BRIEF;
	return 0.0;
}

float
verbose_function_trace(const char* a, int b[4]) {
	STACKTRACE_VERBOSE;
	return 0.0;
}

int
main(int argc, char* argv[]) {
	STACKTRACE_VERBOSE;
	static int four[4];
	{
		STACKTRACE_ECHO_ON;
		cerr << f_g() << endl;
		dude_wheres_my_prototype("useless", four);
		verbose_function_trace("useless", four);
	}
	{
		STACKTRACE_ECHO_OFF;
		cerr << f_g() << endl;
		dude_wheres_my_prototype("useless", four);
		verbose_function_trace("useless", four);
	}
}

