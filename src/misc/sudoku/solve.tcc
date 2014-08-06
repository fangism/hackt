/**
	\file "misc/sudoku/solve.tcc"
	$Id: solve.tcc,v 1.2 2006/01/22 06:53:22 fang Exp $
 */

#ifndef	__SUDOKU_SOLVE_TCC__
#define	__SUDOKU_SOLVE_TCC__

#include "misc/sudoku/solve.hh"

namespace sudoku {
//=============================================================================
TABLEAU_TEMPLATE_SIGNATURE
template <class SolutionsType>
TABLEAU_CLASS::subproblem_solver<SolutionsType>::subproblem_solver(
		TABLEAU_CLASS& t, SolutionsType& s, cell_type& c) :
		_this(t), sols(s), cell(c), undo_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TABLEAU_TEMPLATE_SIGNATURE
template <class SolutionsType>
TABLEAU_CLASS::subproblem_solver<SolutionsType>::~subproblem_solver() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TABLEAU_TEMPLATE_SIGNATURE
template <class SolutionsType>
void
TABLEAU_CLASS::subproblem_solver<SolutionsType>::operator () (const size_t i) {
	if (cell.push(i, this->undo_list)) {
		_this.solve(sols);
	}
}

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_SOLVE_TCC__

