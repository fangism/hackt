/**
	\file "util/memory/pointer_classes_fwd.hh"
	Forward declarations for non-counted pointer classes in 
	Note: the double declarations with default argument the 2nd time
		facilitates defaulting!
	"util/memory/pointer_classes.hh".
	$Id: pointer_classes_fwd.hh,v 1.6 2006/01/22 06:53:44 fang Exp $
 */

#ifndef	__UTIL_MEMORY_POINTER_CLASSES_FWD_H__
#define	__UTIL_MEMORY_POINTER_CLASSES_FWD_H__

namespace util {
namespace memory {
//=============================================================================
// the default deallocation policy
// policies are defined in "util/memory/deallocation_policy.hh"
struct delete_tag;
/**
	For convenience, we typedef a default deallocation policy
	to the one corresponding to delete.
	You probably do NOT want to change this.  
 */
typedef	delete_tag	default_deallocation_policy;

template <class, class>
class some_ptr;		// sometimes owner
template <class, class = default_deallocation_policy>
class some_ptr;		// sometimes owner

#if 1
// template <class T, class TP = T*>
// template <class T, class TP>
// template <class T>
template <class, class>
class excl_ptr;		// exclusive owner and deleter
template <class, class = default_deallocation_policy>
class excl_ptr;		// exclusive owner and deleter

#define	DEFAULT_EXCL_PTR(T)	util::memory::excl_ptr<T,T*>
#endif

template <class, class>
class sticky_ptr;	// like excl, but sticky
template <class, class = default_deallocation_policy>
class sticky_ptr;	// like excl, but sticky

template <class, class>
class unique_ptr;
template <class, class = default_deallocation_policy>
class unique_ptr;

// never deallocates, thus, needs no deallocator policy
template <class>
class never_ptr;	// never owner

template <class, class>
class count_ptr;	// reference-counted
template <class, class = default_deallocation_policy>
class count_ptr;

/**
	This never deletes but still needs to know the deallocation
	policy for type-safety -- it transfers to other things that delete.  
 */
template <class, class>
struct raw_count_ptr;	// reference-counted helper
template <class, class = default_deallocation_policy>
struct raw_count_ptr;

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_POINTER_CLASSES_FWD_H__

