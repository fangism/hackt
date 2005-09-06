/**
	\file "Object/global_entry.tcc"
	$Id: global_entry.tcc,v 1.1.2.1 2005/09/06 05:56:45 fang Exp $
 */

#ifndef	__OBJECT_GLOBAL_ENTRY_TCC__
#define	__OBJECT_GLOBAL_ENTRY_TCC__

#include "Object/global_entry.h"

namespace ART {
namespace entity {
//=============================================================================
// class global_entry method definitions

template <class Tag>
global_entry<Tag>::global_entry() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry<Tag>::~global_entry() { }

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_GLOBAL_ENTRY_TCC__

