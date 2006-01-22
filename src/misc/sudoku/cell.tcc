/**
	\file "misc/sudoku/cell.tcc"
	$Id: cell.tcc,v 1.2 2006/01/22 06:53:19 fang Exp $
 */

#ifndef	__SUDOKU_CELL_TCC__
#define	__SUDOKU_CELL_TCC__

#include "misc/sudoku/cell.h"
// #include "misc/sudoku/constraint.h"

namespace sudoku {
//=============================================================================
// class cell method definitions

CELL_TEMPLATE_SIGNATURE
CELL_CLASS::cell() : constraint_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CELL_TEMPLATE_SIGNATURE
CELL_CLASS::~cell() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CELL_TEMPLATE_SIGNATURE
void
CELL_CLASS::add_constraint(const constraint_ptr_type c) {
	constraint_set.push_back(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Conditional push: try-value.
	If rejected, caller's destruction of the undo_list will
	automatically restore all affected cell_states' former values!
	\return true if value is accepted, false if rejected.  
 */
CELL_TEMPLATE_SIGNATURE
template <class UndoListType>
bool
CELL_CLASS::push(const uchar val, UndoListType& ul) {
	// push new value onto this cell's stack unconditionally
	state_stack.push(this->top());
	this->top().assign(val);
	ul.push_back(this);
	// check all constraints
	typedef	typename constraint_set_type::const_iterator
			constraint_iterator;
	constraint_iterator i(constraint_set.begin());
	const constraint_iterator e(constraint_set.end());
	for ( ; i!=e; ++i) {
		if (!i->try_value(this, val, ul))
			return false;
		// at least one constraint failed
	}
	// else all constraints were met
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CELL_TEMPLATE_SIGNATURE
void
CELL_CLASS::pop(void) {
	
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called as a result of constraint checking when a value
	is assigned (attempted) for a cell that shares the same constraint.  
	\pre value must not already be assigned for this cell.  
	\return true if there are still choices available in the pigeonholes,
		i.e. this cell accepts the assignment.  
 */
CELL_TEMPLATE_SIGNATURE
template <class UndoListType>
bool
CELL_CLASS::eliminate_value(const uchar val, UndoListType& ul) {
	if (this->top().is_assigned()) {
		// then we just accept
		// ASSERT(val is free);
		return true;
	} else {
		// must not already be assigned!
		const state_type temp(this->top());
		if (temp.set_position(val)) {
			// conditional modification if state actually changed
			state_stack.push(temp);
			ul.push_back(this);
			// else no change, skip
		}
		// check number of available choices remaining
		return this->top().num_avail();
	}
}

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_CELL_TCC__

