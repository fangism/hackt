/**
	\file "Object/unroll/channel_instantiation_statement.hh"
	Contains definition of nested, specialized class_traits types.  
	This file came from "Object/art_object_inst_stmt_chan.h"
		in a previous life.  
	$Id: channel_instantiation_statement.hh,v 1.5 2007/01/21 05:59:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_CHANNEL_INSTANTIATION_STATEMENT_CHAN_H__
#define	__HAC_OBJECT_UNROLL_CHANNEL_INSTANTIATION_STATEMENT_CHAN_H__

#include "Object/traits/chan_traits.hh"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/type/builtin_channel_type_reference.hh"
#else
#include "Object/type/channel_type_reference_base.hh"
#endif
#include "Object/inst/channel_instance_collection.hh"
#include "Object/inst/general_collection_type_manager.hh"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/unroll/channel_instantiation_type_ref_base.hh"
#else
#include "Object/unroll/instantiation_statement_type_ref_default.hh"
#endif

namespace HAC {
namespace entity {
class param_expr_list;
class const_param_expr_list;
#include "util/using_ostream.hh"

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_CHANNEL_INSTANTIATION_STATEMENT_CHAN_H__

