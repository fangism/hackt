/**
	\file "sodoku.cc"
	$Id: sodoku.cc,v 1.1.2.1 2006/01/05 01:15:27 fang Exp $
 */

#include "misc/sodoku.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

#define	DEBUG_LOAD		0
#define	STEP_SOLVE		0
#define	DEBUG_SOLVE		0
#define	REVEAL_SOLUTION		1	// print as they are found

namespace sodoku {
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

//=============================================================================
// class solution method definitions

solution::solution(const board& b) {
	size_t i = 1;
	for ( ; i<=9; i++) {
		size_t j = 1;
		for ( ; j<=9; j++)
			cell[i][j] = b.probe(i,j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
solution::dump(ostream& o) const {
	size_t i = 1;
	for ( ; i<=9; i++) {
		o << '\t';
		size_t j = 1;
		for ( ; j<=9; j++) {
			const size_t val = cell[i][j];
			if (val) o << val;
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
 */
board::board() {
	// the cell array is already initialized, by default construction
	// so are the row, col, and block constraint arrays!
	size_t i = 1;
	for ( ; i<=9; i++) {
		size_t j = 1;
		for ( ; j<=9; j++)
			cell[i][j].push(avail_set());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
board::~board() {
	// just default clear everything
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
bool
board::conditional_assign_cell(const uchar x, const uchar y, const uchar v, 
		undo_list& ul) {
	// if unassigned, then modify its state, else skip
	cell_type& c(cell[x][y]);
	if (!c.top().value()) {
		ul.push(x, y);
		c.push(c.top());
		return c.top().set(v);
	} else	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if assignment was accepted.  
 */
bool
board::assign(const uchar x, const uchar y, const uchar v, undo_list& ul) {
	assert(x-1 < 9);
	assert(y-1 < 9);
	assert(v-1 < 9);
	cell_type& c(cell[x][y]);
	c.push(c.top());
	c.top().assign(v);
	ul.push(x, y);
	// tighten constraints on row, col, blocks
	bool avail = true;
	// cerr << "updating rows/cols..." << endl;
{
	uchar i = 1;
	for ( ; i<=9; i++) {
		if (i != y)	avail &= conditional_assign_cell(x, i, v, ul);
		if (i != x)	avail &= conditional_assign_cell(i, y, v, ul);
	}
}
	// cerr << "updating blocks..." << endl;
{
	const uchar bx = ((x-1)/3)*3;
	const uchar by = ((y-1)/3)*3;
	uchar j = 1;
	for ( ; j<=3; j++) {
		uchar k = 1;
		for ( ; k<=3; k++) {
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
	assert(x-1 < 9);
	assert(y-1 < 9);
	assert(v-1 < 9);
	cell[x][y].top().assign(v);
	// tighten constraints on row, col, blocks
	bool avail = true;
//	cerr << "updating rows/cols..." << endl;
{
	size_t i = 1;
	for ( ; i<=9; i++) {
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
	const uchar bx = ((x-1)/3)*3;
	const uchar by = ((y-1)/3)*3;
	uchar j = 1;
	for ( ; j<=3; j++) {
		uchar k = 1;
		for ( ; k<=3; k++) {
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
	assert(x-1 < 9);
	assert(y-1 < 9);
	cell_type& c(cell[x][y]);
	assert(!c.empty());
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
	size_t best_x = 0, best_y = 0;
	uchar min_avail = 9;
{
	uchar i = 1;
	for ( ; i<=9; i++) {
		uchar j = 1;
		for ( ; j<=9; j++) {
			const avail_set& scan(cell[i][j].top());
			const uchar avail = scan.num_avail();
			const uchar v = scan.value();
			if (!v) {
				assert(avail);
				if (avail < min_avail) {
					best_x = i;
					best_y = j;
					min_avail = avail;
				}
			}
		}
	}
}
#if STEP_SOLVE
{
	char c;
	dump_state(cerr << "Examining: " << endl);
	std::cin >> c;
}
#endif
if (best_x && best_y) {
#if DEBUG_SOLVE
	cerr << "Cell [" << best_x << "][" << best_y << "] has " << 
		size_t(min_avail) << " choices." << endl;
#endif
	// cell[best_x][best_y] has the fewest number of available values
	const avail_set& pivot(cell[best_x][best_y].top());
	// extract list of possible values and recursively try one-by-one
	ushort mask = pivot.get_mask();
	assert(mask);
	uchar val = 1;
	for ( ; mask; mask >>= 1, ++val) {
		if (mask & 0x1) {
			const move m(*this, best_x, best_y, val);
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
istream&
board::load(istream& f) {
	do {
		ushort x, y, z;
		f >> x >> y >> z;
		if (f.good()) {
#if DEBUG_LOAD
			cerr << "placing: [" << x << ',' << y << "]="
				<< z << endl;
			
#endif
			if (!commit(x,y,z)) {
				cerr << "ERROR: [" << x << ',' << y << "]="
					<< z << " was rejected!" << endl;
				throw std::exception();
			}
#if DEBUG_LOAD
			dump_state(cerr) << endl;
#endif
		}
	} while (f.good());
	return f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
board::dump(ostream& o) const {
	ushort i = 1;
	for ( ; i<=9; i++) {
		ushort j = 1;
		o << '\t';
		for ( ; j<=9; j++) {
			const size_t val = cell[i][j].top().value();
			if (val) o << val;
			else o << '-';
			o << ' ';
		}
		o << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
board::dump_state(ostream& o) const {
	ushort i = 1;
	for ( ; i<=9; i++) {
		ushort j = 1;
		o << '\t';
		for ( ; j<=9; j++) {
			const size_t val = cell[i][j].top().value();
			if (val) o << val;
			else {
				o << std::setbase(16);
				o << '(' << cell[i][j].top().get_mask() << ')';
				o << std::setbase(10);
			}
			o << '\t';
		}
		o << endl;
	}
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
}	// end namespace sodoku

