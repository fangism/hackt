/**
	\file "misc/sudoku-main.cc"
	The stand-alone solver program.  
	$Id: sudoku-main.cc,v 1.1.2.1 2006/01/06 07:35:30 fang Exp $
 */

#include "misc/sudoku-solver.h"
#include "main/program_registry.h"	// just for the following macro
FAKE_REGISTER_HACKT_PROGRAM

int
main(int argc, char* argv[]) {
        return sudoku::solver::main(argc, argv);
}

