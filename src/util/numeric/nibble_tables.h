/**
	\file "util/numeric/nibble_tables.h"
	Includable small tables, useful for bit operations.  
	$Id: nibble_tables.h,v 1.4 2005/08/08 16:51:16 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_NIBBLE_TABLES_H__
#define	__UTIL_NUMERIC_NIBBLE_TABLES_H__

#include "util/numeric/inttype_traits.h"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialized implementation of evaluating the position of the MSB.  
 */
template <>
struct MSB_position<uint8> {
	typedef	uint8		arg_type;
	enum {	half_size = 4 };
	static const arg_type	half_mask = 0xF;

	/**
		Note that this does NOT check the range of c.
		The caller should guarantee that it is NOT zero.  
		\param c the byte to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type c) const {
		return (c > half_mask) ?
			nibble_MSB_position[(c >> half_size)] +half_size :
			nibble_MSB_position[c];
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	General implementation for evaluating the position of the MSB.  
	Recursively halves the bit field in question.  
 */
template <class U>
struct MSB_position {
	typedef	U					arg_type;
	typedef	typename half_type<arg_type>::type	half_type;
	// number of bytes to number of bits
	enum {	half_size = sizeof(half_type) << 3 };
	static const half_type half_mask;

	/**
		\param c the int to lookup MSB.
		\pre c must be non-zero.
	 */
	char
	operator () (const arg_type s) const {
		return (s > half_mask) ?
			MSB_position<half_type>()(half_type(s >> half_size))
				+half_size :
			MSB_position<half_type>()(half_type(s));
	}
};	// end struct MSB_position

template <class U>
const typename MSB_position<U>::half_type
MSB_position<U>::half_mask = half_type(-1);

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_NIBBLE_TABLES_H__

