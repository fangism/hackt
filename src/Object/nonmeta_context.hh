/**
	\file "Object/nonmeta_context.hh"
	This is used to lookup run-time values and references.  
	$Id: nonmeta_context.hh,v 1.4 2010/04/07 00:12:30 fang Exp $
 */
#ifndef	__HAC_OBJECT_NONMETA_CONTEXT_H__
#define	__HAC_OBJECT_NONMETA_CONTEXT_H__

#include "util/size_t.h"
#include "Object/global_entry_context.hh"

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
	using parent_type::topfp;
	using parent_type::fpf;
	/**
		Run-time data, modifiable.  
	 */
	nonmeta_state_manager&			values;

protected:
	nonmeta_context_base(const global_entry_context& c,
		nonmeta_state_manager& v) : 
		global_entry_context(c), 
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

