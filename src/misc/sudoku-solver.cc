/**
	\file "sudoku-solver.cc"
	$Id: sudoku-solver.cc,v 1.2 2006/01/22 06:53:14 fang Exp $
 */

#include "misc/sudoku.h"
#include "misc/sudoku-solver.h"
#include <iostream>
#include <fstream>
#include "main/program_registry.h"

using std::list;
using std::ifstream;
using std::endl;
using std::cerr;
using std::cout;
using sudoku::board;
using sudoku::solution;
using HAC::global_options;

namespace sudoku {
//=============================================================================
// class solver method definitions

solver::solver() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char
solver::name[] = "sudoku";

const char
solver::brief_str[] = "Solves sudoku puzzles";

/**
	NOTE: this is the only symbol referenced outside of the
	sudoku modules.  
	To make this a standalone program, all you need to do is define
	main to call solver::main and fake the symbol:
	HAC::register_hackt_program().
	See "sudoku-main.cc" for example. 
 */
const size_t
solver::program_id = HAC::register_hackt_program_class<solver>();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
solver::main(int argc, char* argv[], const global_options&) {
	return main(argc, argv);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
solver::main(int argc, char* argv[]) {
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}
	board b;	// the initial board
	{
		ifstream f;
		f.open(argv[1]);
		if (!f.good()) {
			cerr << "Error opening file: " << argv[1] << endl;
			return 1;
		} else {
			if (b.load(f)) {
				cerr << "Invalid initial board.  Exiting."
					<< endl;
				f.close();
				return 1;
			}
		}
		f.close();
	}
	// show initial board
	b.dump(cout << "Initial board: " << endl);
	// b.dump_state(cout);
	list<solution> sols;	// cumulative solution set

	b.solve(sols);
	// TODO: dump all solutions

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
solver::usage(const char* prog) {
	assert(prog);
	cerr << prog <<
	": solves sudoku puzzles by brute force, by David Fang." << endl;
	cerr << "usage: " << prog << " <board file>" << endl;
	cerr <<
"\tThe board file is a list of whitespace-delimited integer triplets "
	<< endl <<
"\t<x,y,z>, where x,y are the coordinates of a pre-placed number, and z "
	<< endl <<
"\tis its value.  All coordinates and values must be in the range [1-9]." << endl;
	cerr << "\tExample file: " << endl <<
		"\t\t1 4 1\t1 6 4\t1 8 5" << endl <<
		"\t\t2 3 8\t2 6 5\t2 7 6" << endl <<
		"\t\t3 9 1" << endl <<
		"\t\t4 4 4\t4 6 7\t4 9 6" << endl <<
		"\t\t5 3 6\t5 7 3" << endl <<
		"\t\t6 1 7\t6 4 9\t6 6 1" << endl <<
		"\t\t7 1 5\t7 9 2" << endl <<
		"\t\t8 3 7\t8 4 2\t8 7 9" << endl <<
		"\t\t9 2 4\t9 4 5\t9 6 8\t9 8 7" << endl;
}

//=============================================================================
}	// end namespace sudoku

