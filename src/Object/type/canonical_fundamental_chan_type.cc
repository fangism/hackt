/**
	\file "Object/type/canonical_fundamental_chan_type.cc"
	$Id: canonical_fundamental_chan_type.cc,v 1.1.2.2 2007/01/10 20:14:27 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/type/canonical_type.h"	// for generic data-type
#include "Object/type/channel_type_reference_base.h"
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
	HAC::entity::canonical_fundamental_chan_type_base, 
	CANONICAL_FUNDAMENTAL_CHANNEL_TYPE_TYPE_KEY, 0)

namespace memory {
using HAC::entity::canonical_fundamental_chan_type_base;

/**
	Comparison operator for set sorting.  
	Dereference-and-compare pointees.  
	TODO: guarantee that this is being used, as opposed to pointer
		address comparison.  
 */
bool
operator < (const count_ptr<const canonical_fundamental_chan_type_base>& l, 
		const count_ptr<const canonical_fundamental_chan_type_base>& r) {
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
canonical_fundamental_chan_type_base::global_registry_type
canonical_fundamental_chan_type_base::global_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private default constructor.  
 */
canonical_fundamental_chan_type_base::canonical_fundamental_chan_type_base() :
		persistent(), 
		datatype_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_fundamental_chan_type_base::canonical_fundamental_chan_type_base(
		const datatype_list_type& d) :
		persistent(), 
		datatype_list(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default destructor
canonical_fundamental_chan_type_base::~canonical_fundamental_chan_type_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
canonical_fundamental_chan_type_base::what(ostream& o) const {
	return o << "canonical-fund-chan-type";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ripped from canonical_generic_chan_type::dump()
 */
ostream&
canonical_fundamental_chan_type_base::dump(ostream& o, const char d) const {
	typedef	datatype_list_type::const_iterator	const_iterator;
	o << "chan";
	channel_type_reference_base::dump_direction(o, d);
	o << '(';
	INVARIANT(datatype_list.size());
	const_iterator i(datatype_list.begin());
	const const_iterator e(datatype_list.end());
	i->dump(o);
	for (i++; i!=e; i++) {
		i->dump(o << ", ");
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default: non-directional.
 */
ostream&
canonical_fundamental_chan_type_base::dump(ostream& o) const {
	return dump(o, '\0');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Comparison for set membership test.  
 */
bool
canonical_fundamental_chan_type_base::operator < (const this_type& r) const {
	return std::lexicographical_compare(
		datatype_list.begin(), datatype_list.end(), 
		r.datatype_list.begin(), r.datatype_list.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type equivalence test.
	Ripped from
	canonical_generic_chan_type::must_be_connectibly_type_equivalent().
 */
bool
canonical_fundamental_chan_type_base::operator == (const this_type& t) const {
	typedef datatype_list_type::const_iterator      const_iterator;
	if (datatype_list.size() != t.datatype_list.size())
		return false;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; i++, j++) {
		if (!i->must_be_connectibly_type_equivalent(*j))
			return false;
	}
	// else everything matches
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a built-in canonical channel type to a unique, re-usable, 
	type description.  
	Use with global registry avoids unnecessary replication.  
	TODO: concern with thread-safety (side-effect)?
 */
count_ptr<const canonical_fundamental_chan_type_base>
canonical_fundamental_chan_type_base::register_type(
		const datatype_list_type& d) {
	typedef	count_ptr<const this_type>		return_type;
	const return_type probe(new this_type(d));
	typedef global_registry_type::iterator		iterator;
	std::pair<iterator, bool> t(global_registry.insert(probe));
	return *t.first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type_base::collect_transient_info(
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
canonical_fundamental_chan_type_base::write_object(
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
canonical_fundamental_chan_type_base::load_object(
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
// class canonical_fundamental_chan_type method definitions

canonical_fundamental_chan_type::canonical_fundamental_chan_type() :
		base_chan_type(), direction('\0') {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_fundamental_chan_type::canonical_fundamental_chan_type(
		const base_chan_ptr_type& c) :
		base_chan_type(c), direction('\0') {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_fundamental_chan_type::~canonical_fundamental_chan_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
canonical_fundamental_chan_type::dump(ostream& o) const {
	return base_chan_type->dump(o, direction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check type equivalence.
	Ignores direction flag for type checking.
 */
bool
canonical_fundamental_chan_type::must_be_connectibly_type_equivalent(
		const this_type& t) const {
	NEVER_NULL(base_chan_type);
	NEVER_NULL(t.base_chan_type);
	const bool ret = (base_chan_type == t.base_chan_type) ||
		(*base_chan_type == *t.base_chan_type);
	if (!ret) {
		type_mismatch_error(cerr, *this, t);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For channels, connectibility == collectibility
	because there are no relaxed parameters.  
 */
bool
canonical_fundamental_chan_type::must_be_collectibly_type_equivalent(
		const this_type& t) const {
	return must_be_connectibly_type_equivalent(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Standard error message for mismatched types.  
 */
ostream&
canonical_fundamental_chan_type::type_mismatch_error(ostream& o,
		const this_type& l, const this_type& r) {
	o << "ERROR: types mismatch!" << endl;
	l.dump(o << "\tgot: ") << endl;
	r.dump(o << "\tand: ") << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::collect_transient_info_base(
		persistent_object_manager& m) const {
	NEVER_NULL(base_chan_type);
	base_chan_type->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, base_chan_type);
	util::write_value(o, direction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, base_chan_type);
	// TODO: load global map! (responsibility here)
	util::read_value(i, direction);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

