/**
	\file "misc/sudoku/solve.hh"
	$Id: solve.h,v 1.2 2006/01/22 06:53:22 fang Exp $
 */

#ifndef	__SUDOKU_SOLVE_H__
#define	__SUDOKU_SOLVE_H__

#include "util/size_t.h"
#include "misc/sudoku/tableau.hh"
#include "misc/sudoku/move.hh"
#include "util/finite_vector.hh"

namespace sudoku {
//=============================================================================
/**
	Functor for solving subproblems recursively.  
 */
TABLEAU_TEMPLATE_SIGNATURE
template <class SolutionsType>
class TABLEAU_CLASS::subproblem_solver {
	/// TODO: determine maximum number of moves to undo
	typedef	util::finite_vector<
		single_move<cell_type>,
		rules_type::MAX_UNDO_SIZE>
					undo_list_type;
private:
	TABLEAU_CLASS&			_this;
	SolutionsType&			sols;
	cell_type&			cell;
	undo_list_type			undo_list;
public:
	subproblem_solver(TABLEAU_CLASS&, SolutionsType&, cell_type&);

	~subproblem_solver();

	void
	operator () (const size_t);
};

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_SOLVE_H__

