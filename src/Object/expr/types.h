/**
	\file "Object/expr/types.h"
	Forward declarations for all HAC::entity classes and typedefs.
	NOTE: this file was moved from "Object/art_object_expr_types.h"
	$Id: types.h,v 1.3 2005/12/13 04:15:27 fang Exp $
 */

#ifndef	__OBJECT_EXPR_TYPES_H__
#define	__OBJECT_EXPR_TYPES_H__

namespace HAC {
namespace entity {
	/**
		The global integer-type for parameter integers.  
		This may have to be changed to int32 in the future, 
		for 64b portability...
	 */
	typedef	long		pint_value_type;

	/**
		The global boolean-type for parameter integers.  
	 */
	typedef	bool		pbool_value_type;

	/**
		Type used to represent built in int<W> types.  
		For arbitrary precision integers, will want to use
		mpz_class from GMP, or cl_z from CLN.  
	 */
	typedef	long		int_value_type;

	/**
		Representation of built in bool data type.  
		Intuitive choice.  
	 */
	typedef	bool		bool_value_type;

	/**
		For now, limiting to 256 enumerations.  
	 */
	typedef	char		enum_value_type;

#if 1
	class			struct_value_type;
#else
	/**
		Generic meta-type for structured values.  
	 */
	typedef	something_or_other	struct_value_type;
#endif

}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_EXPR_TYPES_H__

