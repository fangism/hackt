/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.3.14.1 2005/12/11 00:45:42 fang Exp $
 */

#ifndef	__OBJECT_LANG_CHP_BASE_H__
#define	__OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.h"

namespace HAC {
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
}	// end namespace HAC

#endif	// __OBJECT_LANG_CHP_BASE_H__

