/**
	\file "Object/nonmeta_context.h"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.h,v 1.1.2.1 2006/12/19 23:44:00 fang Exp $
 */
#ifndef	__HAC_OBJECT_NONMETA_CONTEXT_H__
#define	__HAC_OBJECT_NONMETA_CONTEXT_H__

#include "util/size_t.h"

namespace HAC {
namespace entity {

class state_manager;		// for structural information
class nonmeta_state_manager;	// for run-time value information

//=============================================================================
/**
	Context information for lookup up run-time values from state.  
 */
class nonmeta_context {
	/**
		Read-only structural information including
		footprint frames.  
	 */
	const state_manager&			sm;
	/**
		Run-time data, modifiable.  
	 */
	nonmeta_state_manager&			values;
	/**
		Global process index, for looking up footprint frames.  
		Zero-value means top-level.
	 */
	const size_t				process_index;
public:
	nonmeta_context(const state_manager&, nonmeta_state_manager&, 
		const size_t);

	~nonmeta_context();

};	// end class nonmeta_context

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_CONTEXT_H__

