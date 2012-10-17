/**
	\file "Object/inst/port_alias_tracker.tcc"
	Selectively exported template methods.  
	$Id: port_alias_tracker.tcc,v 1.1 2006/11/02 22:02:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_ALIAS_TRACKER_TCC__
#define	__HAC_OBJECT_INST_PORT_ALIAS_TRACKER_TCC__

#include "Object/inst/port_alias_tracker.hh"
#include "Object/inst/instance_alias_info.hh"

#include "util/macros.h"
// #include "util/stacktrace.hh"

namespace HAC {
namespace entity {

//=============================================================================
// class alias_reference_set method definitions

template <class Tag>
alias_reference_set<Tag>::alias_reference_set() : alias_array()
#if USE_ALIAS_STRING_CACHE
	, cache()
#endif
	{
	alias_array.reserve(2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
alias_reference_set<Tag>::~alias_reference_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::push_back(const alias_ptr_type a) {
        NEVER_NULL(a);
        alias_array.push_back(a);
#if USE_ALIAS_STRING_CACHE
        cache.valid = false;
#endif
}

//=============================================================================
// class port_alias_tracker_base method definitions

template <class Tag>
port_alias_tracker_base<Tag>::port_alias_tracker_base() : _ids() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
port_alias_tracker_base<Tag>::~port_alias_tracker_base() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_ALIAS_TRACKER_TCC__

