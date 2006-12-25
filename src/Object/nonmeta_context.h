/**
	\file "Object/nonmeta_context.h"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.h,v 1.1.2.4 2006/12/25 03:27:29 fang Exp $
 */
#ifndef	__HAC_OBJECT_NONMETA_CONTEXT_H__
#define	__HAC_OBJECT_NONMETA_CONTEXT_H__

#include "util/size_t.h"

namespace HAC {
namespace entity {
class footprint;
class footprint_frame;
class state_manager;		// for structural information
class nonmeta_state_manager;	// for run-time value information

//=============================================================================
/**
	Context information for lookup up run-time values from state.  
	This is all that is needed to lookup run-time values and references.  
 */
class nonmeta_context_base {
public:
	/**
		Read-only structural information including
		footprint frames.  
	 */
	const state_manager&			sm;
	/**
		Top-level footprint.
	 */
	const footprint&			topfp;
	/**
		Local lookup-footprint.
	 */
	const footprint_frame* const		fpf;
	/**
		Run-time data, modifiable.  
	 */
	nonmeta_state_manager&			values;

protected:
	nonmeta_context_base(const state_manager& s, 
		const footprint& f, const footprint_frame* l, 
		nonmeta_state_manager& v) : 
		sm(s), topfp(f), fpf(l), values(v) { }


	// default copy-ctor
	// default dtor

};	// end class nonmeta_context_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_CONTEXT_H__

