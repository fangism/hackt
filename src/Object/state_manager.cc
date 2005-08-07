/**
	\file "Object/state_manager.cc"
	$Id: state_manager.cc,v 1.1.2.1 2005/08/07 01:07:25 fang Exp $
 */

#include <iostream>
#include "Object/state_manager.h"
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
	process_instance::dump_pool_state(o);
	channel_instance::dump_pool_state(o);
	struct_instance::dump_pool_state(o);
	enum_instance::dump_pool_state(o);
	int_instance::dump_pool_state(o);
	bool_instance::dump_pool_state(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects persistently manager pointers to objects.  
 */
void
state_manager::collect_state(persistent_object_manager& m) {
	process_instance::collect_pool_state(m);
	channel_instance::collect_pool_state(m);
	struct_instance::collect_pool_state(m);
	enum_instance::collect_pool_state(m);
	int_instance::collect_pool_state(m);
	bool_instance::collect_pool_state(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves the global allocated state.  
 */
void
state_manager::write_state(const persistent_object_manager& m, ostream& o) {
	process_instance::write_pool_state(m, o);
	channel_instance::write_pool_state(m, o);
	struct_instance::write_pool_state(m, o);
	enum_instance::write_pool_state(m, o);
	int_instance::write_pool_state(m, o);
	bool_instance::write_pool_state(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the global allocated state.  
 */
void
state_manager::load_state(const persistent_object_manager& m, istream& i) {
	process_instance::load_pool_state(m, i);
	channel_instance::load_pool_state(m, i);
	struct_instance::load_pool_state(m, i);
	enum_instance::load_pool_state(m, i);
	int_instance::load_pool_state(m, i);
	bool_instance::load_pool_state(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

