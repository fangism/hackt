/**
	\file "Object/inst/pint_value_collection.h"
	Parameter instance collection classes for HAC.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: pint_value_collection.h,v 1.5.16.1 2006/02/20 05:29:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PINT_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PINT_VALUE_COLLECTION_H__

#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/pint_traits.h"
#include "Object/inst/pint_instance.h"

namespace HAC {
namespace entity {
struct pint_tag;
using util::memory::count_ptr;

template <class, size_t>
class value_array;

//=============================================================================
// convenient typedefs

typedef	value_array<pint_tag,0>	pint_scalar;
typedef	value_array<pint_tag,1>	pint_array_1D;
typedef	value_array<pint_tag,2>	pint_array_2D;
typedef	value_array<pint_tag,3>	pint_array_3D;
typedef	value_array<pint_tag,4>	pint_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PINT_VALUE_COLLECTION_H__

