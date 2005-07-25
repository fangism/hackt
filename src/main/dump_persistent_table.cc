/**
	\file "main/dump_persistent_table.cc"
	Just dumps the registered persistent type table.  
	This file came from "art_persistent_table.cc".
	$Id: dump_persistent_table.cc,v 1.1 2005/07/25 02:10:07 fang Exp $
 */

#include <iostream>
#include "main/dump_persistent_table.h"
#include "main/program_registry.h"
#include "util/persistent_object_manager.h"
#include "Object/module.h"

namespace ART {

using std::cerr;
using util::persistent_object_manager;
using ART::entity::module;

//=============================================================================
// class dump_persistent_table static initializers

const char
dump_persistent_table::name[] = "dump_persistent_table";

const char
dump_persistent_table::brief_str[] = "Dumps persistent type registry.";

const size_t
dump_persistent_table::program_id =
	register_hackt_program(dump_persistent_table::name, 
		dump_persistent_table::main, 
		dump_persistent_table::brief_str);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dump_persistent_table::dump_persistent_table() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
dump_persistent_table::main(int argc, char* argv[], const global_options&) {
	// just print out the registered persistent type map's pairs
	persistent_object_manager::dump_registered_type_map(cerr);
	// NOTE: comment on lazy-linking moved to "main/force_load.cc"
	return 0;
}
//=============================================================================

}	// end namespace ART

