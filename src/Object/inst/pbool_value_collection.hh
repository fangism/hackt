/**
	\file "Object/inst/pbool_value_collection.hh"
	Parameter instance collection classes for HAC.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: pbool_value_collection.hh,v 1.6 2006/02/21 04:48:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PBOOL_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PBOOL_VALUE_COLLECTION_H__

#include "Object/inst/param_value_collection.hh"
#include "Object/inst/value_collection.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/traits/pbool_traits.hh"
#include "Object/inst/pbool_instance.hh"

namespace HAC {
namespace entity {
struct pbool_tag;
using util::memory::count_ptr;

template <class, size_t>
class value_array;

//=============================================================================
// convenient typedefs

typedef	value_array<pbool_tag,0>	pbool_scalar;
typedef	value_array<pbool_tag,1>	pbool_array_1D;
typedef	value_array<pbool_tag,2>	pbool_array_2D;
typedef	value_array<pbool_tag,3>	pbool_array_3D;
typedef	value_array<pbool_tag,4>	pbool_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PBOOL_VALUE_COLLECTION_H__

