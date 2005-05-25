/**
	\file "Object/art_object_CHP.h"
	Class definitions for CHP-related objects.  
	$Id: art_object_CHP.h,v 1.1.2.1 2005/05/25 00:41:46 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CHP_H__
#define	__OBJECT_ART_OBJECT_CHP_H__

#include <list>
#include "Object/art_object_CHP_base.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
namespace CHP {
using std::list;
using util::memory::count_ptr;
//=============================================================================
/**
	Typical action list.  
 */
typedef	list<count_ptr<action> >		action_list_type;

//=============================================================================
/**
	Sequential CHP actions.  
 */
class action_sequence : public action {
};	// end class action_sequence

//=============================================================================
/**
	Concurrent CHP actions.  
 */
class concurrent_actions : public action {
};	// end class concurrent_actions

//=============================================================================
// class for tight concurrency? (bullet)
//=============================================================================
class guarded_action {

};	// end class guarded_action

//=============================================================================
/**
	Typical guarded statement list.  
 */
typedef	list<count_ptr<guarded_action> >	selection_list_type;

//=============================================================================
/**
	Deterministic selection statement.  
 */
class deterministic_selection : public action {

};	// end class deterministic_selection

//=============================================================================
/**
	Non-deterministic selection statement.  
 */
class nondeterministic_selection : public action {

};	// end class nondeterministic_selection

//=============================================================================
/**
	Variable assignment.  
 */
class assignment : public action {

};	// end class assignment

//=============================================================================
// no skip action, just left out!
//=============================================================================
class condition_wait : public action {
};	// end class condition_wait

//=============================================================================
class channel_send : public action {
};	// end class channel_send

//=============================================================================
class channel_receive : public action {
};	// end class channel_receive

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CHP_H__

