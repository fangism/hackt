/**
	\file "Object/inst/preal_value_collection.hh"
	Parameter instance collection classes for HAC.  
	$Id: preal_value_collection.hh,v 1.3 2006/02/21 04:48:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_H__

#include "Object/inst/param_value_collection.hh"
#include "Object/inst/value_collection.hh"
#include "Object/expr/preal_const.hh"
#include "Object/traits/preal_traits.hh"
#include "Object/inst/preal_instance.hh"

namespace HAC {
namespace entity {
struct preal_tag;
using util::memory::count_ptr;

template <class, size_t>
class value_array;

//=============================================================================
// convenient typedefs

typedef	value_array<preal_tag,0>	preal_scalar;
typedef	value_array<preal_tag,1>	preal_array_1D;
typedef	value_array<preal_tag,2>	preal_array_2D;
typedef	value_array<preal_tag,3>	preal_array_3D;
typedef	value_array<preal_tag,4>	preal_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_H__

