/**
	\file "art_object_index.h"
	Typedef for index types.  
	$Id: art_object_index.h,v 1.1.2.3 2005/01/20 18:29:54 fang Exp $
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
	Used in "art_object_expr.cc".
 */
typedef	util::multikey_base<pint_value_type>		multikey_index_type;

/**
	Base type for multidimensional key ranged iterators, used to 
	walk subslices of multidimensional structures.  
	Used in "art_object_expr.cc".
 */
typedef	util::multikey_generator_base<pint_value_type>	index_generator_type;

/**
	Index type used for referencing unrolling instance collections.  
	Used in virtually all instance_collection_base children classes.  
 */
typedef	util::multikey_base<pint_value_type>		unroll_index_type;

}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INDEX_H__

