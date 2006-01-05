/**
	\file "misc/sodoku-main.cc"
	The stand-alone solver program.  
	$Id: sodoku-main.cc,v 1.1.2.1 2006/01/05 01:15:25 fang Exp $
 */

#include "misc/sodoku-solver.h"
#include "main/program_registry.h"	// just for the following macro
FAKE_REGISTER_HACKT_PROGRAM

int
main(int argc, char* argv[]) {
        return sodoku::solver::main(argc, argv);
}

