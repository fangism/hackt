/**
	\file "util/memory/pointer_classes_fwd.h"
	Forward declarations for non-counted pointer classes in 
	"util/memory/pointer_classes.h".
	$Id: pointer_classes_fwd.h,v 1.3.4.1 2005/05/17 21:48:45 fang Exp $
 */

#ifndef	__UTIL_MEMORY_POINTER_CLASSES_FWD_H__
#define	__UTIL_MEMORY_POINTER_CLASSES_FWD_H__

namespace util {
namespace memory {

template <class>	class some_ptr;		// sometimes owner
template <class>	class excl_ptr;		// exclusive owner and deleter
template <class>	class sticky_ptr;	// like excl, but sticky
template <class>	class unique_ptr;
template <class>	class never_ptr;	// never owner
template <class>	class count_ptr;	// reference-counted
template <class>	class raw_count_ptr;	// reference-counted helper

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_POINTER_CLASSES_FWD_H__

