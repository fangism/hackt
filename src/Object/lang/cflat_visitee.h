/**
	\file "Object/lang/cflat_visitee.h"
	TODO: rename and organize files?
	$Id: cflat_visitee.h,v 1.4 2010/04/07 00:12:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_VISITEE_H__
#define	__HAC_OBJECT_LANG_CFLAT_VISITEE_H__

#include "util/visitee.h"
// #include "Object/devel_switches.h"

namespace HAC {
namespace entity {
namespace PRS {
// forward declaration
class cflat_visitor;

/**
	Base class for cflat visitable instance hierarchy.  
	NOTE: we forbid modifications to the hierarchy by enforcing constness.
 */
typedef	util::visitee_const<cflat_visitor, void>	cflat_visitee;

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_VISITEE_H__

