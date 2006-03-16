/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.6 2006/03/16 03:40:26 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_BASE_H__
#define	__HAC_OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.h"

namespace HAC {
namespace entity {
struct expr_dump_context;
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
	dump(ostream&, const expr_dump_context&) const = 0;

};	// end class action

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_BASE_H__

