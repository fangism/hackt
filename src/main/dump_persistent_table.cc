/**
	\file "main/dump_persistent_table.cc"
	Just dumps the registered persistent type table.  
	This file came from "art_persistent_table.cc".
	$Id: dump_persistent_table.cc,v 1.6 2007/04/15 05:52:25 fang Exp $
 */

#include <iostream>
#include "main/dump_persistent_table.h"
#include "main/program_registry.h"
#include "util/persistent_object_manager.h"
#include "util/what.tcc"	// use default typeinfo-based mangled names
#include "Object/module.h"

#include "common/sizes-common.h"
#include "AST/sizes-AST.h"
#include "Object/sizes-entity.h"

namespace HAC {
using std::ostream;
using std::cerr;
using std::endl;
using util::persistent_object_manager;
using HAC::entity::module;

//=============================================================================
// class dump_persistent_table static initializers

const char
dump_persistent_table::name[] = "dump_persistent_table";

const char
dump_persistent_table::brief_str[] = "Dumps persistent type registry.";

const size_t
dump_persistent_table::program_id =
	register_hackt_program_class<dump_persistent_table>();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dump_persistent_table::dump_persistent_table() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
dump_persistent_table::main(int, char*[], const global_options&) {
	// just print out the registered persistent type map's pairs
	persistent_object_manager::dump_registered_type_map(cerr);
	// NOTE: comment on lazy-linking moved to "main/force_load.cc"

	// print structure sizes
	HAC::dump_class_sizes(cerr);
	parser::dump_class_sizes(cerr);
	entity::dump_class_sizes(cerr);
	return 0;
}

//=============================================================================
}	// end namespace HAC

