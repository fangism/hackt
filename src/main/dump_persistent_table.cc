/**
	\file "main/dump_persistent_table.cc"
	Just dumps the registered persistent type table.  
	This file came from "art_persistent_table.cc".
	$Id: dump_persistent_table.cc,v 1.6 2007/04/15 05:52:25 fang Exp $
 */

#include <iostream>
#include "main/dump_persistent_table.hh"
#include "main/program_registry.hh"
#include "util/persistent_object_manager.hh"
#include "util/what.tcc"	// use default typeinfo-based mangled names
#include "Object/module.hh"

#include "common/sizes-common.hh"
#include "AST/sizes-AST.hh"
#include "Object/sizes-entity.hh"

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

