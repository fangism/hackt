/**
	\file "Object/cflat_context.h"
	This file just provides typedefs.  
	Structure containing all the minimal information
	needed for a cflat traversal over instances.  
	$Id: cflat_context.h,v 1.4 2006/02/04 06:43:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_CFLAT_CONTEXT_H__
#define	__HAC_OBJECT_CFLAT_CONTEXT_H__

#include "Object/global_entry_context.h"

namespace HAC {
namespace entity {
typedef	global_entry_context_base		cflat_context_base;
typedef	global_entry_context			cflat_context;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_CFLAT_CONTEXT_H__

