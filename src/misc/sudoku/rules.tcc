/**
	\file "misc/sudoku/rules.tcc"
	$Id: rules.tcc,v 1.2 2006/01/22 06:53:22 fang Exp $
 */

#ifndef	__SUDOKU_RULES_TCC__
#define	__SUDOKU_RULES_TCC__

#include "misc/sudoku/rules.hh"
#include "misc/sudoku/cell.hh"
#include "misc/sudoku/constraint.hh"
// #include "misc/sudoku/board.hh"
// #include "util/wtf.hh"

namespace sudoku {
//=============================================================================
//=============================================================================
// class classic_rules method definitions
CLASSIC_RULES_TEMPLATE_SIGNATURE
CLASSIC_RULES_CLASS::classic_rules() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CLASSIC_RULES_TEMPLATE_SIGNATURE
CLASSIC_RULES_CLASS::~classic_rules() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param BoardType the type of board.
		concept_check: BoardType::cell_type must be same as cell_type.
	\param b the board for which to setup constraints.  
 */
CLASSIC_RULES_TEMPLATE_SIGNATURE
template <class BoardType>
void
CLASSIC_RULES_CLASS::setup_board(BoardType& b) {
	SUDOKU_ASSERT(b.size() >= 81);
{
	// row constraints
	// column constraints
	size_t i = 0;
	for ( ; i<9; ++i) {
		size_t j = 0;
		for ( ; j<9; j++) {
			const size_t index = 9*i +j;
			// g++-4 gives better error message
			cell_type& c(b[index]);
			constraint_type& rc(row_set[i]);
			constraint_type& cc(col_set[j]);
			rc[j] = &c;
			cc[i] = &c;
			c.add_constraint(&rc);
			c.add_constraint(&cc);
		}
	}
}
	// block constraints
{
	size_t i = 0;
	for ( ; i<9; ++i) {
		size_t j = 0;
		for ( ; j<9; ++j) {
			const size_t index = 9*i +j;
			// util::wtf_is(b);
			cell_type& c(b[index]);
			constraint_type& bc(block_set[i/3][j/3]);
			bc[3*(i%3) +j%3] = &c;
			c.add_constraint(&bc);
		}
	}
}
}	// end method set_board

//=============================================================================
// class diagonal_rules method definitions

DIAGONAL_RULES_TEMPLATE_SIGNATURE
DIAGONAL_RULES_CLASS::diagonal_rules() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIAGONAL_RULES_TEMPLATE_SIGNATURE
DIAGONAL_RULES_CLASS::~diagonal_rules() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIAGONAL_RULES_TEMPLATE_SIGNATURE
template <class BoardType>
void
DIAGONAL_RULES_CLASS::setup_board(BoardType& b) {
	parent_type::setup_board(b);
	// diagonal constraints
	size_t i = 0;
	size_t fi = 0;
	size_t bi = 8;
	for ( ; i<9; ++i, fi += 10, bi += 8) {
		cell_type& fc(b[fi]);
		cell_type& bc(b[bi]);
		fwd_diagonal_set[i] = &fc;
		back_diagonal_set[i] = &bc;
		fc.add_constraint(&fwd_diagonal_set);
		bc.add_constraint(&back_diagonal_set);
	}
}

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_RULES_TCC__

