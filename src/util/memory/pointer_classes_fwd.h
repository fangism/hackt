/**
	\file "pointer_classes_fwd.h"
	Forward declarations for non-counted pointer classes in 
	"pointer_classes.h".
	$Id: pointer_classes_fwd.h,v 1.1 2004/11/28 23:46:12 fang Exp $
 */

#ifndef	__POINTER_CLASSES_FWD_H__
#define	__POINTER_CLASSES_FWD_H__

namespace util {
namespace memory {

template <class>	class some_ptr;		// sometimes owner
// template <class>	class some_const_ptr;	// sometimes owner
template <class>	class excl_ptr;		// exclusive owner and deleter
// template <class>	class excl_const_ptr;	// exclusive owner and deleter
template <class>	class never_ptr;	// never owner
// template <class>	class never_const_ptr;	// never owner

}	// end namespace memory
}	// end namespace util

#endif	//	__POINTER_CLASSES_FWD_H__

