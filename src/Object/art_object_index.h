/**
	\file "art_object_index.h"
	Typedef for index types.  
	$Id: art_object_index.h,v 1.2 2005/01/28 19:58:42 fang Exp $
 */

#ifndef	__ART_OBJECT_INDEX_H__
#define	__ART_OBJECT_INDEX_H__

#include "multikey_fwd.h"
#include "art_object_fwd.h"	// for pint/pbool_value_type

namespace ART {
namespace entity {

/**
	Base type for multidimensional keys used in manipulating indices, 
	espeicially in instance references.  
	This is actually an abstract type with a static generator function.  
	Used in "art_object_expr.cc".
 */
typedef	util::multikey_base<pint_value_type>		multikey_index_type;

/**
	Base type for multidimensional key ranged iterators, used to 
	walk subslices of multidimensional structures.  
	This is actually an abstract type with a static generator function.  
	Used in "art_object_expr.cc".
 */
typedef	util::multikey_generator_base<pint_value_type>	index_generator_type;

/**
	Index type used for referencing unrolling instance collections.  
	Used in virtually all instance_collection_base children classes.  
 */
typedef	util::multikey_base<pint_value_type>		unroll_index_type;

/**
	Generic (concrete) multidimensional key generator.
	Used during unrolling resolution, in "art_object_expr.cc".
	Consider using this instead of the base type interface
	for efficiency.  
 */
typedef	util::multikey_generator_generic<pint_value_type>
						generic_index_generator_type;

}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INDEX_H__

