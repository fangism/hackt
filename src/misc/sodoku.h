/**
	\file "sodoku.h"
	$Id: sodoku.h,v 1.1.2.1 2006/01/05 01:15:27 fang Exp $
 */

#ifndef	__SODUKU_H__
#define	__SODUKU_H__

#include <iosfwd>
#include <cassert>
#include <list>
#include <vector>
#include <stack>
// #include <bitset>
#include <utility>

namespace sodoku {
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
	// static const ushort		ones = 0xFFFF;

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
	avail_set() : ints(0x1FF), avail(9), val(0) { }

	// default POD copy-constructor and assignment

	~avail_set() { }

	bool
	probe(const uchar pos) const {
		assert(pos -1 < 9);
		return ints & masks[pos-1];
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
		// assert(!val);
		assert(pos -1 < 9);
		// assert(ints & masks[pos-1]);		// not true
		assert(avail);
		ushort n = ints & ~masks[pos-1];
		if (ints != n)
			--avail;
		ints = n;
		return avail;
	}

#if 0
	/**
		Unclears a bit.  
		\param pos must be in [1,9]
		We automatically subtract 1.  
		\pre bit at the position must be set!
	 */
	void
	unset(const uchar pos) {
		assert(pos -1 < 9);
		// assert(val);
		assert(!(ints & masks[pos-1]));
		ints |= masks[pos-1];
		++avail;
		assert(avail <= 9);
	}
#endif

	void
	assign(const uchar v) {
		assert(!val);
		val = v;
	}

#if 0
	void
	unassign(void) {
		assert(val);
		val = 0;
	}
#endif

	ushort
	get_mask(void) const { return ints; }

	uchar
	num_avail(void) const { return avail; }

	uchar
	value(void) const { return val; }

};	// end class avail_set

//=============================================================================
/**
	Use a 10x10 board, 1-indexed, with 0 as a NULL value.  
	Representation using characters.  
	Each cell needs a back-link to constraints.  
	Each cell is a member of 3 constraints: row, col, block.
 */
class board {
public:
	class move;
	class undo_list;
private:
	typedef	stack<avail_set, vector<avail_set> >	cell_type;
	cell_type			cell[10][10];
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
	istream&
	load(istream&);

private:
	void
	__solve(list<solution>&);

};	// end class board

//=============================================================================
class solution {
	uchar				cell[10][10];
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
class board::undo_list {
	board&				bd;
	// maximum number of cells to undo
	pair<uchar,uchar>		cell[20];
	//
	size_t				count;

public:
	explicit
	undo_list(board& b) : bd(b), count(0) { }

	~undo_list();

	void
	push(const uchar x, const uchar y) {
		assert(count < 20);
		cell[count].first = x;
		cell[count].second = y;
		++count;
	}

};	// end class board::undo_list

//=============================================================================
/**
	Representation of an attempted placement of a number
	and position.  
 */
class board::move {
	board::undo_list		ul;
	/// true if move was accepted
	uchar				x, y;
	uchar				val;
	bool				accept;

public:
	/**
		Don't really need to remember val.  
	 */
	move(board&, const uchar, const uchar, const uchar);

	~move();

	bool
	accepted(void) const { return accept; }

};	// end class board::move

//=============================================================================
}	// end namespace sodoku

#endif	// __SODUKU_H__

