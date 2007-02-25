/**
	\file "Object/ref/reference_set.cc"
	$Id: reference_set.cc,v 1.1.2.1 2007/02/25 19:54:40 fang Exp $
 */

#include "Object/ref/reference_set.h"
#include <functional>
#include <algorithm>

namespace HAC {
namespace entity {
using std::for_each;
using std::mem_fun_ref;
//=============================================================================

void
global_references_set::clear(void) {
	for_each(&ref_bin[0], &ref_bin[MAX], mem_fun_ref(&ref_bin_type::clear));
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

