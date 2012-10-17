/**
	\file "util/uninitialized.cc"
	Utility for explicit uninitialized construction.
	$Id: uninitialized.cc,v 1.1 2009/03/14 01:46:23 fang Exp $
 */

#include "util/uninitialized.hh"

namespace util {
// construct global object
const uninitialized_tag_type
uninitialized_tag = uninitialized_tag_type();

}	// end namespace util

