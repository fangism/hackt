/**
	\file "Object/common/multikey_index.hh"
	Typedef for index types.  
	This file was moved from "Object/common/multikey_index.h".
	$Id: multikey_index.hh,v 1.4 2006/01/22 18:19:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_MULTIKEY_INDEX_H__
#define	__HAC_OBJECT_COMMON_MULTIKEY_INDEX_H__

#include "util/multikey_fwd.hh"
#include "Object/expr/types.hh"	// for pint/pbool_value_type

namespace HAC {
namespace entity {

/**
	Base type for multidimensional keys used in manipulating indices, 
	espeicially in instance references.  
	This is actually an abstract type with a static generator function.  
	Used in "art_object_expr.cc".

	Index type used for referencing unrolling instance collections.  
	Used in virtually all instance_collection_base children classes.  
 */
typedef	util::multikey_generic<pint_value_type>		multikey_index_type;


/**
	Generic (concrete) multidimensional key generator.
	Used during unrolling resolution, in "art_object_expr.cc".
	Consider using this instead of the base type interface
	for efficiency.  
 */
typedef	util::multikey_generator_generic<pint_value_type>
						generic_index_generator_type;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_MULTIKEY_INDEX_H__

