/**
	\file "util/numeric/nibble_tables.cc"
	Lookup tables for bit manipulation functions.  
	$Id: nibble_tables.cc,v 1.1 2007/08/29 04:45:54 fang Exp $
 */

#include "util/numeric/nibble_tables.h"

namespace util {
namespace numeric {
//=============================================================================

const char
nibble_LSB_position[16] = {
	4, 0, 1, 0,
	2, 0, 1, 0,
	3, 0, 1, 0,
	2, 0, 1, 0
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	First set bit position. 
	If index/argument is 0, return 0 (defined).  
 */
const char
nibble_FS_position[16] = {
	0, 1, 2, 1,
	3, 1, 2, 1,
	4, 1, 2, 1,
	3, 1, 2, 1
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char
nibble_popcount[16] = {
	0, 1, 1, 2,
	1, 2, 2, 3,
	1, 2, 2, 3,
	2, 3, 3, 4
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char
nibble_parity[16] = {
	0, 1, 1, 0,
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 1, 0
};

//=============================================================================
}	// end namespace numeric
}	// end namespace util

