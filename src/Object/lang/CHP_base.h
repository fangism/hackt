/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.2 2005/07/23 06:52:44 fang Exp $
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
class action : public persistent {
public:
	action() { }
virtual	~action() { }

virtual	ostream&
	dump(ostream&) const = 0;

};	// end class action

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CHP_BASE_H__

