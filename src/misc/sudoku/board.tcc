/**
	\file "misc/sudoku/board.tcc"
	$Id: board.tcc,v 1.2 2006/01/22 06:53:18 fang Exp $
 */

#ifndef	__SUDOKU_BOARD_TCC__
#define	__SUDOKU_BOARD_TCC__

#include "misc/sudoku/board.hh"

namespace sudoku {
//=============================================================================
// class classic_board method definitions

/// default construct all member cells
CLASSIC_BOARD_TEMPLATE_SIGNATURE
CLASSIC_BOARD_CLASS::classic_board() : undo_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// default destructor
CLASSIC_BOARD_TEMPLATE_SIGNATURE
CLASSIC_BOARD_CLASS::~classic_board() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return whether or not the load was accepted.  
	\pre rules and constraints have already been loaded.
 */
CLASSIC_BOARD_TEMPLATE_SIGNATURE
bool
CLASSIC_BOARD_CLASS::load_cell(const size_t i, const uchar v) {
	if (i >= SIZE)
		return false;
	// if v is our of range, reject
	return cells[i].push(v, this->undo_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return whether or not the load was accepted.  
	\pre rules and constraints have already been loaded.
 */
CLASSIC_BOARD_TEMPLATE_SIGNATURE
bool
CLASSIC_BOARD_CLASS::load_cell(const size_t i, const size_t j, const uchar v) {
	if (i >= 9 || j >= 9)
		return false;
	// if v is our of range, reject
	return cells[9*i +j].push(v, this->undo_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor visitor, modifying.  
 */
CLASSIC_BOARD_TEMPLATE_SIGNATURE
template <class F>
void
CLASSIC_BOARD_CLASS::accept(F& f) {
	size_t i = 0;
	for ( ; i<SIZE; ++i) {
		f(cells[i]);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor visitor, non-modifying.  
 */
CLASSIC_BOARD_TEMPLATE_SIGNATURE
template <class F>
void
CLASSIC_BOARD_CLASS::accept(F& f) const {
	size_t i = 0;
	for ( ; i<SIZE; ++i) {
		f(cells[i]);
	}
}

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_BOARD_TCC__

