/**
	\file "misc/sudoku/move.tcc"
	$Id: move.tcc,v 1.2 2006/01/22 06:53:21 fang Exp $
 */

#ifndef	__SUDOKU_MOVE_TCC__
#define	__SUDOKU_MOVE_TCC__

#include "misc/sudoku/move.hh"

namespace sudoku {
//=============================================================================
SINGLE_MOVE_TEMPLATE_SIGNATURE
SINGLE_MOVE_CLASS::~single_move() {
	if (this->cell)
		this->cell->pop();
}

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_MOVE_TCC__

