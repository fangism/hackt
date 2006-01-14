/**
	\file "misc/sudoku/constraint.tcc"
	$Id: constraint.tcc,v 1.1.2.1 2006/01/14 06:52:05 fang Exp $
 */

#ifndef	__SUDOKU_CONSTRAINT_TCC__
#define	__SUDOKU_CONSTRAINT_TCC__

#include "misc/sudoku/debug.h"
#include "misc/sudoku/constraint.h"

namespace sudoku {
//=============================================================================
// class pigeonholes method definitions

template <size_t N>
template <class ValueContainer>
void
pigeonholes<N>::collect_possible_values(ValueContainer& vc) const {
	state_type mask = this->state;
	size_t val = 0;
	for ( ; mask; mask >>= 1, ++val) {
		if (mask & 0x1) {
			vc.push_back(val);
		}
	}
}

//=============================================================================
// class cell_set method definitions

/**
	Attempts to rule-out values in cells.  
	\return true if successful.  
 */
CELL_SET_TEMPLATE_SIGNATURE
template <class UndoListType>
bool
CELL_SET_CLASS::try_value(const member_type skip, const uchar val, 
		UndoListType& ul) const {
	size_t i = 0;
	for ( ; i<N; i++) {
		const member_type c = members[i];
		SUDOKU_ASSERT(c);
		// skipping is unnecessary if eliminate_value checks
		// for non-assignment before pushing state.  
		if (c != skip && !c->eliminate_value(val, ul))
			return false;
	}
	return true;
}

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_CONSTRAINT_TCC__

