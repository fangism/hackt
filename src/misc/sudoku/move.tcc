/**
	\file "misc/sudoku/move.tcc"
	$Id: move.tcc,v 1.1.2.1 2006/01/14 06:52:07 fang Exp $
 */

#ifndef	__SUDOKU_MOVE_TCC__
#define	__SUDOKU_MOVE_TCC__

#include "misc/sudoku/move.h"

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

