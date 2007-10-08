/**
	\file "Object/inst/node_instance_collection.h"
	Class declarations for internal nodes.
	Should be named "node_instance_placeholder.h"...
	$Id: node_instance_collection.h,v 1.2 2007/10/08 01:21:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_H__

#include "Object/inst/collection_fwd.h"
#include "Object/inst/dummy_placeholder.h"
#include "Object/traits/node_traits.h"

namespace HAC {
namespace entity {

//=============================================================================
#if 0
ostream&
operator << (ostream&, const node_instance_alias_info&);
#endif

//-----------------------------------------------------------------------------
// convenient typedefs

#if 0
typedef	instance_array<node_tag, 0>	node_scalar;
typedef	instance_array<node_tag, 1>	node_array_1D;
typedef	instance_array<node_tag, 2>	node_array_2D;
typedef	instance_array<node_tag, 3>	node_array_3D;
typedef	instance_array<node_tag, 4>	node_array_4D;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_H__

