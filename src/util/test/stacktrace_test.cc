/**
	\file "stacktrace_test.cc"
	Test for stacktrace dumping utility.
 */

#include <iostream>
#include "using_ostream.h"

#define	ENABLE_STACKTRACE	1

#include "stacktrace.h"

using util::stacktrace;

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

int
main(int argc, char* argv[]) {
	{
		STACKTRACE_ECHO_ON;
		cerr << f_g() << endl;
	}
	{
		STACKTRACE_ECHO_OFF;
		cerr << f_g() << endl;
	}
}

