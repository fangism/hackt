/**
	\file "numeric/nibble_tables.h"
	Includable small tables, useful for bit operations.  
	$Id: nibble_tables.h,v 1.2 2005/03/06 04:36:49 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_NIBBLE_TABLES_H__
#define	__UTIL_NUMERIC_NIBBLE_TABLES_H__

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

//=============================================================================
/**
	MSB position template functors.  
	We provide specializations for some basic unsigned types.  
 */
template <class T>
struct MSB_position;

template <>
struct MSB_position<unsigned char> {
	typedef	unsigned char	arg_type;

	/**
		Note that this does NOT check the range of c.
		The caller should guarantee that it is NOT zero.  
		\param c the byte to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type c) const {
		return (c > 0xF) ?
			nibble_MSB_position[(c >> 4)] +4 :
			nibble_MSB_position[c];
	}
};

template <>
struct MSB_position<unsigned short> {
	typedef	unsigned short	arg_type;
	typedef	unsigned char	half_type;

	/**
		\param c the short to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type s) const {
		return (s > 0xFF) ?
			MSB_position<half_type>()(half_type(s >> 8)) +8 :
			MSB_position<half_type>()(half_type(s));
	}
};

template <>
struct MSB_position<unsigned int> {
	typedef	unsigned int	arg_type;
	typedef	unsigned short	half_type;

	/**
		\param c the int to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type s) const {
		return (s > 0xFFFF) ?
			MSB_position<half_type>()(half_type(s >> 16)) +16 :
			MSB_position<half_type>()(half_type(s));
	}
};

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_NIBBLE_TABLES_H__

