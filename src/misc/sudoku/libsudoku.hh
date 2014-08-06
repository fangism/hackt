/**
	\file "misc/sudoku/libsudoku.hh"
	Typedefs for sudoku variations. 
	$Id: libsudoku.h,v 1.2 2006/01/22 06:53:21 fang Exp $
 */

#include "misc/sudoku/tableau.hh"
#include "misc/sudoku/solve.hh"
#include "misc/sudoku/cell.hh"
#include "misc/sudoku/cell_state.hh"
#include "misc/sudoku/board.hh"
#include "misc/sudoku/rules.hh"
#include "misc/sudoku/constraint.hh"
// #include "misc/sudoku/move.hh"
// #include "misc/sudoku/pivot.hh"
#include "util/finite_vector.hh"

namespace sudoku {
using util::finite_vector_size_binder;
//=============================================================================
// classic variation

typedef	cell_state<pigeonholes<9> >		classic_cell_state;
// limit to 4 constraints (only 3 are needed)
typedef	cell_set<9, cell, classic_cell_state,
		finite_vector_size_binder<4>::type>	classic_cell_set;
typedef	finite_vector_size_binder<4>::type<classic_cell_set*>
							classic_constraint_set;
typedef	cell<classic_constraint_set, classic_cell_state>	classic_cell;
typedef	classic_board<classic_cell>			classic_board_type;
typedef	classic_rules<classic_constraint_set>		classic_rule_set;
typedef	tableau<classic_board_type, classic_rule_set>	classic_variation;

//=============================================================================
// diagonal variation

typedef	classic_cell_state				diagonal_cell_state;
typedef	classic_cell_set				diagonal_cell_set;
typedef	finite_vector_size_binder<4>::type<diagonal_cell_set*>
							diagonal_constraint_set;
typedef	cell<diagonal_constraint_set, diagonal_cell_state>	diagonal_cell;
typedef	classic_board<diagonal_cell>			diagonal_board_type;
typedef	diagonal_rules<diagonal_constraint_set>		diagonal_rule_set;
typedef	tableau<diagonal_board_type, diagonal_rule_set>	diagonal_variation;

//=============================================================================
}	// end namespace sudoku

