/**
	\file "art_object_IO.cc"
	Template method instantiations for 
	persistent object management, specific to ART.  
 	$Id: art_object_IO.cc,v 1.12 2004/11/30 01:25:08 fang Exp $
 */

#include "art_object_module.h"
#include "persistent_object_manager.tcc"

namespace util {
using namespace util::memory;
using ART::entity::module;

EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(module)

}	// end namespace util

