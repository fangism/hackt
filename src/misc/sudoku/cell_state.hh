/**
	\file "misc/sudoko/cell_state.hh"
	The stateful data retained by a cell.  
	$Id: cell_state.h,v 1.2 2006/01/22 06:53:20 fang Exp $
 */

#ifndef	__SUDOKU_CELL_STATE_H__
#define	__SUDOKU_CELL_STATE_H__

#include "misc/sudoku/inttypes.hh"
// #include "misc/sudoku/constraint.hh"

namespace sudoku {
//=============================================================================
/**
	\param ValueSetType some sort of set of values, e.g. pigeonholes<9>.
		The ValueSetType must have member functions:
			static bool is_valid_position(const size_t)
			static uchar invalid_position(void)
	Hope that the size/layout of this struct is compact.  
 */
template <class ValueSetType>
class cell_state : public ValueSetType {
public:
	typedef	ValueSetType			value_set_type;
private:
	/// the assigned value
	uchar					val;
public:
	cell_state() : value_set_type(),
		val(value_set_type::invalid_position()) { }
	// default copy-constructor
	~cell_state() { }

	uchar
	value(void) const { return val; }

	bool
	is_assigned(void) const {
		return value_set_type::is_valid_position(val);
	}

};	// end class cell_state

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_CELL_STATE_H__
