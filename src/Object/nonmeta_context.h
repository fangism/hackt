/**
	\file "Object/nonmeta_context.h"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.h,v 1.1.2.3 2006/12/20 20:36:42 fang Exp $
 */
#ifndef	__HAC_OBJECT_NONMETA_CONTEXT_H__
#define	__HAC_OBJECT_NONMETA_CONTEXT_H__

#include "util/size_t.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
	class EventNode;
}	// end namespace CHPSIM
}	// end namespace SIM
namespace entity {

class state_manager;		// for structural information
class nonmeta_state_manager;	// for run-time value information

//=============================================================================
/**
	Context information for lookup up run-time values from state.  
	This is all that is needed to lookup run-time values and references.  
 */
class nonmeta_context_base {
protected:
	/**
		Read-only structural information including
		footprint frames.  
	 */
	const state_manager&			sm;
	/**
		Run-time data, modifiable.  
	 */
	nonmeta_state_manager&			values;

	nonmeta_context_base(const state_manager& s, 
		nonmeta_state_manager& v) : sm(s), values(v) { }

	// default copy-ctor
	// default dtor

};	// end class nonmeta_context_base

//=============================================================================
/**
	Context information for lookup up run-time values from state.  
	This is now tied to CHPSIM data structures.  
 */
class nonmeta_context : public nonmeta_context_base {
	typedef	SIM::CHPSIM::EventNode		event_type;
	typedef	std::default_vector<size_t>::type	enqueue_queue_type;
	/**
		Global process index, for looking up footprint frames.  
		Zero-value means top-level.
	 */
	event_type&				event;
	/**
		This is where to enqueue successors to evaluate.  
	 */
	enqueue_queue_type&			queue;
public:
	nonmeta_context(const state_manager&, nonmeta_state_manager&, 
		event_type&, enqueue_queue_type&);

	~nonmeta_context();

};	// end class nonmeta_context

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_CONTEXT_H__

