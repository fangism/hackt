/**
	\file "art_object_IO.cc"
	Template method instantiations for 
	persistent object management, specific to ART.  
 	$Id: art_object_IO.cc,v 1.10 2004/11/02 07:51:44 fang Exp $
 */

#include "art_object_module.h"
#include "persistent_object_manager.tcc"

using namespace PTRS_NAMESPACE;
using ART::entity::module;

namespace util {

EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(module)

}	// end namespace util

