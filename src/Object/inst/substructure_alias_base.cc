/**
	\file "Object/inst/substructure_alias_base.cc"
	$Id: substructure_alias_base.cc,v 1.1.2.2 2005/07/11 21:40:39 fang Exp $
 */

#include "Object/inst/substructure_alias_base.h"

namespace ART {
namespace entity {
//=============================================================================

void
substructure_alias::collect_transient_info_base(
		persistent_object_manager& m) const {
	subinstances.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
substructure_alias::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	subinstances.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Once the children pointers have been loaded, this can
	go in and restore the link to the parents (this).  
 */
void
substructure_alias::load_object_base(const persistent_object_manager& m,
		istream& i) {
	subinstances.load_object_base(m, i);
	subinstances.relink_super_instance_alias(*this);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

