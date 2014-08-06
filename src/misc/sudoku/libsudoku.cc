/**
	\file "misc/sudoku/libsudoku.cc"
	Template instantiations for sudoku variations. 
	$Id: libsudoku.cc,v 1.2 2006/01/22 06:53:21 fang Exp $
 */

#include "misc/sudoku/libsudoku.hh"
#include "misc/sudoku/tableau.tcc"
#include "misc/sudoku/solve.tcc"
#include "misc/sudoku/cell.tcc"
#include "misc/sudoku/cell_state.hh"
#include "misc/sudoku/board.tcc"
#include "misc/sudoku/rules.tcc"
#include "misc/sudoku/constraint.tcc"
#include "misc/sudoku/move.tcc"
#include "misc/sudoku/pivot.hh"

namespace sudoku {
using util::finite_vector_size_binder;
//=============================================================================
// classic variation

#if 1
// accepted by all
template class tableau<classic_board_type, classic_rule_set>;
#else
// explicit instantiation of template using typedef
// g++-3.3 accepts, but not g++-4
template classic_variation;
#endif

//=============================================================================
// diagonal variation

template class tableau<diagonal_board_type, diagonal_rule_set>;

//=============================================================================
}	// end namespace sudoku

