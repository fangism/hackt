/**
	\file "art_persistent_table.cc"
	Just dumps the registered persistent type table.  
 */

#include <iostream>
#include "art++.h"			// has everything you need

using namespace std;

int
main(int argc, char* argv[]) {
	persistent_object_manager::dump_registered_type_map(cerr);
	return 0;
}

