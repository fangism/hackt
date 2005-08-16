/**
	\file "Object/state_manager.cc"
	$Id: state_manager.cc,v 1.2.4.2 2005/08/16 20:32:13 fang Exp $
 */

#include <iostream>
#include "Object/state_manager.h"
#include "Object/devel_switches.h"

#if !USE_MODULE_FOOTPRINT
#include "Object/inst/instance_pool.h"
#include "Object/inst/alias_empty.h"	// for bool and int
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/struct_instance_collection.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/process_instance_collection.h"
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class state_manager method definitions

/**
	Collects persistently manager pointers to objects.  
 */
ostream&
state_manager::dump_state(ostream& o) {
	process_instance::pool.dump(o);
	channel_instance::pool.dump(o);
	struct_instance::pool.dump(o);
	enum_instance::pool.dump(o);
	int_instance::pool.dump(o);
	bool_instance::pool.dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects persistently manager pointers to objects.  
 */
void
state_manager::collect_state(persistent_object_manager& m) {
	process_instance::pool.collect_transient_info_base(m);
	channel_instance::pool.collect_transient_info_base(m);
	struct_instance::pool.collect_transient_info_base(m);
	enum_instance::pool.collect_transient_info_base(m);
	int_instance::pool.collect_transient_info_base(m);
	bool_instance::pool.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves the global allocated state.  
 */
void
state_manager::write_state(const persistent_object_manager& m, ostream& o) {
	process_instance::pool.write_object_base(m, o);
	channel_instance::pool.write_object_base(m, o);
	struct_instance::pool.write_object_base(m, o);
	enum_instance::pool.write_object_base(m, o);
	int_instance::pool.write_object_base(m, o);
	bool_instance::pool.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the global allocated state.  
 */
void
state_manager::load_state(const persistent_object_manager& m, istream& i) {
	process_instance::pool.load_object_base(m, i);
	channel_instance::pool.load_object_base(m, i);
	struct_instance::pool.load_object_base(m, i);
	enum_instance::pool.load_object_base(m, i);
	int_instance::pool.load_object_base(m, i);
	bool_instance::pool.load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART
#endif	// !USE_MODULE_FOOTPRINT

