/**
	\file "art_object_IO.cc"
	Template method instantiations for 
	persistent object management, specific to ART.  
 	$Id: art_object_IO.cc,v 1.13.12.1 2005/01/26 20:55:00 fang Exp $
 */

#ifndef	__ART_OBJECT_IO_CC__
#define	__ART_OBJECT_IO_CC__

#define	ENABLE_STACKTRACE		1

#include "art_object_module.h"
#include "persistent_object_manager.tcc"

namespace util {
using namespace util::memory;
using ART::entity::module;

EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(module)

}	// end namespace util

#endif	// __ART_OBJECT_IO_CC__

