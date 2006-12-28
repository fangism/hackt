/**
	\file "sim/chpsim/nonmeta_context.cc"
	$Id: nonmeta_context.cc,v 1.1.4.3 2006/12/28 04:28:20 fang Exp $
 */

#include <vector>
#include "sim/chpsim/nonmeta_context.h"
#include "sim/chpsim/Event.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/traits/proc_traits.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using entity::process_tag;
//=============================================================================
// class nonmeta_context method definitions

/**
	If process_index is 0 duplicate the top-level footprint as the 
	local footprint, else use the global entry's local footprint pointer.  
 */
nonmeta_context::nonmeta_context(const state_manager& s, 
		const footprint& f, 
		nonmeta_state_manager& v, 
		event_type& e, 
		event_subscribers_type& r, 
		enqueue_queue_type& q) :
		nonmeta_context_base(s, f, 
			(e.get_process_index() ?
				&s.get_pool<process_tag>()
				[e.get_process_index()]._frame
				: NULL),
			v),
		event(e), 
		rechecks(r), 
		queue(q) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_context::~nonmeta_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_context::enqueue(const size_t i) {
	queue.push_back(i);
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

