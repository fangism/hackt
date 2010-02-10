/**
	\file "Object/lang/cflat_visitee.h"
	TODO: rename and organize files?
	$Id: cflat_visitee.h,v 1.2.154.1 2010/02/10 06:43:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_VISITEE_H__
#define	__HAC_OBJECT_LANG_CFLAT_VISITEE_H__

#include "util/visitee.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
struct global_entry_context;
#endif
namespace PRS {
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
// forward declaration
struct cflat_visitor;
#endif

/**
	Base class for cflat visitable instance hierarchy.  
	NOTE: we forbid modifications to the hierarchy by enforcing constness.
 */
typedef	util::visitee_const<
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		global_entry_context
#else
		cflat_visitor
#endif
		, void>	cflat_visitee;

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_VISITEE_H__

