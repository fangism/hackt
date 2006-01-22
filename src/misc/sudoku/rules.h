/**
	\file "misc/sudoku/rules.h"
	Rules are a system of constraints.  
	Each rule set adds constraints to board cells.  
	$Id: rules.h,v 1.2 2006/01/22 06:53:22 fang Exp $
 */

#ifndef	__SUDOKU_RULES_H__
#define	__SUDOKU_RULES_H__

#include "misc/sudoku/constraint.h"
#include "util/memory/pointer_traits.h"

namespace sudoku {
using util::memory::pointee;

//=============================================================================
/**
	Interface requirements for a sudoku variation rule-set.
 */
class rules_interface {
#if 0
public:
	static
	void
	setup(...);
#endif
};	//

//=============================================================================
#define	CLASSIC_RULES_TEMPLATE_SIGNATURE	template <class ConstraintSetType>
#define	CLASSIC_RULES_CLASS			classic_rules<ConstraintSetType>
/**
	Classic sudoko rules.  
 */
CLASSIC_RULES_TEMPLATE_SIGNATURE
class classic_rules {
public:
	typedef	ConstraintSetType			constraint_set_type;
	typedef	typename constraint_set_type::value_type
							constraint_ptr_type;
	typedef	typename pointee<constraint_ptr_type>::type	constraint_type;
	typedef	typename constraint_type::cell_type	cell_type;
	enum {
		/// the maximum number of moves to undo per search
		MAX_UNDO_SIZE = 20
	};
private:
	// describe the constraints here:
	constraint_type				row_set[9];
	constraint_type				col_set[9];
	constraint_type				block_set[3][3];
public:
	classic_rules();
	~classic_rules();

	template <class BoardType>
	void
	setup_board(BoardType&);
};	// end class classic_rules

//=============================================================================
#define	DIAGONAL_RULES_TEMPLATE_SIGNATURE	CLASSIC_RULES_TEMPLATE_SIGNATURE
#define	DIAGONAL_RULES_CLASS			diagonal_rules<ConstraintSetType>

/**
	In additional to the classic constraints, the diagonals
	must also contain each digits exactly once.  
 */
DIAGONAL_RULES_TEMPLATE_SIGNATURE
class diagonal_rules : protected CLASSIC_RULES_CLASS {
	typedef	CLASSIC_RULES_CLASS		parent_type;
	typedef	ConstraintSetType		constraint_set_type;
	typedef	typename constraint_set_type::value_type
							constraint_ptr_type;
	typedef	typename pointee<constraint_ptr_type>::type	constraint_type;
	typedef	typename constraint_type::cell_type	cell_type;
private:
	constraint_type				fwd_diagonal_set;
	constraint_type				back_diagonal_set;
public:
	diagonal_rules();
	~diagonal_rules();

	template <class BoardType>
	void
	setup_board(BoardType&);
};	// end class diagonal_rules

//=============================================================================
// four-dimensional variant!

//=============================================================================
/**
	Programmable rule set.
 */
class programmable_rules {
private:
public:
#if 0
	void
	setup_board(board_type&) const;
#endif
};	// end class programmable rules

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_RULES_H__

