/**
	\file "util/numeric/nibble_tables.h"
	Includable small tables, useful for bit operations.  
	See "util/bitmanip.h" for built-in functions.  
	$Id: nibble_tables.h,v 1.6 2007/08/29 18:56:45 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_NIBBLE_TABLES_H__
#define	__UTIL_NUMERIC_NIBBLE_TABLES_H__

// #include "util/numeric/inttype_traits.h"

namespace util {
namespace numeric {

/**
	Lookup tables of MSB positions, indexed by nibble (4 bits).  
	Each 4-bit pattern yields an int that corresponds to the MSB
	position, or -1 if there are no 1s.  

	Given static linkage and included with every translation unit
	to avoid the problem of global static initialization ordering, 
	especially if this is needed by allocators during static 
	initialization.  
 */
static
const char nibble_MSB_position[16] = {
	-1, 0, 1, 1, 
	2, 2, 2, 2, 
	3, 3, 3, 3, 
	3, 3, 3, 3
};

extern
const char nibble_LSB_position[16];

extern
const char nibble_FS_position[16];

extern
const char nibble_popcount[16];

extern
const char nibble_parity[16];

extern
const char nibble_LZ[16];

/**
	\return true if this module has been statically, globally initialized.
	Typically one asserts this as a safety check.  
 */
static
inline
bool
nibble_tables_initialized(void) {
	return nibble_LSB_position[0];
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_NIBBLE_TABLES_H__

