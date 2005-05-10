/**
	\file "util/static_trace.cc"
	Implementation (simple) of static initialization tracing utility
	class. 
	$Id: static_trace.cc,v 1.3 2005/05/10 04:51:30 fang Exp $
 */

// force proper preprocessing of its own header file
#define	ENABLE_STATIC_TRACE		1

#include <iostream>
#include "util/static_trace.h"
#include "util/using_ostream.h"
#include "util/macros.h"

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

