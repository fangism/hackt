/**
	\file "misc/sudoku/sudoku_IO.h"
	Input and output related functions.  
	$Id: sudoku_IO.h,v 1.1.2.1 2006/01/14 06:52:08 fang Exp $
 */

#ifndef	__SUDOKU_SUDOKU_IO_H__
#define	__SUDOKU_SUDOKU_IO_H__

#include <iosfwd>
#include "misc/sudoku/board.h"

namespace sudoku {
using std::istream;
using std::ostream;

template <class BoardType>
bool
load_linear_board(BoardType&, istream&);

template <class BoardType>
bool
load_rectangular_board(BoardType&, istream&);

}	// end namespace sudoku

#endif	// __SUDOKU_SUDOKU_IO_H__

