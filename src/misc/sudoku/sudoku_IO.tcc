/**
	\file "misc/sudoku/sudoku_IO.tcc"
	$Id: sudoku_IO.tcc,v 1.1.2.1 2006/01/14 06:52:08 fang Exp $
 */

#include <iostream>

#ifndef	__SUDOKU_SUDOKU_IO_TCC__
#define	__SUDOKU_SUDOKU_IO_TCC__

#include "misc/sudoku/sudoku_IO.h"
#include "misc/sudoku/board.h"

namespace sudoku {
//=============================================================================
/**
	\return true if there is an error.  
 */
template <class BoardType>
bool
load_linear_board(BoardType& b, istream& i) {
	do {
		ushort x, v;
		f >> x >> v;
		if (x >= b.size() || v >= BoardType::cell_type::max_value()) {
			cerr << "ERROR: all input out of range." << endl;
			return true;
		}
		if (f.good()) {
			if (!b.load_cell(x,v)) {
				cerr << "ERROR: [" << x << "]="
					<< y << " was rejected!" << endl;
				return true;
			}
		}
	} while (f.good());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_SUDOKU_IO_TCC__

