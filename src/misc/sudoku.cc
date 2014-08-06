/**
	\file "sudoku.cc"
	$Id: sudoku.cc,v 1.3 2006/04/03 19:36:24 fang Exp $
 */

#include <iostream>
#include <iomanip>
#include <algorithm>
#include "misc/sudoku.hh"

// some handy compile-switches
#define	DEBUG_LOAD		0
#define	STEP_SOLVE		0
#define	DEBUG_SOLVE		0
#define	REVEAL_SOLUTION		1	// print as they are found

namespace sudoku {
using std::ios_base;
using std::cerr;
using std::endl;
//=============================================================================
// class avail_set member definitions

const ushort
avail_set::masks[9] = {
	0x001, 0x002, 0x004, 0x008,
	0x010, 0x020, 0x040, 0x080,
	0x100
};

#define	FIRST		0
#define	THIRD		2
#define	LAST		8

//=============================================================================
// internal classes

/**
	Initialize with one element on stack.  
 */
board::cell_type::cell_type() : cell_type_base() {
	push(avail_set());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Undo moves in a stack-managed tracker.  
	TODO: give this class hidden visibility.  
 */
class board::undo_list {
	board&                          bd;
	// maximum number of cells to undo
	pair<uchar,uchar>               cell[20];
	//
	size_t                          count;

public:
	explicit
	undo_list(board& b) : bd(b), count(0) { }

	~undo_list();

	void
	push(const uchar x, const uchar y) {
		SUDOKU_ASSERT(count < 20);
		cell[count].first = x;
		cell[count].second = y;
		++count;
	}

};      // end class board::undo_list

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Representation of an attempted placement of a number
	and position.
	TODO: give this class hidden visibility.  
 */
class board::move {
	board::undo_list                ul;
	/// true if move was accepted
	uchar                           x, y;
	uchar                           val;
	bool                            accept;

public:
	/**
		Don't really need to remember val.
	 */
	move(board&, const uchar, const uchar, const uchar);

	~move();

	bool
	accepted(void) const { return accept; }

};      // end class board::move

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct board::pivot_finder {
	size_t			best_x;
	size_t			best_y;
	uchar			min_avail;

	pivot_finder() : best_x(9), best_y(9), min_avail(9) { }

	/**
		\param i, j the coordinates of the cell.  
	 */
	void
	operator () (cell_type& c, const size_t i, const size_t j) {
		const avail_set& scan(c.top());
		if (!scan.is_assigned()) {
			const uchar avail = scan.num_avail();
			SUDOKU_ASSERT(avail);
			if (avail < min_avail) {
				best_x = i;
				best_y = j;
				min_avail = avail;
			}
		}
	}

	bool
	valid(void) const {
		return best_x < 9 && best_y < 9;
	}

};	// end struct board::pivot_finder

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct board::value_dumper {
	ostream&	o;
	size_t		count;

	value_dumper(ostream& _o) : o(_o), count(0) { }

	void
	operator () (const cell_type& cell, const size_t, const size_t) {
		if (!(count % 9)) o << '\t';
		++count;
		const avail_set& c(cell.top());
		if (c.is_assigned())
			o << c.printed_value();
		else o << '-';
		if (!(count % 9)) o << endl;
		else o << ' ';
	}
};	// end struct value_dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct board::state_dumper {
	ostream&	o;
	size_t		count;

	state_dumper(ostream& _o) : o(_o), count(0) { }

	void
	operator () (const cell_type& cell, const size_t, const size_t) {
		o << '\t';
		const avail_set& c(cell.top());
		const size_t val = c.printed_value();
		if (c.is_assigned())
			o << val;
		else {
			o << std::setbase(16);
			o << '(' << c.get_mask() << ')';
			o << std::setbase(10);
		}
		++count;
		if (!(count % 9)) o << endl;
	}
};	// end struct state_dumper

//=============================================================================
// class solution method definitions

solution::solution(const board& b) {
	size_t i = FIRST;
	for ( ; i<=LAST; i++) {
		size_t j = FIRST;
		for ( ; j<=LAST; j++)
			cell[i][j] = b.probe(i,j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
solution::dump(ostream& o) const {
	size_t i = FIRST;
	for ( ; i<=LAST; i++) {
		o << '\t';
		size_t j = FIRST;
		for ( ; j<=LAST; j++) {
			const size_t val = cell[i][j];
			if (val < 9) o << val+1;
			else o << '-';
			o << ' ';
		}
		o << endl;
	}
	return o;
}

//=============================================================================
// class board method definitions

/**
	Initially, stack matrix is empty, so we need to 
	initialize each cell with at least a top element.  
	Done in each cell's default constructor already.  
 */
board::board() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
board::~board() {
	// just default clear everything
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-modifying visitor acceptor.
 */
template <class F>
void
board::accept(F& f) const {
	size_t i = FIRST;
	for ( ; i<=LAST; i++) {
		size_t j = FIRST;
		for ( ; j<=LAST; j++)
			f(cell[i][j], i, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifying visitor acceptor.
 */
template <class F>
void
board::accept(F& f) {
	size_t i = FIRST;
	for ( ; i<=LAST; i++) {
		size_t j = FIRST;
		for ( ; j<=LAST; j++)
			f(cell[i][j], i, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
bool
board::conditional_assign_cell(const uchar x, const uchar y, const uchar v, 
		undo_list& ul) {
	// if unassigned, then modify its state, else skip
	cell_type& c(cell[x][y]);
	if (!c.top().is_assigned()) {
		ul.push(x, y);
		c.push(c.top());
		return c.top().set(v);
	} else	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: return early when avail goes false.
	\return true if assignment was accepted.  
 */
bool
board::assign(const uchar x, const uchar y, const uchar v, undo_list& ul) {
	SUDOKU_ASSERT(x < 9);
	SUDOKU_ASSERT(y < 9);
	SUDOKU_ASSERT(v < 9);
	cell_type& c(cell[x][y]);
	c.push(c.top());
	c.top().assign(v);
	ul.push(x, y);
	// tighten constraints on row, col, blocks
	bool avail = true;
	// cerr << "updating rows/cols..." << endl;
{
	uchar i = FIRST;
	for ( ; i<=LAST; i++) {
		if (i != y)	avail &= conditional_assign_cell(x, i, v, ul);
		if (i != x)	avail &= conditional_assign_cell(i, y, v, ul);
	}
}
	// cerr << "updating blocks..." << endl;
{
	const uchar bx = (x/3)*3;
	const uchar by = (y/3)*3;
	uchar j = FIRST;
	for ( ; j<=THIRD; j++) {
		uchar k = FIRST;
		for ( ; k<=THIRD; k++) {
			const uchar cx = bx+j;
			const uchar cy = by+k;
			if (cx != x && cy != y)
				avail &= conditional_assign_cell(cx, cy, v, ul);
		}
	}
}
	return avail;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Irreversible modification of the board.  
	Used for loading the initial board.  
	\return true if this cell assignment was accepted.  
 */
bool
board::commit(const uchar x, const uchar y, const uchar v) {
	SUDOKU_ASSERT(x < 9);
	SUDOKU_ASSERT(y < 9);
	SUDOKU_ASSERT(v < 9);
	cell[x][y].top().assign(v);
	// tighten constraints on row, col, blocks
	bool avail = true;
//	cerr << "updating rows/cols..." << endl;
{
	size_t i = FIRST;
	for ( ; i<=LAST; i++) {
		if (i != y) {
			avail &= cell[x][i].top().set(v);
			if (!avail)
				cerr << "reject by " << size_t(x)
					<< ',' << i << endl;
		}
		if (i != x) {
			avail &= cell[i][y].top().set(v);
			if (!avail)
				cerr << "reject by " << i << ','
					<< size_t(y) << endl;
		}
	}
}
//	cerr << "updating blocks..." << endl;
{
	const uchar bx = (x/3)*3;
	const uchar by = (y/3)*3;
	uchar j = FIRST;
	for ( ; j<=THIRD; j++) {
		uchar k = FIRST;
		for ( ; k<=THIRD; k++) {
			const size_t cx = bx+j;
			const size_t cy = by+k;
			if (cx != x && cy != y) {
				avail &= cell[cx][cy].top().set(v);
				if (!avail)
					cerr << "reject by " <<
						cx << ',' << cy << endl;
			}
		}
	}
}
	return avail;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Undo of assign.  
 */
void
board::unassign(const uchar x, const uchar y) {
	SUDOKU_ASSERT(x < 9);
	SUDOKU_ASSERT(y < 9);
	cell_type& c(cell[x][y]);
	SUDOKU_ASSERT(!c.empty());
	c.pop();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
board::solve(list<solution>& sols) const {
	board copy(*this);
	copy.__solve(sols);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive brute-force solver. 
	\pre some positions have already been assigned legally.  
		(Previous moves have all been accepted.)
 */
void
board::__solve(list<solution>& sols) {
	// to minimize recursion breadth, we find some pivot cell
	// with the tightest constraints (fewest possible values).  
	// later optimization: memoize priority list to minimize re-evaluation
	pivot_finder pf;	// visitor functor
	accept(pf);
#if STEP_SOLVE
{
	char c;
	dump_state(cerr << "Examining: " << endl);
	std::cin >> c;
}
#endif
if (pf.valid()) {
#if DEBUG_SOLVE
	cerr << "Cell [" << pf.best_x << "][" << pf.best_y << "] has " << 
		size_t(min_avail) << " choices." << endl;
#endif
	// cell[best_x][best_y] has the fewest number of available values
	const avail_set& pivot(cell[pf.best_x][pf.best_y].top());
	// extract list of possible values and recursively try one-by-one
	ushort mask = pivot.get_mask();
	SUDOKU_ASSERT(mask);
	uchar val = FIRST;
	for ( ; mask; mask >>= 1, ++val) {
		if (mask & 0x1) {
			const move m(*this, pf.best_x, pf.best_y, val);
			if (m.accepted())
				__solve(sols);	// recursion here
			// else just undo it upon move destruction
		}
	}
} else {
	// this board position is accepted!
	sols.push_back(solution());
	new (&sols.back()) solution(*this);	// in-place construction!
	// we save a copy away in the solution set
#if REVEAL_SOLUTION
	sols.back().dump(cerr << "SOLUTION " << sols.size() << ":" << endl);
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is an error.
 */
bool
board::load(istream& f) {
	while (f.good()) {
		ushort x, y, z;
		f >> x >> y >> z;
		if (!f.good())
			break;
		--x, --y, --z;		// normalize to 0-8
		if (x >=9 || y >= 9 || z >= 9) {
			cerr << "ERROR: all input numbers must be 1-9." << endl;
			return true;
		}
		if (f.good()) {
#if DEBUG_LOAD
			cerr << "placing: [" << x << ',' << y << "]="
				<< z << endl;
			
#endif
			if (!commit(x,y,z)) {
				cerr << "ERROR: [" << x << ',' << y << "]="
					<< z << " was rejected!" << endl;
				return true;
			}
#if DEBUG_LOAD
			dump_state(cerr) << endl;
#endif
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
board::dump(ostream& o) const {
	value_dumper d(o);
	accept(d);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
board::dump_state(ostream& o) const {
	state_dumper d(o);
	accept(d);
	return o;
}

//=============================================================================
// class board::undo_list method definitions

board::undo_list::~undo_list() {
	size_t i = 0;
	for ( ; i < count; i++) {
		bd.unassign(cell[i].first, cell[i].second);
	}
}

//=============================================================================
// class board::move method definitions

/**
	Non-trivial constructor. :)
 */
board::move::move(board& b, const uchar _x, const uchar _y, const uchar _v) :
		ul(b), x(_x), y(_y), val(_v), accept(b.assign(x, y, val, ul)) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The undo work is done by ~undo_list().
 */
board::move::~move() {
}

//=============================================================================
}	// end namespace sudoku

