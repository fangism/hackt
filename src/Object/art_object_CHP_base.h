/**
	\file "Object/art_object_CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: art_object_CHP_base.h,v 1.1.2.1 2005/05/25 00:41:47 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CHP_BASE_H__
#define	__OBJECT_ART_OBJECT_CHP_BASE_H__

#include "util/persistent.h"

namespace ART {
namespace entity {
/**
	Namespace for CHP object classes.  
 */
namespace CHP {
using std::ostream;
using util::persistent;
using util::persistent_object_manager;

//=============================================================================
/**
	Abstract base class for type-checked CHP statements.  
 */
class action {
public:
	action() { }
virtual	~action() { }

virtual	ostream&
	dump(ostream&) = 0;

};	// end class action

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CHP_BASE_H__

