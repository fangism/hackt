/**
	\file "Object/nonmeta_context.cc"
	$Id: nonmeta_context.cc,v 1.1.2.3.2.1 2006/12/22 04:10:48 fang Exp $
 */

#include "Object/nonmeta_context.h"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_context method definitions

nonmeta_context::nonmeta_context(const state_manager& s, 
		const footprint& f, 
		nonmeta_state_manager& v, event_type& e, 
		enqueue_queue_type& q) :
		nonmeta_context_base(s, f, v), event(e), queue(q) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_context::~nonmeta_context() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

