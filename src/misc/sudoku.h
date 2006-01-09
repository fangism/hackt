/**
	\file "sudoku.h"
	The name of the game is sudoku.
	Place numbers 1-9 in a 9x9 grid such that:
	1) each row contains each digit exactly once
	2) each column contains each digit exactly once
	3) each 3x3 sub-block contains each digit exactly once
	$Id: sudoku.h,v 1.1.2.2 2006/01/09 21:37:12 fang Exp $
	TODO: generalize for variations!
	http://www.maa.org/editorial/mathgames/mathgames_09_05_05.html
	Implement general constraints!
 */

#ifndef	__SUDOKU_H__
#define	__SUDOKU_H__

#include <iosfwd>
#include <cassert>
#include <list>
#include <vector>
#include <stack>
#include <utility>

#ifndef	DEBUG_SUDOKU
#define	DEBUG_SUDOKU		1
#endif

#if	DEBUG_SUDOKU
#define	SUDOKU_ASSERT(x)	assert(x)
#else
#define	SUDOKU_ASSERT(x)	assert(x)
#endif

/**
	Namespace for the sudoku solver data structures. 
 */
namespace sudoku {
using std::pair;
using std::list;
using std::vector;
using std::stack;
using std::istream;
using std::ostream;
typedef	unsigned char			uchar;
typedef	unsigned short			ushort;
typedef	unsigned int			uint;
typedef	unsigned long			ulong;
class board;
class solution;

//=============================================================================
/**
	Representation of a pigeon-hole set of numbers 1-9.  
	Either use a bitset or some integer and mask operations.  
	The set can be used for both board-cells and set-constraints.
	This captures the constraint rules of the game.  
 */
class avail_set {
private:
	static const ushort		masks[];

	/// 9-bit integer mask representing the state of the set
	ushort			ints;
	/**
		population count cache.
		Can either represent pigeons or holes.  
		Convention: the smaller this value, the more constrained
		the remaining set.  
		Initially the mask should be full of set bits.  
		This is kept consistent with ints.  
	 */
	uchar			avail;
	/**
		This field is only used for cells of the board.  
		Value ranges from 0-9, 0 meaning unassigned.  
		If this is set, ints and avail are meaningless, 
		and immutable.  
	 */
	uchar			val;
public:
	/**
		\param n the initial mask.  
	 */
	avail_set() : ints(0x1FF), avail(9), val(0xFF) { }

	// default POD copy-constructor and assignment

	~avail_set() { }

	bool
	probe(const uchar pos) const {
		SUDOKU_ASSERT(pos < 9);
		return ints & masks[pos];
	}

	/**
		Clears a bit, 1-indexed from LSB.
		\param pos must be in [1,9]
		We automatically subtract 1.  
		\pre bit at the position must not already be set!
		\return true if the assignment is accepted.
	 */
	bool
	set(const uchar pos) {
		SUDOKU_ASSERT(avail);
		SUDOKU_ASSERT(pos < 9);
		ushort n = ints & ~masks[pos];
		if (ints != n)
			--avail;
		ints = n;
		return avail;
	}

	bool
	is_assigned(void) const {
		return val < 9;
	}

	void
	assign(const uchar v) {
		SUDOKU_ASSERT(!is_assigned());
		val = v;
	}

	ushort
	get_mask(void) const { return ints; }

	uchar
	num_avail(void) const { return avail; }

	uchar
	value(void) const { return val; }

	size_t
	printed_value(void) const {
		return val+1;
	}

};	// end class avail_set

//=============================================================================
/**
	Use a 10x10 board, 1-indexed, with 0 as a NULL value.  
	Representation using characters.  
	Each cell needs a back-link to constraints.  
	Each cell is a member of 3 constraints: row, col, block.
 */
class board {
private:
	class move;
	class undo_list;
	struct pivot_finder;
	struct value_dumper;
	struct state_dumper;
private:
	typedef	stack<avail_set, vector<avail_set> >	cell_type_base;
	class cell_type : public cell_type_base {
	public:
		cell_type();
	};	// end struct cell_type
	cell_type			cell[9][9];
public:
	board();
	~board();
private:
	bool
	conditional_assign_cell(const uchar, const uchar, const uchar, 
		undo_list&);

	// push
	bool
	assign(const uchar, const uchar, const uchar, undo_list&);

	bool
	commit(const uchar, const uchar, const uchar);

	// pop
	void
	unassign(const uchar, const uchar);

	template <class F>
	void
	accept(F&) const;

	template <class F>
	void
	accept(F&);
public:
	uchar
	probe(const uchar x, const uchar y) const {
		return cell[x][y].top().value();
	}

	void
	solve(list<solution>&) const;

	ostream&
	dump(ostream&) const;

	ostream&
	dump_state(ostream&) const;

	// optional: undo_list
	bool
	load(istream&);

private:
	void
	__solve(list<solution>&);

};	// end class board

//=============================================================================
class solution {
	uchar				cell[9][9];
public:
	solution() { }

	// hint: manually elide unnecessary copy-construction on push
	// copy
	explicit
	solution(const board&);

	~solution() { }

	ostream&
	dump(ostream&) const;
};	// end class solution

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_H__

