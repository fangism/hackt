/**
	\file "util/static_trace.cc"
	Implementation (simple) of static initialization tracing utility
	class. 
	$Id: static_trace.cc,v 1.7 2007/02/27 05:37:43 fang Exp $
 */

// force proper preprocessing of its own header file
#define	ENABLE_STATIC_TRACE		1

#include <iostream>			// for std::cerr, std::endl
// to guarantee proper iostream initialization prior to use
// declare a std::ios_base::Init.  
#include "util/static_trace.h"
#include "util/macros.h"		// for INVARIANT (assert)

namespace util {
using std::string;
#include "util/using_ostream.h"

//=============================================================================
/**
	Common initializing constructor.  
 */
static_common::static_common(const string& s) : 
		ios_init(), msg(s) {
	INVARIANT(cerr);
}

static_common::~static_common() {
	INVARIANT(cerr);
}

//=============================================================================
static_begin::static_begin(const string& s) : static_common(s) {
	cerr << "START-initialization: " << msg << endl;
}

static_begin::~static_begin() {
	cerr << "END---destruction: " << msg << endl;
}

//=============================================================================

static_here::static_here(const string& s) : static_common(s) {
	cerr << "creating marker: " << msg << endl;
}

static_here::~static_here() {
	cerr << "destroying marker: " << msg << endl;
}

//=============================================================================
static_end::static_end(const string& s) : static_common(s) {
	cerr << "END---initialization: " << msg << endl;
}

static_end::~static_end() {
	cerr << "START-destruction: " << msg << endl;
}

//=============================================================================
}	// end namespace util

