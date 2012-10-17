/**
	\file "util/memory/deallocation_policy.cc"
	Definition of frequently used deallocation policies.  
	$Id: deallocation_policy.cc,v 1.1 2011/02/04 02:23:41 fang Exp $
 */

#include <cassert>
#include <iostream>
#include "util/memory/deallocation_policy.hh"

namespace util {
namespace memory {
using std::istream;
using std::ostream;

//-----------------------------------------------------------------------------
void
istream_tag::operator () (istream* s) const {
	if (s != &std::cin) {
		delete s;
	}
}

//-----------------------------------------------------------------------------
void
ostream_tag::operator () (ostream* s) const {
	if (s != &std::cout && s != &std::cerr) {
		delete s;
	}
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

