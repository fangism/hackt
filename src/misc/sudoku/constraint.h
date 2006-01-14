/**
	\file "sudoku/constraint.h"
	Each constraint defines a rule of the game.  
	TODO: another idea use constraints to narrow down
		the pivot point, with extra bookkeeping.  
		E.g. a digit K remains to be allocated in a particular row
			and it must go in one of the remaining spots.
			Compound this information with the cell constraints
			remaining on the board.  Should reduce searching!
		In other words, choose the pivot point, also accounting
			for the constraints' pigeonholes, in addition to
			the cell's pigeonholes.  
		Can this result in tighter searching?  I'm not sure...
	$Id: constraint.h,v 1.1.2.1 2006/01/14 06:52:05 fang Exp $
 */

#ifndef	__SUDOKU_CONSTRAINT_H__
#define	__SUDOKU_CONSTRAINT_H__

#include "misc/sudoku/debug.h"
#include "misc/sudoku/inttypes.h"

namespace sudoku {
//=============================================================================
/**
	Abstract base class for puzzle constraints.  
 */
class constraint {
protected:
	/// protected destructor: never delete as base
virtual ~constraint() { }
public:
	/// \return the number of choices available
virtual	uchar
	num_avail(void) const = 0;

	/// \return true if position queried is available
virtual bool
	probe(const size_t) const = 0;

	/// \return whether or not the internal state changed
virtual bool
	set_position(const size_t) = 0;

};	// end class constraint

//-----------------------------------------------------------------------------
/**
	For efficiency, this should be a POD-type.  
	\param N is the number of pigeonholes, which must be [1,16];
 */
template <size_t N>
class pigeonholes {
	typedef	ushort			state_type;
private:
	/// mask representing the state of the constraint
	state_type			state;
	/// availability count -- the smaller, the more constrained
	uchar				avail;
public:
	static const state_type		full_mask = (1 << N) -1;

	static
	state_type
	position_mask(const size_t i) { return 1 << i; }
public:
	pigeonholes() : state(full_mask), avail(N) { }
	// default copy-ctor
	// trivial default dtor

	bool
	probe(const size_t pos) const {
		assert(pos < N);
		return state & position_mask(pos);
	}

	/// \return true if pos is in valid range
	static
	bool
	is_valid_position(const size_t pos) {
		return pos < N;
	}

	/// \return an invalid positional value
	static
	uchar
	invalid_position(void) { return N; }

	state_type
	get_state(void) const { return state; }

	uchar
	num_avail(void) const { return avail; }

	/**
		Attempts to reserve a pigeon hole.
		If this is already assigned, this will be skipped.  
		It is the caller's job to check the availability after each 
		set_position to determine the legality of the move.  
		\return true if state was changed.  
	 */
	bool
	set_position(const size_t pos) {
		assert(avail);
		const state_type next = state & ~position_mask(pos);
		if (next != state) {
			--avail;
			state = next;
			return true;
		}
		return false;
	}

	template <class ValueContainer>
	void
	collect_possible_values(ValueContainer&) const;

};	// end class pigeonholes

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	CELL_SET_TEMPLATE_SIGNATURE					\
	template <size_t N, template <class,class> class CellType,	\
		class StateType, template <class> class ContainerType>
#define	CELL_SET_CLASS		cell_set<N,CellType,StateType,ContainerType>

/**
	This represents a group of cells belonging to the same 
	pigeonhole constraint.  
	\param N is the fixed exact membership size.  
 */
CELL_SET_TEMPLATE_SIGNATURE
class cell_set {
public:
	typedef	cell_set			this_type;
	// WRONG, need container of this_type for cell_type!
	typedef	CellType<ContainerType<this_type*>,StateType>
						cell_type;
	// TODO: use never_ptr
	typedef	cell_type*			member_type;
	typedef	member_type			value_type;
private:
	// cell members
	member_type				members[N];
public:
	// ctor should initialize pointers with NULL
	cell_set();
	~cell_set() { }

	member_type&
	operator [] (const size_t i) {
		SUDOKU_ASSERT(i < N);
		return members[i];
	}

	const member_type&
	operator [] (const size_t i) const {
		SUDOKU_ASSERT(i < N);
		return members[i];
	}

	template <class UndoListType>
	bool
	try_value(const member_type, const uchar, UndoListType&) const;

	static
	uchar
	invalid_value(void) { return N; }

};	// end class cell_set

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_CONSTRAINT_H__

