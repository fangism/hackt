/**
	\file "Object/art_object_fwd.h"
	Forward declarations for all ART::entity classes and typedefs.
	$Id: art_object_expr_types.h,v 1.1.2.1 2005/06/10 04:16:38 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_EXPR_TYPES_H__
#define	__OBJECT_ART_OBJECT_EXPR_TYPES_H__

namespace ART {
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

}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_EXPR_TYPES_H__

