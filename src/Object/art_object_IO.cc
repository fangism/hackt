/**
	\file "art_object_IO.cc"
	Template method instantiations for 
	persistent object management, specific to ART.  
 	$Id: art_object_IO.cc,v 1.11 2004/11/05 02:38:23 fang Exp $
 */

#include "art_object_module.h"
#include "persistent_object_manager.tcc"

namespace util {
using namespace PTRS_NAMESPACE;
using ART::entity::module;

EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(module)

}	// end namespace util

