/**
	\file "util/static_trace.cc"
	Implementation (simple) of static initialization tracing utility
	class. 
	$Id: static_trace.cc,v 1.4.50.1 2006/01/04 08:42:44 fang Exp $
 */

// force proper preprocessing of its own header file
#define	ENABLE_STATIC_TRACE		1

#include <iostream>
#include "util/static_trace.h"
#include "util/macros.h"

namespace util {
#include "util/using_ostream.h"
//=============================================================================
static_begin::static_begin(const string& s) : msg(s) {
	INVARIANT(cerr);
	cerr << "START-initialization: " << msg << endl;
}

static_begin::~static_begin() {
	INVARIANT(cerr);
	cerr << "END---destruction: " << msg << endl;
}

//=============================================================================

static_here::static_here(const string& s) : msg(s) {
	INVARIANT(cerr);
	cerr << "creating marker: " << msg << endl;
}

static_here::~static_here() {
	INVARIANT(cerr);
	cerr << "destroying marker: " << msg << endl;
}

//=============================================================================
static_end::static_end(const string& s) : msg(s) {
	INVARIANT(cerr);
	cerr << "END---initialization: " << msg << endl;
}

static_end::~static_end() {
	INVARIANT(cerr);
	cerr << "START-destruction: " << msg << endl;
}

//=============================================================================
}	// end namespace util

