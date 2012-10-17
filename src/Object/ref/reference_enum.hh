/**
	\file "Object/ref/reference_enum.hh"
	Forward declarations and typedefs for indexed/enumerated references.  
	$Id: reference_enum.hh,v 1.4 2010/08/24 21:05:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_REFERENCE_ENUM_H__
#define	__HAC_OBJECT_REF_REFERENCE_ENUM_H__

#include <utility>
#include <vector>

#include "Object/traits/type_tag_enum.hh"
#include "Object/expr/types.hh"
#include "util/packed_array_fwd.hh"
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

/**
	Return type for references to dense array collections.
 */
typedef	util::packed_array_generic<pint_value_type, size_t>
					index_array_reference;
/**
	Same, but paired with meta-type tag.
 */
typedef	std::pair<size_t, index_array_reference>
					typed_index_array_reference;

static
inline
global_indexed_reference
make_global_reference(const size_t f, const size_t s) {
	return global_indexed_reference(f, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Array container of reference.  
	Used to accumulate heterogenous collections of instance references.  
 */
typedef	std::vector<global_indexed_reference>
				global_reference_array_type;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_REFERENCE_ENUM_H__

