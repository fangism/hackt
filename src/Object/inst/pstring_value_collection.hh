/**
	\file "Object/inst/pstring_value_collection.hh"
	Parameter instance collection classes for HAC.  
	$Id: pstring_value_collection.hh,v 1.2 2010/09/21 00:18:23 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PSTRING_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PSTRING_VALUE_COLLECTION_H__

#include "Object/inst/param_value_collection.hh"
#include "Object/inst/value_collection.hh"
#include "Object/expr/pstring_const.hh"
#include "Object/traits/pstring_traits.hh"
#include "Object/inst/pstring_instance.hh"

namespace HAC {
namespace entity {
struct pstring_tag;
using util::memory::count_ptr;

template <class, size_t>
class value_array;

//=============================================================================
// convenient typedefs

typedef	value_array<pstring_tag,0>	pstring_scalar;
typedef	value_array<pstring_tag,1>	pstring_array_1D;
typedef	value_array<pstring_tag,2>	pstring_array_2D;
typedef	value_array<pstring_tag,3>	pstring_array_3D;
typedef	value_array<pstring_tag,4>	pstring_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PSTRING_VALUE_COLLECTION_H__

