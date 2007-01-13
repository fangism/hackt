/**
	\file "Object/ref/reference_enum.h"
	Forward declarations and typedefs for indexed/enumerated references.  
	$Id: reference_enum.h,v 1.1.2.1 2007/01/13 02:08:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_REFERENCE_ENUM_H__
#define	__HAC_OBJECT_REF_REFERENCE_ENUM_H__

#include "Object/traits/type_tag_enum.h"
#include <utility>
#include "util/STL/vector_fwd.h"
#include "util/size_t.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	First member: meta-type enum, see traits's type_tag_enum.
	Second member: index (from global allocation).  
	NOTE: size was chose to align naturally to machine integer boundary.  
 */
typedef	std::pair<size_t, size_t>	global_indexed_reference;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Array container of reference.  
	Used to accumulate heterogenous collections of instance references.  
 */
typedef	std::default_vector<global_indexed_reference>::type
				global_reference_array_type;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_REFERENCE_ENUM_H__

