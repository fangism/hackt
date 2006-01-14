/**
	\file "misc/sudoku/pivot.h"
	$Id: pivot.h,v 1.1.2.1 2006/01/14 06:52:07 fang Exp $
 */

#ifndef	__SUDOKU_PIVOT_H__
#define	__SUDOKU_PIVOT_H__

#include "misc/sudoku/inttypes.h"
#include "misc/sudoku/debug.h"

namespace sudoku {
//=============================================================================
/**
	Functor for finding the most favorable place to initiate
	a brute force search: one with the fewest possible choices remaining.  
 */
template <class CellType>
struct cell_pivot_finder {
	typedef	CellType			cell_type;
	typedef	typename cell_type::state_type	state_type;
	cell_type*				best_cell;
	uchar					min_avail;

	cell_pivot_finder() : best_cell(NULL),
		min_avail(cell_type::invalid_value()) { }
	~cell_pivot_finder() { }

	bool
	is_valid(void) const { return best_cell; }

	cell_type&
	cell(void) const { return *best_cell; }

	void
	operator () (cell_type& c) {
		const state_type& s(c.top());
		if (!s.is_assigned()) {
			const uchar avail = s.num_avail();
			SUDOKU_ASSERT(avail);
			if (avail < min_avail) {
				best_cell = &c;
				min_avail = avail;
			}
		}
	}

};	// end class cell_pivot

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_PIVOT_H__

