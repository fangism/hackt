/**
	\file "sodoku-solver.h"
	$Id: sodoku-solver.h,v 1.1.2.1 2006/01/05 01:15:26 fang Exp $
 */

#ifndef	__SODOKU_SOLVER_H__
#define	__SODOKU_SOLVER_H__

#include "main/hackt_fwd.h"

namespace sodoku {

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

}	// end namespace sodoku

#endif	// __SODOKU_SOLVER_H__

