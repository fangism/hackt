/**
	\file "Object/inst/subinstance_manager.cc"
	Class implementation of the subinstance_manager.
	$Id: subinstance_manager.cc,v 1.1.2.2 2005/07/11 21:40:39 fang Exp $
 */

#include "Object/inst/subinstance_manager.h"
#include "Object/art_object_instance_base.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"

namespace ART {
namespace entity {
//=============================================================================
// class subinstance_manager method definitions

subinstance_manager::subinstance_manager() : subinstance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: not your everyday copy-constructor, needs to 
	re-establish local connections.  
 */
subinstance_manager::subinstance_manager(const this_type& s) :
	subinstance_array() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
subinstance_manager::~subinstance_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::relink_super_instance_alias(
		const substructure_alias& p) {
	iterator i(subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->relink_super_instance(p);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(subinstance_array);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, subinstance_array);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::load_object_base(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer_list(i, subinstance_array);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

