/**
	\file "static_trace.cc"
	Implementation (simple) of static initialization tracing utility
	class. 
	$Id: static_trace.cc,v 1.1.4.1 2005/01/27 23:36:11 fang Exp $
 */

// force proper preprocessing of its own header file
#define	ENABLE_STATIC_TRACE		1

#include <iostream>
#include "static_trace.h"
#include "using_ostream.h"
#include "macros.h"

namespace util {

static_begin::static_begin(const string& s) : msg(s) {
	INVARIANT(cerr.good());
	cerr << "START-initialization: " << msg << endl;
}

static_begin::~static_begin() {
	INVARIANT(cerr.good());
	cerr << "END---destruction: " << msg << endl;
}



static_end::static_end(const string& s) : msg(s) {
	INVARIANT(cerr.good());
	cerr << "END---initialization: " << msg << endl;
}

static_end::~static_end() {
	INVARIANT(cerr.good());
	cerr << "START-destruction: " << msg << endl;
}

}	// end namespace util

