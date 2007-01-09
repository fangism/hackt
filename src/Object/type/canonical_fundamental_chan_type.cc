/**
	\file "Object/type/canonical_fundamental_chan_type.cc"
	$Id: canonical_fundamental_chan_type.cc,v 1.1.2.1 2007/01/09 19:30:39 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/type/canonical_type.h"	// for generic data-type
#include "Object/def/datatype_definition_base.h"
#include "Object/persistent_type_hash.h"
#include "Object/expr/const_param_expr_list.h"

#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.h"
#include "util/IO_utils.tcc"

namespace util {
// TODO: specialize reconstructor for de-serialization
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::canonical_fundamental_chan_type, 
	CANONICAL_FUNDAMENTAL_CHANNEL_TYPE_TYPE_KEY, 0)

namespace memory {
using HAC::entity::canonical_fundamental_chan_type;

/**
	Comparison operator for set sorting.  
	Dereference-and-compare pointees.  
	TODO: guarantee that this is being used, as opposed to pointer
		address comparison.  
 */
bool
operator < (const count_ptr<const canonical_fundamental_chan_type>& l, 
		const count_ptr<const canonical_fundamental_chan_type>& r) {
	NEVER_NULL(l);
	NEVER_NULL(r);
	return *l < *r;
}

}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
// using util::memory::operator<;
using util::persistent_traits;

//=============================================================================
// class canonical_fundamental_chan_type method definitions

/**
	Global set of all instantiated built-in channel types.  
	Instantiated types should register and check with this set
	to eliminate redundancy.  
 */
canonical_fundamental_chan_type::global_registry_type
canonical_fundamental_chan_type::global_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private default constructor.  
 */
canonical_fundamental_chan_type::canonical_fundamental_chan_type() :
		persistent(), 
		datatype_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_fundamental_chan_type::canonical_fundamental_chan_type(
		const datatype_list_type& d) :
		persistent(), 
		datatype_list(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default destructor
canonical_fundamental_chan_type::~canonical_fundamental_chan_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
canonical_fundamental_chan_type::what(ostream& o) const {
	return o << "canonical-fund-chan-type";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
canonical_fundamental_chan_type::dump(ostream& o) const {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Comparison for set membership test.  
 */
bool
canonical_fundamental_chan_type::operator < (const this_type& r) const {
	return std::lexicographical_compare(
		datatype_list.begin(), datatype_list.end(), 
		r.datatype_list.begin(), r.datatype_list.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a built-in canonical channel type to a unique, re-usable, 
	type description.  
	Use with global registry avoids unnecessary replication.  
	TODO: concern with thread-safety (side-effect)?
 */
count_ptr<const canonical_fundamental_chan_type>
canonical_fundamental_chan_type::register_type(const datatype_list_type& d) {
	typedef	count_ptr<const canonical_fundamental_chan_type>
					return_type;
	const return_type probe(new canonical_fundamental_chan_type(d));
	typedef global_registry_type::iterator		iterator;
	std::pair<iterator, bool> t(global_registry.insert(probe));
	return *t.first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	for_each(datatype_list.begin(), datatype_list.end(), 
		util::persistent_collector_ref(m)
	);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::write_object(
		const persistent_object_manager& m, ostream& o) const {
	const size_t s = datatype_list.size();
	util::write_value(o, s);
	for_each(datatype_list.begin(), datatype_list.end(), 
		util::persistent_writer<canonical_generic_datatype>(
			&canonical_generic_datatype::write_object_base, m, o)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: pointers are loaded into a heap-allocated structure, 
	but need to end up in the globally-managed set.
	Thus, the caller, in addition to calling m.read_pointer(), 
	needs to call a substituting function to load the structure into 
	a map and get the mapped pointer.  
	This effectively transfers data from non-mapped to mapped pointer.
	Don't forget to call m.mark_delete(), if necessary.
 */
void
canonical_fundamental_chan_type::load_object(
		const persistent_object_manager& m, istream& i) {
	size_t s;
	util::read_value(i, s);
	datatype_list.resize(s);
	for_each(datatype_list.begin(), datatype_list.end(), 
		util::persistent_loader<canonical_generic_datatype>(
			&canonical_generic_datatype::load_object_base, m, i)
	);
	// Q: need to load the datatypes because need to immediately
	// sort using datatypes as keys for ordering comparison?
	// A: no, only addresses, not pointees, are needed in comparison!
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

