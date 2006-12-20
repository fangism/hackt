/**
	\file "Object/nonmeta_context.cc"
	$Id: nonmeta_context.cc,v 1.1.2.2 2006/12/20 08:33:14 fang Exp $
 */

#include "Object/nonmeta_context.h"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_context method definitions

nonmeta_context::nonmeta_context(const state_manager& s, 
		nonmeta_state_manager& v, event_type& e, 
		enqueue_queue_type& q) :
		sm(s), values(v), event(e), queue(q) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_context::~nonmeta_context() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

