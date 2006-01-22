/**
	\file "misc/sudoku/sudoku_IO.tcc"
	$Id: sudoku_IO.tcc,v 1.2 2006/01/22 06:53:23 fang Exp $
 */

#include <iostream>

#ifndef	__SUDOKU_SUDOKU_IO_TCC__
#define	__SUDOKU_SUDOKU_IO_TCC__

#include "misc/sudoku/sudoku_IO.h"
#include "misc/sudoku/board.h"

namespace sudoku {
using std::cerr;
using std::endl;
//=============================================================================
/**
	\return true if there is an error.  
 */
template <class BoardType>
bool
load_linear_board(BoardType& b, istream& f) {
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
					<< v << " was rejected!" << endl;
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

