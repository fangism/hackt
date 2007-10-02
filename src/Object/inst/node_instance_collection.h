/**
	\file "Object/inst/node_instance_collection.h"
	Class declarations for internal nodes.
	$Id: node_instance_collection.h,v 1.1.2.1 2007/10/02 05:15:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
// #include "Object/inst/node_instance.h"
// #include "Object/inst/instance_alias_info.h"
// #include "Object/inst/alias_empty.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/null_collection_type_manager.h"

namespace HAC {
namespace entity {
struct node_tag;

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

//=============================================================================
#if 0
ostream&
operator << (ostream&, const node_instance_alias_info&);
#endif

//-----------------------------------------------------------------------------
// convenient typedefs

typedef	instance_array<node_tag, 0>	node_scalar;
typedef	instance_array<node_tag, 1>	node_array_1D;
typedef	instance_array<node_tag, 2>	node_array_2D;
typedef	instance_array<node_tag, 3>	node_array_3D;
typedef	instance_array<node_tag, 4>	node_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_NODE_INSTANCE_COLLECTION_H__

