/**
	\file "Object/nonmeta_context.h"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.h,v 1.2.76.1 2010/01/09 03:29:59 fang Exp $
 */
#ifndef	__HAC_OBJECT_NONMETA_CONTEXT_H__
#define	__HAC_OBJECT_NONMETA_CONTEXT_H__

#include "util/size_t.h"
#include "Object/global_entry_context.h"

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
	TODO: derive from global_entry_context for consistency?
 */
class nonmeta_context_base : public global_entry_context {
	typedef	global_entry_context		parent_type;
public:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	using parent_type::sm;
#endif
	using parent_type::topfp;
	using parent_type::fpf;
	/**
		Run-time data, modifiable.  
	 */
	nonmeta_state_manager&			values;

protected:
	nonmeta_context_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& s, 
#endif
		const footprint& f, const footprint_frame* const l, 
		nonmeta_state_manager& v) : 
		global_entry_context(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			s,
#endif
			f, l), 
		values(v) { }

	// default copy-ctor
	// default dtor
public:
	template <class reference_type>
	size_t
	lookup_nonmeta_reference_global_index(const reference_type&) const;

};	// end class nonmeta_context_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_CONTEXT_H__

