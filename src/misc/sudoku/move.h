/**
	\file "misc/sudoku/move.h"
	$Id: move.h,v 1.1.4.2 2006/01/15 22:25:36 fang Exp $
 */

#ifndef	__SUDOKU_MOVE_H__
#define	__SUDOKU_MOVE_H__

#include "util/macros.h"

namespace sudoku {
#define	SINGLE_MOVE_TEMPLATE_SIGNATURE		template <class CellType>
#define	SINGLE_MOVE_CLASS			single_move<CellType>
/**
	PROBLEM: if this needs to be copied, then we need nullify-on-copy
		semantics, else will result in multiple pops. 
	SOLUTION: just make this NOT copy-constructible.  
	\param CellType is the cell_type that must support a pop()
		operation to undo the last state modification.  
 */
template <class CellType>
class single_move {
	typedef	CellType		cell_type;
	cell_type*			cell;
public:
	single_move() : cell(NULL) { }

	explicit
	single_move(cell_type* const c) : cell(c) { }

	~single_move();
private:
	/**
		Intentionally undefined private copy-constructor.  
	 */
	explicit
	single_move(const single_move&);

};	// end class CellType

}	// end namespace sudoku

#endif	// __SUDOKU_MOVE_H__

