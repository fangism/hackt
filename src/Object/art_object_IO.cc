/**
	\file "art_object_IO.cc"
	Template method instantiations for 
	persistent object management, specific to ART.  
 */

#include "art_object_module.h"
#include "persistent_object_manager.tcc"

using namespace PTRS_NAMESPACE;
using ART::entity::module;

namespace util {

EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(module)

}	// end namespace util

