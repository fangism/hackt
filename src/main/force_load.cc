/**
	\file "main/force_load.cc"
	Forces linker to load modules.  
	Only needed for compilers that use lazy-linkage, 
		e.g. darwin-gcc.  
	$Id: force_load.cc,v 1.4.8.1 2005/11/03 02:16:36 fang Exp $
 */

#include "main/force_load.h"
#include "Object/module.h"
#include "main/compile.h"
#include "main/parse_test.h"
#include "main/unroll.h"
#include "main/create.h"
#include "main/objdump.h"
#include "main/alloc.h"
#include "main/cflat.h"
#include "main/prsobjdemo.h"
#include "main/dump_persistent_table.h"
#include "main/shell.h"
#include "main/version.h"

namespace ART {

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

	TODO: use configure to determine whether or not this is necessary.
***/
void
force_load(void) {
	// this is sufficient to induce non-lazy linking
	const entity::module the_module("anonymodule");
	// if declared const, compiler will complain uninitialized
	// unless a custom empty constructor is available
	const compile compiler;
	const parse_test parse_tester;
	const unroll unroller;
	const create creator;
	const alloc allocator;
	const cflat cflattener;
	const objdump objdumper;
	const prsobjdemo prsobjdemoer;
	const dump_persistent_table persistent_dumper;
	const version versionator;
	const shell shell_shocker;
}

}	// end namespace ART

