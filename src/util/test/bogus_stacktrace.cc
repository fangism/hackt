/**
	\file "bogus_stacktrace.cc"
	Implementation of bogus_stacktrace class.
	$Id: bogus_stacktrace.cc,v 1.3 2005/09/04 21:15:10 fang Exp $
 */

// ENABLE_STACKTRACE is forced for this module, regardless of pre-definitions!
#define	ENABLE_STACKTRACE	1

#include "bogus_stacktrace.hh"
#include "util/static_trace.hh"

STATIC_TRACE_BEGIN("bogus_stacktrace")

namespace util {
// macro to guarantee proper orderly initialization
REQUIRES_BOGUS_STACKTRACE_STATIC_INIT

//=============================================================================
// class bogus_stacktrace method definitions

bogus_stacktrace::bogus_stacktrace(const string&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bogus_stacktrace::~bogus_stacktrace() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bogus_stacktrace::init_token&
bogus_stacktrace::require_static_init(void) {
	static init_token tok;
	return tok;
}

//=============================================================================
// class bogus_stacktrace::init_token method definitions

/**
	This will hold onto a reference count, intended to be kept
	per-module that requires static initialization of bogus_stacktrace.  
 */
bogus_stacktrace::init_token::init_token() { }

/**
	When the last init_token is destroyed (among all modules)
 */
bogus_stacktrace::init_token::~init_token() { }

void
bogus_stacktrace::init_token::check(void) const {
}

//=============================================================================
}	// end namespace util

STATIC_TRACE_END("bogus_stacktrace")

