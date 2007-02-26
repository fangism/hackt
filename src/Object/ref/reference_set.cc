/**
	\file "Object/ref/reference_set.cc"
	$Id: reference_set.cc,v 1.1.2.2 2007/02/26 06:11:53 fang Exp $
 */

#include "Object/ref/reference_set.h"
#include <functional>
#include <algorithm>
#include "util/iterator_more.h"

namespace HAC {
namespace entity {
using std::for_each;
using std::mem_fun_ref;
using util::set_inserter;

//=============================================================================
/**
	Clears all member sets.  
 */
void
global_references_set::clear(void) {
	for_each(&ref_bin[0], &ref_bin[MAX], mem_fun_ref(&ref_bin_type::clear));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if all sub-sets are empty
 */
bool
global_references_set::empty(void) const {
	size_t i = 0;
	do {
		if (!ref_bin[i].empty()) {
			return false;
		}
		++i;
	} while (i<MAX);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Insert the set differences into the destination set.  
	Difference: this set - src set
 */
void
global_references_set::set_difference(const global_references_set& src, 
		global_references_set& dst) const {
	size_t i = 0;
	do {
		// linear time complexity
		std::set_difference(ref_bin[i].begin(), ref_bin[i].end(),
			src.ref_bin[i].begin(), src.ref_bin[i].end(), 
			set_inserter(dst.ref_bin[i]));
		++i;
	} while (i<MAX);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Insert the set intersections into the destination set.  
 */
void
global_references_set::set_intersection(const global_references_set& src, 
		global_references_set& dst) const {
	size_t i = 0;
	do {
		// linear time complexity
		std::set_intersection(ref_bin[i].begin(), ref_bin[i].end(),
			src.ref_bin[i].begin(), src.ref_bin[i].end(), 
			set_inserter(dst.ref_bin[i]));
		++i;
	} while (i<MAX);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

