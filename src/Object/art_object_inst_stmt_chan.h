/**
	\file "Object/art_object_inst_stmt_chan.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_chan.h,v 1.5.2.2 2005/07/22 00:24:53 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_CHAN_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_CHAN_H__

#include <iostream>
#include "Object/traits/chan_traits.h"
#include "Object/type/channel_type_reference_base.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "util/persistent_object_manager.h"
#include "Object/art_object_inst_stmt_type_ref_default.h"

namespace ART {
namespace entity {
class param_expr_list;
class const_param_expr_list;
#include "util/using_ostream.h"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_CHAN_H__

