/**
	\file "art_persistent_table.cc"
	Just dumps the registered persistent type table.  
 */

#include <iostream>
#include "art++.h"			// has everything you need

using namespace std;
using util::persistent_object_manager;
using ART::entity::module;

int
main(int argc, char* argv[]) {
	persistent_object_manager::dump_registered_type_map(cerr);
/***
	Problem: compiler might be so smart that it won't
	call static initializations of the ART classes, unless
	they are needed, as can be deduced from the call-graph.  

	Quote from a correspondence:
	It has to do with the fact that the C++ language does not require this 
	behavior, and Mac OS X takes an appropriate and valid approach of 
	deferring static object creation until code in the library is actually 
	called for the first time.

	Thus we force initialization with the following code.
	For kicks, try commenting it out, and see the resulting output.  
	If that's not enough of a shock, compare the size of the 
	resulting binaries.  
***/

#if 1
	// this code does nothing but alter the potential call-graph
	module the_module("-stdin-");
	assert(persistent_object_manager::self_test_no_file(the_module));
#endif

	return 0;
}

