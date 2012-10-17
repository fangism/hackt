/**
	\file "main/force_load.cc"
	Forces linker to load modules.  
	Only needed for compilers that use lazy-linkage, 
		e.g. darwin-gcc.  
	$Id: force_load.cc,v 1.13 2010/04/05 23:59:44 fang Exp $
 */

#include "config.h"
#include "main/force_load.hh"
#include "Object/module.hh"
#include "main/compile.hh"
#include "main/haco.hh"
#include "main/flatten.hh"
#include "main/parse_test.hh"
#include "main/create.hh"
#include "main/objdump.hh"
#include "main/cflat.hh"
#include "main/prsim.hh"
#include "main/prsobjdemo.hh"
#include "main/dump_persistent_table.hh"
#include "main/shell.hh"
#include "main/version.hh"
#include "main/chpsim.hh"
#include "main/hacknet.hh"

#if	WANT_TO_HAVE_FUN
#include "misc/sudoku-solver.hh"
#endif

namespace HAC {

/***
	Problem: compiler might be so smart that it won't
	call static initializations of the HAC classes, unless
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
	const haco hacker;
	const flatten flattener;
	const parse_test parse_tester;
	const create creator;
	const cflat cflattener;
	const objdump objdumper;
	const prsim prsimmer;
	const prsobjdemo prsobjdemoer;
	const dump_persistent_table persistent_dumper;
	const version versionator;
	const shell shell_shocker;
	const chpsim chpsimulator;
	const hacknet hacknetter;
#if WANT_TO_HAVE_FUN
	const sudoku::solver ss;
#endif
}

}	// end namespace HAC

