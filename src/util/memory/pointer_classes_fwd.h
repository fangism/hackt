/**
	\file "pointer_classes_fwd.h"
	Forward declarations for non-counted pointer classes in 
	"pointer_classes.h".
	$Id: pointer_classes_fwd.h,v 1.2 2004/11/30 01:26:06 fang Exp $
 */

#ifndef	__POINTER_CLASSES_FWD_H__
#define	__POINTER_CLASSES_FWD_H__

namespace util {
namespace memory {

template <class>	class some_ptr;		// sometimes owner
template <class>	class excl_ptr;		// exclusive owner and deleter
template <class>	class sticky_ptr;	// like excl, but sticky
template <class>	class unique_ptr;
template <class>	class never_ptr;	// never owner
template <class>	class count_ptr;	// reference-counted

}	// end namespace memory
}	// end namespace util

#endif	//	__POINTER_CLASSES_FWD_H__

