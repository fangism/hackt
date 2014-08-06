/**
	\file "sudoku/cell.hh"
	$Id: cell.h,v 1.2 2006/01/22 06:53:18 fang Exp $
 */

#ifndef	__SUDOKU_CELL_H__
#define	__SUDOKU_CELL_H__

#include <stack>
#include "misc/sudoku/inttypes.hh"
#include "util/memory/pointer_traits.hh"

namespace sudoku {
using util::memory::pointee;
using util::memory::pointer_traits;

//=============================================================================
/**
	Generic interface requirements:
	sequential interface to constraints of which the cell is a member.  
 */
class cell_base {
};	// end class cell_base

//=============================================================================
#define	CELL_TEMPLATE_SIGNATURE						\
			template <class ConstraintSetType, class StateType>
#define	CELL_CLASS			cell<ConstraintSetType, StateType>
/**
	Generic cell for the sudoku board.  
	\param ConstraintSetType should be some container of 
		*modifiable pointers* to constraints.  
		e.g. vector<constraint*>.
 */
CELL_TEMPLATE_SIGNATURE
class cell {
	// type used to identify which member of a cell_set constraint this is
	typedef	cell*			identifier_type;
public:
	typedef	ConstraintSetType	constraint_set_type;
	typedef	typename constraint_set_type::value_type
					constraint_ptr_type;
	typedef	typename pointee<constraint_ptr_type>::type
					constraint_type;
	typedef	StateType		state_type;
	typedef	std::stack<StateType>	state_stack_type;
private:
	ConstraintSetType		constraint_set;
	state_stack_type		state_stack;
public:
	cell();
	~cell();

	void
	add_constraint(const constraint_ptr_type);

	template <class UndoListType>
	bool
	push(const uchar, UndoListType&);

	void
	pop(void);

	state_type&
	top(void) { return state_stack.top(); }

	const state_type&
	top(void) const { return state_stack.top(); }

	template <class UndoListType>
	bool
	eliminate_value(const uchar, UndoListType&);

	static
	uchar
	invalid_value(void) { return constraint_type::invalid_value() +1; }

};	// end class cell

//=============================================================================
#if 0
/**
 */
class classic_cell {
	
};	// end class
#endif

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_CELL_H__

