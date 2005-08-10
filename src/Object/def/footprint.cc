/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.1.2.1 2005/08/10 20:30:53 fang Exp $
 */

#include "util/hash_specializations.h"
#include "Object/def/footprint.h"
#include "util/persistent_object_manager.tcc"
#include "util/hash_qmap.tcc"
#include "util/IO_utils.h"

namespace ART {
namespace entity {
using util::write_value;
using util::read_value;
//=============================================================================
// class footprint method definitions

footprint::footprint() :
		unrolled(false), created(false),
		instance_collection_map() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::~footprint() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if 0
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	write_value(o, unrolled);
	write_value(o, created);
	// instance_collection_map
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::load_object_base(const persistent_object_manager& m, istream& i) {
	read_value(i, unrolled);
	read_value(i, created);
	// instance_collection_map
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

