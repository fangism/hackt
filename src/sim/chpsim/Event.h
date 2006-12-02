/**
	\file "sim/chpsim/Event.h"
	Various classes of chpsim events.  
	$Id: Event.h,v 1.1.2.1 2006/12/02 22:10:11 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENT_H__
#define	__HAC_SIM_CHPSIM_EVENT_H__

#include "util/size_t.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {

//=============================================================================
/**
	Plan is to have different event pools.  
 */
enum {
	EVENT_NULL = 0,
	EVENT_ASSIGN,
	EVENT_SEND,
	EVENT_RECEIVE
	// flow control events? loops? sequencing, and concurrency?
	// build up successor/predecessor chains?
};

//-----------------------------------------------------------------------------
/**
	Event representing change of variable value.  
	For now, we assign bools and ints, don't deal with structures yet.
	The lvalue reference is the result of run-time resolution, 
	in the case of non-meta indexed variables.  
	The rvalue is a resolved/updated value to assign.  
	Could also template this by type... bool, int.
	Aggregate assignments? not yet.
 */
struct assign_event {
	// lvalue -- some resolved reference to lvalue (bool, int)
	size_t				lvalue_type;
	// would be best to resolve ASAP
	size_t				lvalue_index;
	// the value to assign (currently, only integer)
	size_t				rvalue;
};	// end struct assign_event

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct send_event {
	size_t				lvalue_channel_index;
	// rvalue must be aggregate structure
};	// end struct send_event

//=============================================================================
/**
	The master event record for the event queue.  
	Q: TimeType? integer of floating point?
 */
struct event_placeholder {
	size_t				event_type;
	size_t				event_index;
};	// end struct event_placeholder

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_EVENT_H__

