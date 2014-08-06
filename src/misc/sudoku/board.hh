/**
	\file "misc/sudoku/board.hh"
	$Id: board.h,v 1.2 2006/01/22 06:53:18 fang Exp $
 */

#ifndef	__SUDOKU_BOARD_H__
#define	__SUDOKU_BOARD_H__

#include "misc/sudoku/debug.hh"
#include "misc/sudoku/inttypes.hh"
#include "misc/sudoku/move.hh"
#include "util/finite_vector.hh"

namespace sudoku {
//=============================================================================
/**
	Interface requirements for board classes:
	* RandomAccessContainer<cell_type>
	* must be able to reference any cell randomly.
 */
template <class BoardType>
struct board_interface {
public:
	typedef	BoardType				board_type;
	typedef	typename board_type::cell_type		cell_type;
	// member function requirements:
	// board_type::size(void) const;
	// board_type::operator[](const size_t) const;
};	// end class board_interface

//=============================================================================
#define	BOARD_TEMPLATE_SIGNATURE		template <class CellType>
#define BOARD_CLASS				board<CellType>
template <class CellType>
class board {
};	// end class board

//=============================================================================
#define	CLASSIC_BOARD_TEMPLATE_SIGNATURE	BOARD_TEMPLATE_SIGNATURE
#define	CLASSIC_BOARD_CLASS			classic_board<CellType>
/**
	The classic game uses a 9x9 board.  
	We internally represent the board as a single linear array
	for sequential access purposes.  
 */
CLASSIC_BOARD_TEMPLATE_SIGNATURE
class classic_board {
public:
	typedef	CellType				cell_type;
private:
	enum {
		SIZE = 81
	};
	typedef	util::finite_vector<single_move<cell_type>, SIZE>
							undo_list_type;
	cell_type					cells[SIZE];
	/**
		Board initialization is completely reversible.  
	 */
	undo_list_type					undo_list;
public:
	classic_board();
	~classic_board();

	size_t
	size(void) const { return SIZE; }

	cell_type&
	operator [] (const size_t i) {
		SUDOKU_ASSERT(i < SIZE);
		return cells[i];
	}

	const cell_type&
	operator [] (const size_t i) const {
		SUDOKU_ASSERT(i < SIZE);
		return cells[i];
	}

	bool
	load_cell(const size_t, const uchar);

	bool
	load_cell(const size_t, const size_t, const uchar);

	template <class F>
	void
	accept(F&);

	template <class F>
	void
	accept(F&) const;

};	// end class classic_board

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_BOARD_H__

