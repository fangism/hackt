/**
	\file "Object/expr/types.h"
	Forward declarations for all HAC::entity classes and typedefs.
	This is the ONE file that needs to be touched to change the 
	precisions of meta-parameter (and simulation) values globally.  
	NOTE: this file was moved from "Object/art_object_expr_types.h"
	$Id: types.h,v 1.8 2010/09/21 00:18:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_TYPES_H__
#define	__HAC_OBJECT_EXPR_TYPES_H__

#if defined(BUILT_HACKT) || defined(INSTALLED_HACKT)
#include <string>
#else
#include "util/string_fwd.h"	// depends on "config.h"
#endif

namespace HAC {
namespace entity {
// meta-parameter value types
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
		Real-value parameters, default to single-precision.  
	 */
	typedef	float		preal_value_type;
	/**
		Native string type.  
	 */
	typedef	std::string		pstring_value_type;


// non-parameter value types
	/**
		Type used to represent built in int<W> types.  
		For arbitrary precision integers, will want to use
		mpz_class from GMP, or cl_z from CLN.  
	 */
	typedef	pint_value_type		int_value_type;

	/**
		Representation of built in bool data type.  
		Intuitive choice.  
	 */
	typedef	pbool_value_type	bool_value_type;

	/**
		Don't know how this is going to work.
		PUNT!
	 */
	typedef	preal_value_type	real_value_type;

	/**
		Native string type.  
	 */
	typedef	pstring_value_type	string_value_type;

	/**
		For now, limiting to 256 enumerations.  
	 */
	typedef	char		enum_value_type;

#if 1
	// not yet defined
	class			struct_value_type;
#else
	/**
		Generic meta-type for structured values.  
	 */
	typedef	something_or_other	struct_value_type;
#endif

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_TYPES_H__

