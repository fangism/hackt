/**
	\file "Object/def/footprint_manager.cc"
	Implementation of footprint_manager class. 
	$Id: footprint_manager.cc,v 1.1.2.2 2005/08/11 03:40:53 fang Exp $
 */

#include <iostream>
#include "util/macros.h"
#include "Object/def/footprint_manager.h"
#include "util/persistent_object_manager.tcc"
#include "util/IO_utils.h"

namespace ART {
namespace entity {
using util::write_value;
using util::read_value;
//=============================================================================
// class footprint_manager method definitions

// see if this default constructor can be avoided
footprint_manager::footprint_manager() :
		parent_type(), _arity(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_manager::footprint_manager(const size_t N) :
		parent_type(), _arity(N) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_manager::~footprint_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_manager::set_arity(const size_t a) {
	INVARIANT(!size());
	INVARIANT(!_arity);
	_arity = a;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
footprint_manager::dump(ostream& o) const {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped associative lookup, making sure key has correct arity.  
	NOTE: key comparison is done with std::lexicographical_compare().  
	\param k set of constant template parameters as key 
		for footprint lookup.  
 */
footprint_manager::mapped_type&
footprint_manager::operator [] (const key_type& k) {
	INVARIANT(k.size() == _arity);
	return parent_type::operator[](k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to the only footprint in the map.  
	\pre this map has zero arity, only contains one footprint.  
 */
footprint_manager::mapped_type&
footprint_manager::only(void) {
	INVARIANT(!_arity);
	INVARIANT(size() == 1);
	return begin()->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to the only footprint in the map.  
	\pre this map has zero arity, only contains one footprint.  
 */
const footprint_manager::mapped_type&
footprint_manager::only(void) const {
	INVARIANT(!_arity);
	INVARIANT(size() == 1);
	return begin()->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects reachable pointers in the footprint_map.  
 */
void
footprint_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		i->first.collect_transient_info_base(m);
		i->second.collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves a footprint map.  
 */
void
footprint_manager::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, _arity);
	write_value(o, size());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		i->first.write_object(m, o);	// same as write_object_base
		i->second.write_object_base(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores a footprint map.  
 */
void
footprint_manager::load_object_base(
		const persistent_object_manager& m, istream& i) {
	read_value(i, _arity);
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; j++) {
		// read in key-value pairs
		key_type temp_key;
		temp_key.load_object(m, i);
		// load value in-place
		(*this)[temp_key].load_object_base(m, i);
	}
	INVARIANT(size() == s);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

