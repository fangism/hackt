/**
	\file "Object/def/footprint_manager.cc"
	Implementation of footprint_manager class. 
	$Id: footprint_manager.cc,v 1.1.2.1 2005/08/10 20:30:53 fang Exp $
 */

#include <iostream>
#include "util/macros.h"
#include "Object/def/footprint_manager.h"
#include "util/persistent_object_manager.tcc"

namespace ART {
namespace entity {
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

//=============================================================================
}	// end namespace entity
}	// end namespace ART

