/**
	\file "sudoku-solver.h"
	$Id: sudoku-solver.h,v 1.2 2006/01/22 06:53:15 fang Exp $
 */

#ifndef	__SUDOKU_SOLVER_H__
#define	__SUDOKU_SOLVER_H__

#include "main/hackt_fwd.hh"

namespace sudoku {

class solver {
public:
	static const char		name[];
	static const char		brief_str[];

        solver();

	static
	int
	main(const int, char*[]);

	static
	int
	main(const int, char*[], const HAC::global_options&);

	static
	void
	usage(const char*);

private:
#if 0
	static
	int
	parse_command_options(const int, char*[], options&);
#endif

	static
	const size_t
	program_id;

};	// end class solver

}	// end namespace sudoku

#endif	// __SUDOKU_SOLVER_H__

