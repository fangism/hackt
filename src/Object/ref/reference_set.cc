/**
	\file "Object/ref/reference_set.cc"
	$Id: reference_set.cc,v 1.4 2010/04/02 22:18:46 fang Exp $
 */

#include "Object/ref/reference_set.hh"
#include <iostream>
#include <functional>
#include <algorithm>
#include "Object/entry_collection.hh"
#include "Object/traits/instance_traits.hh"
#include "util/iterator_more.hh"

namespace HAC {
namespace entity {
using std::for_each;
using std::mem_fun_ref;
using util::set_inserter;
#include "util/using_ostream.hh"

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
	Clobbers this set of sets, by taking all sets from the 
	entry collection parameter.  
 */
void
global_references_set::import_entry_collection(const entry_collection& c) {
#define	GRAB_SET(Tag)							\
	ref_bin[class_traits<Tag>::type_tag_enum_value] =		\
		c.get_index_set<Tag>();
	GRAB_SET(bool_tag)
	GRAB_SET(int_tag)
	GRAB_SET(enum_tag)
	GRAB_SET(channel_tag)
	GRAB_SET(process_tag)
#undef	GRAB_SET
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
global_references_set::dump(ostream& o) const {
	typedef ref_bin_type::const_iterator	const_iterator;
#define CASE_PRINT_TYPE_TAG_NAME(Tag)					\
{									\
	const ref_bin_type&						\
		ub(ref_bin[class_traits<Tag>::type_tag_enum_value]);	\
	const_iterator i(ub.begin()), e(ub.end());			\
	for ( ; i!=e; ++i) {						\
		o << class_traits<Tag>::tag_name << '[' << *i << "], ";	\
	}								\
}
	CASE_PRINT_TYPE_TAG_NAME(bool_tag)
	CASE_PRINT_TYPE_TAG_NAME(int_tag)
	CASE_PRINT_TYPE_TAG_NAME(enum_tag)
	CASE_PRINT_TYPE_TAG_NAME(channel_tag)
	CASE_PRINT_TYPE_TAG_NAME(process_tag)
#undef  CASE_PRINT_TYPE_TAG_NAME 
	return o;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

