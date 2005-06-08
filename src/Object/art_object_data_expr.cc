/**
	\file "Object/art_object_data_expr.cc"
	Implementation of data expression classes.  
	$Id: art_object_data_expr.cc,v 1.1.2.2 2005/06/08 04:03:43 fang Exp $
 */

#include <iostream>
#include "Object/art_object_data_expr.h"
#include "Object/art_object_type_hash.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/what.h"

namespace util {
	using ART::entity::nonmeta_index_list;
	SPECIALIZE_UTIL_WHAT(nonmeta_index_list, "nonmeta-index-list")

	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		nonmeta_index_list, NONMETA_INDEX_LIST_TYPE_KEY, 0)
}	// end namespace util

namespace ART {
namespace entity {
using std::istream;
using util::persistent_traits;
//=============================================================================

//=============================================================================
// class nonmeta_index_list method definitions

nonmeta_index_list::nonmeta_index_list() : persistent(), indices() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_index_list::~nonmeta_index_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(nonmeta_index_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
nonmeta_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i(indices.begin());
	const const_iterator e(indices.end());
	for ( ; i!=e; i++) {
		if (i->is_a<const int_expr>())
			ret++;
		else INVARIANT(i->is_a<const int_range_expr>());
			// sanity check
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nonmeta_index_list::dump(ostream& o) const {
	const_iterator i(indices.begin());
	const const_iterator e(indices.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const int_expr>
			b(i->is_a<const int_expr>());
		if (b)
#if 0
			// don't have yet
			b->dump_brief(o << '[') << ']';
#else
			b->dump(o << '[') << ']';
#endif
		else    (*i)->dump(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
nonmeta_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(indices);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
nonmeta_index_list::write_object(const persistent_object_manager& m,
		ostream& f) const {
	m.write_pointer_list(f, indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
nonmeta_index_list::load_object(const persistent_object_manager& m,
		istream& f) {
	m.read_pointer_list(f, indices);
	const_iterator i(indices.begin());
	const const_iterator e(indices.end());
	for ( ; i!=e; i++) {
		const count_ptr<nonmeta_index_expr_base> ip;
		NEVER_NULL(ip);
		// if (ip)
			m.load_object_once(ip);
	}
}

//=============================================================================

//=============================================================================
}	// end namespace entity
}	// end namespace ART

