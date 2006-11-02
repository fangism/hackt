/**
	\file "Object/def/footprint.tcc"
	Exported template implementation of footprint base class. 
	$Id: footprint.tcc,v 1.1 2006/11/02 22:01:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_TCC__
#define	__HAC_OBJECT_DEF_FOOTPRINT_TCC__

#include "Object/def/footprint.h"

namespace HAC {
namespace entity {
//=============================================================================
// class footprint_base method definitions

template <class Tag>
footprint_base<Tag>::footprint_base() :
		_pool(class_traits<Tag>::instance_pool_chunk_size >> 1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
footprint_base<Tag>::~footprint_base() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_TCC__

