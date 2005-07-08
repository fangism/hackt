/**
	\file "Object/art_object_instance_alias_actuals.cc"
	Method definitions of class instance_alias_info_actuals.
	$Id: art_object_instance_alias_actuals.cc,v 1.1.2.1 2005/07/08 18:15:27 fang Exp $
 */

#include "Object/art_object_instance_alias_actuals.h"
#include <iostream>
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class instance_alias_info_actuals method definitions

ostream&
instance_alias_info_actuals::dump_actuals(ostream& o) const {
	return (actuals ? actuals->dump(o << '<') << '>' : o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (actuals)
		actuals->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, actuals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, actuals);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

